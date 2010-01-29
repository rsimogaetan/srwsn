#include <omnetpp.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdlib.h>  // For rand
#include <time.h>  // For rand
#include <stdint.h>  // Use [u]intN_t if you need exactly N bits.

#include "SR.h"
#include "IPv4InterfaceData.h"
#include "SRPacket_m.h"
#include "Ieee802Ctrl_m.h"
#include "TableRAREAccess.h"
#include "BloomTableAccess.h"
#include "InterfaceTableAccess.h"

Define_Module(SR);

// Initialize the number of not known sensors in the network
uint16_t SR::numberSensorNotKnown = 0;
uint16_t SR::numberSensorInNetwork = 0;
long SR::NBTotalMessageSent = 0 ;
long SR::NBTotalMessageRcvd = 0 ;
long SR::hopCountPerRequest = 0;
bool SR::iCanSendRequest = true;
bool SR::stopSim = false;
uint32_t SR::lastPosition = 0;
uint16_t SR::areaBitmap = 0;
uint64_t SR::generatorBitmap = 0;

long SR::NBMsgSentPerRequest = 0;
long SR::NBMsgRcvdPerRequest = 0;

// This module is divided in 3 parts : common part, simple sensor part and sink part

//################################### Common Part ################################

// Initialization
void SR::initialize()
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;

    QueueBase::initialize();

	// Initialize the pseudo-random number generator
    srand( time( NULL ) );

    queueOutGate = gate("queueOut");
    isSink = par("isSink");

    // Retrieve my BloomTable
    bt = BloomTableAccess().get(); // Retrieve the BloomTable
    tr = TableRAREAccess().get();  // Retrieve the TableRARE

    hasSentAdvertMsg = false;

	myID = 65535 ;  // My ID is not initialize yet
    iDBuilder = new IDBuilder();  // An utility to manipulate ids

    areaNumber = 0 ; // Just the sink need to use this parameter
    firstSinkMsg = true; // To know if this is the first message of the sink


    sensorStatus = STATUS_DISCO; // The sensor status
    MAX_ALERT_LIVETIME = 44640; // An alert is relevant during one month
    MAX_ENTRY_RARE = 256;  // Number of row allocated for alerts in the tableRARE
    FALSE_ALERT_TIMEOUT = 2;  // The declare a false alert after 2 minutes
    //LAST_ALERT_ABSOLUTE_TIME = (double)simTime();
    //LAST_ALERT_TIMESTAMP = LAST_ALERT_ABSOLUTE_TIME % MAX_ALERT_LIVETIME; // For now there are no alert
    // The difference bettween alerts timestamps
    // must be higher than the alert maximum livetime divided by the
    // the space required to store alerts
    TIME_BETWEEN_ALERTS = MAX_ALERT_LIVETIME / MAX_ENTRY_RARE;

	NB_EVENT_IN_LIVETIME = 64; // Maximum number of events in one livetime
	EVENT_MAX_LIVETIME = 4096; // Time after which another event can be thrown
	LIVETIME = EVENT_MAX_LIVETIME * NB_EVENT_IN_LIVETIME;  // An event must not be relevant after this amount of time

	currentRequestTimeStamp = 0; // There are no current request now

    // Initialization of global messages
    falseAlertTimeoutMsg = new cMessage("FalseAlertTimeout");
    internalAlertMsg = new cMessage("InternalAlert");
    selfInitializationMsg = new cMessage("SelfInitialization");
    selfGenerateRequestMsg = new cMessage("selfGenerateRequest");
    updateStatsMsg = new cMessage("updateStatsMsg");

    /*
     * Initialize statistics
     */
    numSent = 0;
    numReceived = 0;
    WATCH(numSent);
    WATCH(numReceived);
    hopCountPerRequestStats.setName("hopCountPerRequestStats");
    hopCountPerRequestStats.setRangeAutoUpper(0, 10, 1.5);
    hopCountPerRequestVector.setName("hopCountPerRequest");
    NBMsgSentPerRequestVector.setName("NBMsgSentPerRequest");
    NBMsgRcvdPerRequestVector.setName("First iteration of message set");
    NBMsgRcvdPerRequestVector2.setName("Second iteration of the same message set");

	// Schedule the self initialization
	scheduleAt(simTime()+1.0, selfInitializationMsg);
}


// This method receives first packets on the queue
void SR::endService(cPacket *msg)
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
}

// The time to print statistics
void SR::finish(){
	EV <<"[SR]::"<< __FUNCTION__ ;

	iDBuilder->setID(myID);
	EV <<" Sensor:" << iDBuilder->getID() << " idToMACAddress("<< myID <<")=" << idToMACAddress(myID)
	<< " => mac extended=" << iDBuilder->getMACExt()
	<< ", sink Distance=" << iDBuilder->getSinkDistance()
	<< ", area=" << iDBuilder->getArea() << endl;
//	dumpNeighbors();

	bt->printFilters();

    // This function is called by OMNeT++ at the end of the simulation.
    EV << "Sent:     " << numSent << endl;
    EV << "Received: " << numReceived << endl;
    NBTotalMessageSent += numSent;
    NBTotalMessageRcvd += numReceived;
    EV << "Total Sent:     " << NBTotalMessageSent << endl;
    EV << "Total Received: " << NBTotalMessageRcvd << endl;

    EV << "Hop count, min:    " << hopCountPerRequestStats.getMin() << endl;
    EV << "Hop count, max:    " << hopCountPerRequestStats.getMax() << endl;
    EV << "Hop count, mean:   " << hopCountPerRequestStats.getMean() << endl;
    EV << "Hop count, stddev: " << hopCountPerRequestStats.getStddev() << endl;

    recordScalar("#sent", numSent);
    recordScalar("#received", numReceived);

    hopCountPerRequestStats.recordAs("hop count per request");
}

// This method receives all message sent to this module
void SR::handleMessage(cMessage *msg)
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;


	// This message is computed differently in the sink
	if (isSink){
		SINK_handleMessage(msg);
		return;
	}

	if (msg->isSelfMessage())
    {
		handleSelfMsg(msg);
    }else if (dynamic_cast<SRPacket *>(msg))
    {
   		handleSRMsg((SRPacket*)msg);
    }
    else{
    	// Error
    }
}

// Send the SRPacket to the destination node after some time
void SR::scheduleMsgToSendNic(SRPacket * srPacket, uint16_t delayMin){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;

	srPacket->setSrcMACAddress(myMACAddress);

    // add control info with MAC address
    Ieee802Ctrl *controlInfo = new Ieee802Ctrl();
    controlInfo->setDest(srPacket->getDestMACAddress());
    srPacket->setControlInfo(controlInfo);

	uint32_t randomTime = (rand() % 10) + 1.0 + (uint32_t)delayMin;
    EV << "     Scheduling to send the message in "<< randomTime << " seconds. ";
    scheduleAt(simTime()+randomTime,srPacket);
    EV  <<"src: " << myMACAddress << ", dest: " << srPacket->getDestMACAddress() <<endl;
}

// Add a neighbor
void SR::addNeighbor(MACAddress neighborMACAddress, uint16_t neighborID,BloomFilter neighborBloom){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	bt->AddFilter(neighborBloom,neighborMACAddress);
	addNeighbor(neighborMACAddress,neighborID);
}

// Add a neighbor
void SR::addNeighbor(MACAddress neighborMACAddress, uint16_t neighborID){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	neighborList[neighborID] = neighborMACAddress;
	EV << "     New neighbor <" << neighborID <<","<< neighborMACAddress.str() << "> added "<< endl;
}

// return true if an alert can be thrown at this time
bool SR::isAlertAllowed(){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	// For now, just return true;
	/*
	double currentAbsoluteTime = simTime();
	uint16_t timeInCurrentAlertLiveTime = currentAbsoluteTime % MAX_ALERT_LIVETIME;
	LAST_ALERT_TIMESTAMP = LAST_ALERT_ABSOLUTE_TIME % MAX_ALERT_LIVETIME;

	if((LAST_ALERT_TIMESTAMP/TIME_BETWEEN_ALERTS) != (timeInCurrentAlertLiveTime/TIME_BETWEEN_ALERTS))
		return true;

	if((LAST_ALERT_ABSOLUTE_TIME/MAX_ALERT_LIVETIME) != (currentAbsoluteTime/MAX_ALERT_LIVETIME))
		return true;
*/
	return false;
}

// Notify all my neighbor of an anomaly I detected
void SR::broadcastAlert(){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	// TODO : implement this function

    // Create and send my alert packet
    SRPacket *alert = new SRPacket("ALERT");
    alert->setId(myID);
    alert->setMsgType(MSG_ALERT);
    alert->setAlertTimeStamp(LAST_ALERT_TIMESTAMP);
    alert->setAmIAlertGenerator(true);
    alert->setHopCount(0);
    alert->setQueryType(Q_REQUEST);
    alert->setDestMACAddress(MACAddress::BROADCAST_ADDRESS);
//    scheduleMsgToSendNic(alert,10);
}

// Return true if the neighbor alert timestamp is close enough to ours
bool SR::isNeighborAlertRelevant(uint16_t neighborAlertTimeStamp){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	// Just return true for now;
	return true;
	/*
	 * TODO : implement this function
	if((neighborAlertTimeStamp != LAST_ALERT_TIMESTAMP)||
			(neighborAlertTimeStamp))
		return;
*/
}

// Print all my neighbors
void SR::dumpNeighbors(){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
    EV <<"[SR] My neighbor list : ";
	NeighborList_t::iterator it;

	for (it = neighborList.begin (); it != neighborList.end (); ++it)
	{
		MACAddress mac= (*it).second;
		int id = (*it).first;
//		EV << "<"<< id <<","<< mac << ">";
		EV << "<"<< id <<">";
	}
	EV <<endl;
}

// Change the color of my display
void SR::changeDisplay(){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	// change icon displayed in Tkenv
	cDisplayString* display_string = &getParentModule()->getParentModule()->getDisplayString();
	iDBuilder->setID(myID);
	std::string color = "#ffffff";
	switch(iDBuilder->getSinkDistance()){
	case 0:
		//color = "#11ff22";
		color = "blue";
		break;
	case 1:
		//color = "#99ff11";
		color = "green";
		break;
	case 2:
		//color = "#eebb00";
		color = "black";
		break;
	case 3:
		//color = "#884400";
		color = "red";
		break;
	case 4:
		//color = "#881100";
		color = "gold";
		break;
	case 5:
		//color = "#660022";
		color = "gray";
		break;
	case 6:
		//color = "#dd0055";
		color = "#dd00ee";
		break;
	}
	size_t size = color.size() + 1;
	char * buffer = new char[ size ];
	strncpy( buffer, color.c_str(), size );
	display_string->setTagArg("i", 1, buffer);
	delete [] buffer;
}

// Display informations on the node
void SR::updateDisplay(){
    char buf[80] = "";
    if (numSent>0) sprintf(buf+strlen(buf), "sent:%ld ", numSent);
    if (numReceived>0) sprintf(buf+strlen(buf), "rcvd:%ld ", numReceived);
//    if (hopCountPerRequest>0) sprintf(buf+strlen(buf), "hop:%ld ", hopCountPerRequest);
    getParentModule()->getParentModule()->getDisplayString().setTagArg("t",0,buf);
}

// Return the MAC Address corresponding to that id
MACAddress SR::idToMACAddress(uint16_t id){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	MACAddress *idMAC = new MACAddress(myMACAddress);

	iDBuilder->setID(id);
	uint16_t mac_ext = iDBuilder->getMACExt();
	idMAC->setAddressByte(5,(unsigned char)(mac_ext));
	return *idMAC;
}



// This method helps to schedule a time to generate a request
void SR::scheduleTimeToGenerateRequest(){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	// Generate a request  after a random time in the next event time slot
	uint32_t timeInSeconds = (uint32_t)simTime().raw();
	uint32_t timeInCurrentLivetime = timeInSeconds % LIVETIME;
	ASSERT(timeInCurrentLivetime < LIVETIME);
	uint32_t nextSlotTime = (((timeInCurrentLivetime/EVENT_MAX_LIVETIME) + 1)*EVENT_MAX_LIVETIME) - timeInCurrentLivetime;

	uint32_t randPosInBitmap = (rand()%64);   // a random position in the bitmap  [0; 63]
	int ii=0 ;  // You may not be luck enough to have a free position
	while(((generatorBitmap) & (1<<randPosInBitmap)) == 1){
		// 1 & 1 == 1   and 0 & 1 == 0
		// As long as that position is already set, I chose another one
		randPosInBitmap = (rand()%64);
		if(ii == 64){ // We have tried too much with no luck. We sould get out of here.
			EV <<  "    Sorry, there are no pisition available to send a request now. You will not make any request in this simulation." <<endl;
			return;
		}
		ii++;
	}
	generatorBitmap += (1<<randPosInBitmap);

	if(randPosInBitmap > lastPosition){
		iAmTheLast = true;
		lastPosition = randPosInBitmap;
	}

	// Here there is a free position in the bitmap
	EV << "    Youpi, there is a free slot time for me. The slot time number: "<<randPosInBitmap << endl;

	uint32_t randomTime = nextSlotTime  // wait till the next time slot
		+ (randPosInBitmap)*EVENT_MAX_LIVETIME  // Randomly chose time slot
		+ (rand() % EVENT_MAX_LIVETIME); // Randomly chose a time in that time slot
    EV << "     Scheduling to send request message in "<< randomTime << " seconds."
    << " timeInSeconds=" << timeInSeconds
    << " timeInCurrentLivetime=" <<timeInCurrentLivetime
    << " nextSlotTime=" <<nextSlotTime
    << " EVENT_MAX_LIVETIME=" <<EVENT_MAX_LIVETIME
    <<endl;
	//ASSERT(randomTime < LIVETIME);
    scheduleAt(simTime()+randomTime,selfGenerateRequestMsg);
}


//########################## SIMPLE SENSOR PART ###########################

// Handle messages generated by me
void SR::handleSelfMsg(cMessage *msg)
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	if (msg == selfInitializationMsg)
    {
		// Because I do not know to start at a stage different from 0 (stage 2)
		// Here I initialize some important stuff
		// Initialize my Interface
		InterfaceEntry * my_ie = InterfaceTableAccess().get()->getInterface(1);
		ASSERT(my_ie);
		ASSERT(!my_ie->isLoopback());

		// Initialize my MAC Address and personal ID
		myMACAddress = my_ie->getMacAddress();

		// I am not known yet, so I increase the number of sensors not know in the network
		numberSensorNotKnown++;
		numberSensorInNetwork++;
		EV << "     numberSensorNotKnown = " <<numberSensorNotKnown
			<< ", numberSensorInNetwork=" << numberSensorInNetwork <<endl;

		// Each simple sensor must select what type he is
		bt->selectRandomType();

		// This is all I need to do in the initialization process
    }else if (msg == internalAlertMsg)
    {
    	EV << "     An alert just occurred here" <<endl;
    	handleInternalAlertMsg(msg);

    }else if (msg == falseAlertTimeoutMsg){
    	EV << "     The previous alert generated was a false one" <<endl;
    	handleFalseAlertMsg(msg);

    }else if(msg == selfGenerateRequestMsg){
    	EV << "     I need to generate a request" <<endl;
    	generaterRandomRequest();
    }else if (dynamic_cast<SRPacket *>(msg)){
    	// This packet has been schedule by me to send now
    	SRPacket * sr = (SRPacket*)msg;
    	ASSERT((myMACAddress.compareTo(sr->getSrcMACAddress())==0));

		EV << "     Sending message now. src: " << "myMACAddress:" << myMACAddress <<"; dst:" <<sr->getDestMACAddress()<<endl;
		// send out
		sendDirect(sr, getParentModule(), "ifOut",0);
		numSent++;

		// Update my display
		updateDisplay();
    }
}

// Handle Alert message generated by me
// This function should notice the sink from this error
void SR::handleInternalAlertMsg(cMessage *msg){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	// I detected that something is wrong
	// Check first if an alert can be created now ()
	// If it is allowed :
	if(!isAlertAllowed())return;

	/*
	// I can throw this alert
	LAST_ALERT_ABSOLUTE_TIME = simTime();
	LAST_ALERT_TIMESTAMP = LAST_ALERT_ABSOLUTE_TIME % MAX_ALERT_LIVETIME;
*/
	// I tell my neighbors G=0
	broadcastAlert();
	// I set myself in alert status
	sensorStatus = STATUS_ALERT;
	// and set the timer FALSE_ALERT_TIMEOUT
	scheduleAt(simTime()+(double)FALSE_ALERT_TIMEOUT,falseAlertTimeoutMsg);
}


// Handle False alert generated my me
// This function is called when the FALSE_ALERT_TIMEOUT expires
void SR::handleFalseAlertMsg(cMessage *msg){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	// This message get here if I didn't get any response
	// from my neighbors after the FALSE_ALERT_TIMEOUT time

	// Do nothing if my status is not alert
	if(sensorStatus != STATUS_ALERT)
		return;

	// So this alert was just a false one
	// Set my status back to normal
	sensorStatus = STATUS_NORMAL;
}

// Handle message coming from the network
void SR::handleSRMsg(SRPacket *msg)
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;

	// delete old control info
	delete msg->removeControlInfo();

	EV << "    msg->timestamp : " << msg->getMyTimestamp() << endl;
	msg->setHopCount(msg->getHopCount()+1); // J'incrémente le nombre de saut réalisé par la requête

	switch(msg->getMsgType()){
	case MSG_DISCOVERY:
		numReceived++;
		handleNicDiscoveryMsg(msg);
		break;
	case MSG_ALERT:
		numReceived++;
		handleNicAlertMsg(msg);
		break;
	case MSG_NORMAL:
		numReceived++;
		NBMsgRcvdPerRequest++;
		handleNicNormalMsg(msg);
		break;
	default:
		EV << "   Message Type Unknown" << endl;
	}

	// Update my display
	updateDisplay();
}

// Handle discovery message coming from the network
void SR::handleNicDiscoveryMsg(SRPacket *msg){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;

	// Who sent me this discovery message ?
	MACAddress srcMACAddress = msg->getSrcMACAddress();
	uint16_t srcSensorID = msg->getId();
	iDBuilder->setID(srcSensorID);

	// Is it the sink sending ?
	if(iDBuilder->getSinkDistance()==0){
		// Yes : do I know him ?
		if(!firstSinkMsg)
		{
			// Yes, I know the sink
			EV << "     neighborList["<<srcSensorID<<"] = " <<neighborList[srcSensorID] << " . I already know the sink." << endl;
			//have I already sent an advertisement message ?
			if(hasSentAdvertMsg){
				// Yes : do nothing
			}else{
				// Get an ID
				iDBuilder->setID(srcSensorID);
				iDBuilder->setMACExt(myMACAddress.str());
				iDBuilder->setSinkDistance(iDBuilder->getSinkDistance()+1);
				myID = iDBuilder->getID();

				// Send my first advertisement message
			    // Create an advertisement packet
			    SRPacket *sr = new SRPacket("DISCO");
			    sr->setId(myID);
			    sr->setMsgType(MSG_DISCOVERY);
			    sr->setBloom(bt->GetBloomPerso());
			    sr->setDestMACAddress(MACAddress::BROADCAST_ADDRESS);
			    scheduleMsgToSendNic(sr,10);

				hasSentAdvertMsg = true;

				// Now I am know, so I decrease the number of sensors not known
				numberSensorNotKnown--;
				sensorStatus = STATUS_NORMAL;
				EV << "     numberSensorNotKnown = " <<numberSensorNotKnown <<endl;

				// We should schedule a time when to send a request
				scheduleTimeToGenerateRequest();

				// Change my display
				changeDisplay();
			}
		}else {
			// No : Add the sink as neighbor
//			addNeighbor(srcMACAddress,srcSensorID);
			addNeighbor(srcMACAddress,srcSensorID,msg->getBloom());
			// Send him a request
		    SRPacket *sr = new SRPacket("DISCO-REQUEST");
		    sr->setId(myID);
		    sr->setMsgType(MSG_DISCOVERY);
		    sr->setQueryType(Q_REQUEST);
		    sr->setBloom(bt->GetBloomPerso());
		    sr->setDestMACAddress(srcMACAddress);

			uint16_t randPosInBitmap = (rand()%4);   // a random position in the bitmap  [0; 63]
			int ii=0 ;  // You may not be luck enough to have a free position
			while(((areaBitmap) & (1<<randPosInBitmap)) == 1){
				// 1 & 1 == 1   and 0 & 1 == 0
				// As long as that position is already set, I chose another one
				randPosInBitmap = (rand()%4);
				if(ii == 16){ // We have tried too much with no luck. We sould get out of here.
					ASSERT(false);
				}
				ii++;
			}
			areaBitmap += (1<<randPosInBitmap);

		    scheduleMsgToSendNic(sr,randPosInBitmap*10);

		    firstSinkMsg = false;
		}
	}else{
		// It is NOT the sink sending
		// Do I know the sender ?
		if(neighborList.find(srcSensorID) != neighborList.end())
		{
			// Yes : do nothing
			EV << "     neighborList["<<srcSensorID<<"] = " <<neighborList[srcSensorID] << " . Already registered here!" << endl;
		}else{
			// No :  Add this sensor as neighbor
//			addNeighbor(srcMACAddress,srcSensorID);
			addNeighbor(srcMACAddress,srcSensorID,msg->getBloom());
				// have I already sent an advertisement message ?
			if(hasSentAdvertMsg){
				// Yes : do nothing
			}else{
				// Get an ID
				iDBuilder->setID(srcSensorID);
				iDBuilder->setMACExt(myMACAddress.str());
				iDBuilder->setSinkDistance(iDBuilder->getSinkDistance()+1);
				myID = iDBuilder->getID();

				// No : Send my first advertisement message
			    // Create an advertisement packet
			    SRPacket *sr = new SRPacket("DISCO");
			    sr->setId(myID);
			    sr->setMsgType(MSG_DISCOVERY);
			    sr->setBloom(bt->GetBloomPerso());
			    sr->setDestMACAddress(MACAddress::BROADCAST_ADDRESS);
			    scheduleMsgToSendNic(sr,10*iDBuilder->getSinkDistance());

				hasSentAdvertMsg = true;

				// I should be known now, so i decrease the number of sensors not known in the network
				numberSensorNotKnown--;
				sensorStatus = STATUS_NORMAL;
				EV << "     numberSensorNotKnown = " <<numberSensorNotKnown <<endl;

				// Now, we should schedule a time to generate a equest
				scheduleTimeToGenerateRequest();
			}
		}

	}
}

// Handle Alert messages coming from the network
void SR::handleNicAlertMsg(SRPacket *alertMsg){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	// I just received an alert message from a neighbor

	// Was this alert generated by my neighbor ?
	if(!alertMsg->getAmIAlertGenerator()){
		// This alert is not originated by my neighbor
		// So I just need to forward it
		//TODO : Send this message to the sink
		//forwardAlertPaquetToSink(msg);
		return;
	}


	// This alert was generated by my neighbor
	// I should take care of it
	// I am not if the alert mode, There is nothing to do
	if(sensorStatus != STATUS_ALERT) return;

	// I am in the alert mode too. let see if we detect the same alert
	uint16_t neighborAlertTimeStamp = alertMsg->getAlertTimeStamp();
	// If the alert detected by my neighbor is too different from mine,
	// well I wont support it.
	if(!isNeighborAlertRelevant(neighborAlertTimeStamp)) return;

	// If my neighbor alert is close to mine,
	// That's not good. WE ARE IN ALERT !
	// First I must disable the FALSE_ALERT_TIMEOUT by canceling
	// the timeout message from coming back;
	cancelEvent(falseAlertTimeoutMsg);

	// I must set the message correctly
	alertMsg->setAmIAlertGenerator(false);
	// I send it to the sink ASAP (through my neighbor)

	alertMsg->setMsgType(MSG_ALERT);
	alertMsg->setId(myID);

	// Find the next hop of this message
	// TODO : the next hop should be a node, not a broadcast Address (TableRARE)
	MACAddress nextHop = MACAddress::BROADCAST_ADDRESS;
	alertMsg->setDestMACAddress(nextHop);
	scheduleMsgToSendNic(alertMsg,0);
}

// Handle normal messages coming from the network
void SR::handleNicNormalMsg(SRPacket *message){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	// Je vérifie si c'est bien un message normal
	// if (message->getMsgType() == MSG_NORMAL)

	//*******************************Fonctionnement normal*****************************************//
	// Je crée mon message avant la fonction si je veux envoyer un message avec Id à myID, msgType à MSG_NORMAL, queryType à Q_REQUEST et QueryId rempli et le reste des champs non rempli!
	// Rappel: SRPacket *message = new SRPacket("MA REQUETE");


	// Je définis le délai d'attente
	int delayMin = 10;
	// 2eme Cas: Je reçoit une requête
	if ( (message->getQueryType() == Q_REQUEST) ) {  // C'est sur que c'est pas moi qui ai créé cette requête
		EV << "       I just received a request for " << message->getQueryId() << " and timestamp is :" << message->getMyTimestamp() <<endl;

		if(currentRequestTimeStamp == message->getMyTimestamp()){
			EV << "   I Already have sent this request " << currentRequestTimeStamp <<" . I should destroy this copy." <<endl;
			delete message;
			return;
		}

		currentRequestTimeStamp = message->getMyTimestamp();
		// On stocke la personne qui a envoye la requete soit le couplet <IDsource, prevMAC>
		// pour utiliser cette information lors de la réception de la réponse
		RecordTable[message->getId()] = message->getSrcMACAddress();

		// On initialise le peer qui a potentiellement une réponse à -1, ce sera peut-etre moi
		MACAddress PeerWithAnswer = MACAddress::UNSPECIFIED_ADDRESS;

		// Je regarde d'abord dans mes filtres de Bloom si moi ou mes voisins ont la réponse
		PeerWithAnswer = bt->GetNextHop(message->getQueryId(),false,myMACAddress);

		// Si PeerWithAnswer est de 0 alors je répond à la requête et j'envoie le message de réponse
		if ((PeerWithAnswer.equals(myMACAddress))) {
			EV << "         And I can respond to that request : "<< message->getQueryId() <<endl;

			// Set a request name with its
			std::stringstream responseStream;
			std::string responseName_str;

			responseStream << "RESPONSE-" << currentRequestTimeStamp;
			responseStream >> responseName_str;
			size_t size = responseName_str.size() + 1;
			char * responseName_char = new char[ size ];
			strncpy( responseName_char, responseName_str.c_str(), size );

			// Je crée le message de réponse et rempli les champs puis l'envoie
			SRPacket *messageReponse = new SRPacket(responseName_char);
			messageReponse->setMsgType(MSG_NORMAL); // C'est un message normal
			messageReponse->setQueryType(Q_REPLY); // C'est une reponse
			messageReponse->setId(message->getId()); // Je met l'ID de celui qui a besion de la réponse pour qu'il se reconnaisse
			messageReponse->setHopCount(message->getHopCount()); // Je met le nombre de saut réalisé par la requête
			messageReponse->setSrcMACAddress(myMACAddress);
			messageReponse->setMyTimestamp(currentRequestTimeStamp);
			// TODO : Mettre la réponse dans le message
			// Si j'était moi même l'émetteur de la requête
			MACAddress nextHop;
			if(message->getId() == myID){
				// Je m'envois le message par le réseau pour utiliser la méthode qu'il faut
				nextHop = myMACAddress;
			}
			// Je vais utilisé le routage classique pour le retour de la réponse
			else{
				nextHop = RecordTable[message->getId()];
			}

			messageReponse->setDestMACAddress(nextHop); // J'envoie en routage classique à l'@MAC correspondant à l'ID du créateur de la requête
			messageReponse->setQueryId(message->getQueryId());

			// Et je rajoute le champs ou je met la valeur de la réponse!!!!!!!!!!
			// Selon si c'est une valeur, ou une erreur ou une alerte, je remplis les champs correspondant

			scheduleMsgToSendNic(messageReponse,delayMin);

		}

		// Sinon si PeerWithAnswer est > 0 alors un voisin direct a l'information et je lui envoie ma requête
		// Ce voisin ne doit pas être celui qui m'a précédament envoyer la requête
		else if ((!PeerWithAnswer.isUnspecified()) && (!PeerWithAnswer.equals(message->getSrcMACAddress()))) {
			EV << "         My neighbor can respond to that request : "<< message->getQueryId() <<endl;

			// Seul les adresses MAC ont besoin d'être modifiées
			message->setSrcMACAddress(myMACAddress);
			message->setDestMACAddress(PeerWithAnswer);
			message->setDestMACAddress(PeerWithAnswer);
			scheduleMsgToSendNic(message,delayMin);
		}

		// Sinon si PeerWithAnswer est NULL alors aucun de voisin n'a l'information et je passe à l'étape supérieur, la table de rare
		else {
			EV << "       I know nobody who can respond to that request : "<< message->getQueryId() <<endl;

			message->setSrcMACAddress(myMACAddress);

			// La table de Rare me donne un peer vers qui envoyer la requête, au pire c'est un peer aléatoire
			PeerWithAnswer = tr->LearningPeerSelection(message->getQueryId());

			// La tableRARE peut ne contenir que celui qui m'a envoyé la requête
			// Dans ce cas là, il faut faire du flooding
			if(PeerWithAnswer.isUnspecified()){
				// J'envoi à tous mes voisins, excepté celui qui m'a envoyé la requête
				NeighborList_t::iterator it;
				for (it = neighborList.begin (); it != neighborList.end (); ++it)
				{
					// Pour l'instant, on n'envois qu'au premier voisin différent de l'emetteur de la requête
					if(!message->getDestMACAddress().equals((*it).second)){
						// J'envoie ma requête au peer séléctionné
						message->setDestMACAddress((*it).second);
						scheduleMsgToSendNic(message,delayMin);
						break;
					}
				}
			}else{
				// La table RARE à trouvé quelque à qui envoyer la requête
				// J'envoie ma requête au peer séléctionné
				message->setDestMACAddress(PeerWithAnswer);
				scheduleMsgToSendNic(message,delayMin);
			}
		}

		NBMsgSentPerRequest++;
	}

	// 3eme Cas: Si je forward une réponse
	else if ( (message->getQueryType() == Q_REPLY) && (message->getId() != myID) ) {
		EV << "         I need to forward back this response : "<< message->getQueryId() << " to " << message->getId() <<endl;

		// J'enregistre dans la table de rare le peer qui apporte la réponse
		tr->UpdateTable(message->getQueryId(),message->getSrcMACAddress());

		// On recupere le nextMAC grace a la route enregistree dans RecordTable
		MACAddress nextPeerMAC = RecordTable[message->getId()];
		EV << "     I can do it through " << nextPeerMAC.str() << endl;

		// Je transfert la réponse vers le bon peer (je change les champs nécessaire et je renvoie)
		message->setSrcMACAddress(myMACAddress);
		message->setDestMACAddress(nextPeerMAC);// Le peer qui va nous permettre d'atteindre le peer qui veut la réponse));
		scheduleMsgToSendNic(message,delayMin);

		NBMsgSentPerRequest++;
	}

	// 4eme Cas: Si je recois la réponse à la question que j'avais posé
	else if ( (message->getQueryType() == Q_REPLY) && (message->getId() == myID) ) {
		EV << "         COOL, I got the response to my request : "<< message->getQueryId() <<endl;

		// Je met à jour la table de RARE
		tr->UpdateTable(message->getQueryId(),message->getSrcMACAddress());
	    // Message arrived
	    uint16_t hopcount = message->getHopCount();
	    EV << "		Message " << message << " arrived after " << hopcount << " hops.\n";

		hopCountPerRequest = (long) hopcount;
	    if(iAmTheLast)
	    	stopSim = true;
	}
}

// Generate a random request and send it in the network
void SR::generaterRandomRequest(){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	if(!iCanSendRequest){
		EV << "      I cannot generate requests now " << endl;
//		ASSERT(false);
	}
	iCanSendRequest = false ;


	// generate a random request
	// On initialise le peer qui a potentiellement une réponse à -1 (il n'existe pas)
	MACAddress PeerWithAnswer = MACAddress::UNSPECIFIED_ADDRESS;

	// Set a request name with its
	std::stringstream requestStream;
	std::string requestName_str;
	uint32_t timeInSeconds = (uint32_t)simTime().raw();
	currentRequestTimeStamp = timeInSeconds % LIVETIME;
	requestStream << "REQUEST-" << currentRequestTimeStamp;
	requestStream >> requestName_str;
	size_t size = requestName_str.size() + 1;
	char * requestName_char = new char[ size ];
	strncpy( requestName_char, requestName_str.c_str(), size );

	// Create an normal request packet
    SRPacket *myRequest = new SRPacket(requestName_char);
    myRequest->setId(myID);
    myRequest->setMsgType(MSG_NORMAL);
    myRequest->setQueryType(Q_REQUEST);
    myRequest->setQueryId(SENSOR_HUMIDITY);
    myRequest->setHopCount(0);
    myRequest->setMyTimestamp(currentRequestTimeStamp);
	// 1er Cas: Je veux envoyer une requête

	// Je regarde d'abord dans mes filtres de Bloom si mes voisins ont la réponse
	// Attention dans l'envoie d'une requête, je demande parfois ce que moi même je détecte -> true
	PeerWithAnswer = bt->GetNextHop(myRequest->getQueryId(),true,myMACAddress);


	// Sinon si PeerWithAnswer est de -1 alors aucun de voisin n'a l'information et je passe à l'étape supérieur, la table de rare
	if (PeerWithAnswer.isUnspecified()) {
		// La table de Rare me donne un peer vers qui envoyer la requête, au pire c'est un peer aléatoire
		PeerWithAnswer = tr->LearningPeerSelection(myRequest->getQueryId());

		// Si ma tableRARE est vide pour l'instant
		// On fait du broadcast
		if(PeerWithAnswer.isUnspecified())
			PeerWithAnswer = MACAddress::BROADCAST_ADDRESS;
	}

	// I must have  a neighbor to whom I will send the request
	ASSERT(!PeerWithAnswer.isUnspecified());

	// J'envoie ma requête au peer séléctionné
	// Je remplis les champs
	myRequest->setSrcMACAddress(myMACAddress);
	myRequest->setDestMACAddress(PeerWithAnswer);

	EV <<"      I want to send the request. I am Looking for : " << myRequest->getQueryId()
		<< ". I Will send my request through " << PeerWithAnswer <<endl;

	// J'envoie le message
	scheduleMsgToSendNic(myRequest,5);

	hopCountPerRequest = 0 ;
	NBMsgSentPerRequest = 0 ;
	NBMsgRcvdPerRequest = 0 ;

	scheduleAt(simTime()+(simtime_t)LIVETIME,selfGenerateRequestMsg);
}

//######################## SINK PART #####################################

// handle all messages coming to this module, for a sink
void SR::SINK_handleMessage(cMessage *msg){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;

	if (msg->isSelfMessage())
    {
		SINK_handleSelfMsg(msg);
    }else if (dynamic_cast<SRPacket *>(msg))
    {
   		SINK_handleSRMsg((SRPacket*)msg);
    }
    else{
    	EV << "      Received unknown message !!!" << endl;
    }
}

// Handle messages generated by me
void SR::SINK_handleSelfMsg(cMessage *msg){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	if (msg == selfInitializationMsg)
    {
		// Because I do not know to start at a stage different from 0 (stage 2)
		// Here I initialize some important stuff
		// Initialize my Interface
		InterfaceEntry * my_ie = InterfaceTableAccess().get()->getInterface(1);
		ASSERT(my_ie);
		ASSERT(!my_ie->isLoopback());

		// Initialize my MAC Address and personal ID
		myMACAddress = my_ie->getMacAddress();
	    ASSERT(!myMACAddress.isUnspecified());

	    // The sink has to update its bloom filter
	    // He must set that he is the sink
	    bt->AddToBloomPerso("SINK");
	    // and update his display to ink
	    bt->updateDisplay("srwsn/sink");

		// If I am the sink, I should start the simulation
		// SinkDistance=0, Area=0
		iDBuilder->setMACExt(myMACAddress.str());
		iDBuilder->setSinkDistance(0);
		iDBuilder->setArea(0);
		myID = iDBuilder->getID();
	    // Create an hello world packet
	    SRPacket *sr = new SRPacket("DISCO-INIT");
	    sr->setId(myID);
	    sr->setMsgType(MSG_DISCOVERY);
	    sr->setQueryType(Q_REQUEST);
	    sr->setBloom(bt->GetBloomPerso());
	    sr->setDestMACAddress(MACAddress::BROADCAST_ADDRESS);
	    scheduleMsgToSendNic(sr,5);
	    sensorStatus = STATUS_NORMAL;

	    // Schedule the time to start recording statistics
	    SINK_updateStats();

    }else if (msg == updateStatsMsg){
    	SINK_updateStats();
    }
	else if (msg == internalAlertMsg)
    {
    	EV << "     An alert just occurred here. I am the sink, I take care of everything !" <<endl;
    }else if (dynamic_cast<SRPacket *>(msg)){
    	// This packet has been schedule by me to send now
    	SRPacket * sr = (SRPacket*)msg;
    	ASSERT((myMACAddress.compareTo(sr->getSrcMACAddress())==0));

		EV << "     Sending message now. src: " << "myMACAddress:" << myMACAddress <<"; dst:" <<sr->getDestMACAddress()<<endl;
		// send out
		sendDirect(sr, getParentModule(), "ifOut",0);
		// Change my display
		changeDisplay();

		numSent++;
    }
}

// Handle messages coming from the network
void SR::SINK_handleSRMsg(SRPacket *msg){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	// delete old control info
	delete msg->removeControlInfo();

	switch(msg->getMsgType()){
	case MSG_DISCOVERY:
		SINK_handleNicDiscoveryMsg(msg);
		break;
	case MSG_ALERT:
		SINK_handleNicAlertMsg(msg);
		break;
	case MSG_NORMAL:
		SINK_handleNicNormalMsg(msg);
		break;
	default:
		EV << "   Message Type Unknown" << endl;
	}
}

// Handle discovery message coming from the network
void SR::SINK_handleNicDiscoveryMsg(SRPacket *msg){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	// Who sent me this discovery message?
	MACAddress srcMACAddress = msg->getSrcMACAddress();
	uint16_t srcSensorID = msg->getId();

	// Is it a request ?
	switch (msg->getQueryType()){
	case Q_REQUEST:
	{
		numReceived++;
		EV << "     Received Q_REQUEST from " << srcMACAddress << endl;
		// Add the sensor as neighbor
//		addNeighbor(srcMACAddress,srcSensorID);
		addNeighbor(srcMACAddress,srcSensorID,msg->getBloom());

		// Set my ID in this area
		iDBuilder->setID(myID);
		iDBuilder->setArea(areaNumber);
		myID = iDBuilder->getID();
		areaNumber++;

		// send him an area message
	    // Create a discovery reply packet
	    SRPacket *sr = new SRPacket("DISCO-REPLY");
	    sr->setId(myID);
	    sr->setMsgType(MSG_DISCOVERY);
	    sr->setQueryType(Q_REPLY);
	    sr->setDestMACAddress(srcMACAddress);
	    scheduleMsgToSendNic(sr,40);

		break;
	}
	case Q_REPLY :
		// do nothing
		break;
	}
}

// Handle Alert messages coming from the network
// This function can (may) implement what the sink would do
// when an alert message reaches him
void SR::SINK_handleNicAlertMsg(SRPacket *msg){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	//TODO : implement this function
}
// Handle normal messages coming from the network
void SR::SINK_handleNicNormalMsg(SRPacket *msg){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	//TODO : implement this function
	handleNicNormalMsg(msg);
}

void SR::SINK_updateStats(){
	// update statistics.
    hopCountPerRequestVector.record((int)hopCountPerRequest);
    hopCountPerRequestStats.collect((int)hopCountPerRequest);
    NBMsgSentPerRequestVector.record((int)NBMsgSentPerRequest);
    if(simTime() < (simtime_t)LIVETIME)
        NBMsgRcvdPerRequestVector.record((int)NBMsgRcvdPerRequest);
    else
    	NBMsgRcvdPerRequestVector2.recordWithTimestamp(simTime()-(simtime_t)LIVETIME, (double)NBMsgRcvdPerRequest);


	// Schedule the time to start recording statistics
    updateDisplay();

//    hopCountPerRequest += (rand()%10) - 5;
//    double randNumber = (rand()%EVENT_MAX_LIVETIME);
//    if(!stopSim){
    	scheduleAt(simTime()+EVENT_MAX_LIVETIME/2, updateStatsMsg);
//    }

	iCanSendRequest = true;
}
