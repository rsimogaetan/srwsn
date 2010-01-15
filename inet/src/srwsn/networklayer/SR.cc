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


// This class is divided in 3 parts : common part, simple sensor part and sink part

//################################### Common Part ################################

// Initialization
void SR::initialize()
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;

    QueueBase::initialize();

    queueOutGate = gate("queueOut");
    isSink = par("isSink");

    // Retrieve my BloomTable
    bt = BloomTableAccess().get(); // Retrieve the BloomTable
    // Specify who I am to that
    //bt->

    tr = TableRAREAccess().get();  // Retrieve the TableRARE

    hasSentAdvertMsg = false;

	myID = 65535 ;  // My ID is not initialize yet
    iDBuilder = new IDBuilder();  // An utility to manipulate ids

    areaNumber = 0 ; // Just the sink need to use this parameter
    firstSinkMsg = true; // To know if this is the first message of the sink

	// Initialize the pseudo-random number generator
    srand( time( NULL ) );

    sensorStatus = STATUS_DISCO; // The sensor status
    MAX_ALERT_LIVETIME = 44640; // An alert is relevant during one month
    MAX_ENTRY_RARE = 256;  // Number of row allocated for alerts in the tableRARE
    FALSE_ALERT_TIMEOUT = 2;  // The declare a false alert after 2 minutes
    LAST_ALERT_TIMESTAMP = 0; // For now there are no alert
    // The difference bettween alerts timestamps
    // must be higher than the alert maximum livetime divided by the
    // the space required to store alerts
    TIME_BETWEEN_ALERTS = MAX_ALERT_LIVETIME / MAX_ENTRY_RARE;

    // Initialization of global messages
    falseAlertTimeoutMsg = new cMessage("FalseAlertTimeout");
    internalAlertMsg = new cMessage("InternalAlert");
    selfInitializationMsg = new cMessage("SelfInitialization");
    selfGenerateRequestMsg = new cMessage("selfGenerateRequest");

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
	EV <<" Sensor:" << iDBuilder->getID()
	<< " => mac extended=" << iDBuilder->getMACExt()
	<< ", sink Distance=" << iDBuilder->getSinkDistance()
	<< ", area=" << iDBuilder->getArea() << endl;
	dumpNeighbors();
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
void SR::scheduleMsgToSendNic(MACAddress destMACAddress, SRPacket * srPacket, uint16_t delayMin){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;

	srPacket->setSrcMACAddress(myMACAddress);

    // add control info with MAC address
    Ieee802Ctrl *controlInfo = new Ieee802Ctrl();
    controlInfo->setDest(destMACAddress);
    srPacket->setControlInfo(controlInfo);

    // TODO : Maybe we will use doublerand() from omnetpp.h
	double randomTime = (rand() % 10) + 1.0 + (double)delayMin;
    EV << "     Scheduling to send the message in "<< randomTime << " seconds." <<endl;
    scheduleAt(simTime()+randomTime,srPacket);
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
	return true;
	/*
	 * TODO : implement this function
	uint16_t currentAlertLiveTime = simTime() % MAX_ALERT_LIVETIME;
	if((currentAlertLiveTime - LAST_ALERT_TIMESTAMP)%TIME_BETWEEN_ALERTS == 0)
		return;
	LAST_ALERT_TIMESTAMP = 0; // For now there are no alert
	TIME_BETWEEN_ALERTS = MAX_ALERT_LIVETIME / MAX_ENTRY_RARE;
	*/
}

// Notify all my neighbor of an anomaly I detected
void SR::broadcastAlert(){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	// TODO : implement this function
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
		EV << "     numberSensorNotKnown = " <<numberSensorNotKnown <<endl;

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
		// Change my display
		changeDisplay();
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

	switch(msg->getMsgType()){
	case MSG_DISCOVERY:
		handleNicDiscoveryMsg(msg);
		break;
	case MSG_ALERT:
		handleNicAlertMsg(msg);
		break;
	case MSG_NORMAL:
		handleNicNormalMsg(msg);
		break;
	default:
		EV << "   Message Type Unknown" << endl;
	}
}

// Handle discovery message coming from the network
void SR::handleNicDiscoveryMsg(SRPacket *msg){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	// TODO : the discovery part is not well yet.

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
			// Yes : have I already sent an advertisement message ?
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
			    //sr->setQueryType(Q_REQUEST);
			    scheduleMsgToSendNic(MACAddress::BROADCAST_ADDRESS,sr,10);

				hasSentAdvertMsg = true;

				// Now I am know, so I decrease the number of sensors not known
				numberSensorNotKnown--;
				sensorStatus = STATUS_NORMAL;
				EV << "     numberSensorNotKnown = " <<numberSensorNotKnown <<endl;

			}
		}else {
			// No : Add the sink as neighbor
			addNeighbor(srcMACAddress,srcSensorID);
			// Send him a request
		    SRPacket *sr = new SRPacket("DISCO-REQUEST");
		    sr->setId(myID);
		    sr->setMsgType(MSG_DISCOVERY);
		    sr->setQueryType(Q_REQUEST);
		    scheduleMsgToSendNic(srcMACAddress,sr,0);

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
			addNeighbor(srcMACAddress,srcSensorID);
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
			    //sr->setQueryType(Q_REQUEST);
			    scheduleMsgToSendNic(MACAddress::BROADCAST_ADDRESS,sr,10*iDBuilder->getSinkDistance());

				hasSentAdvertMsg = true;
				// I should be known now, so i decrease the number of sensors not known in the network
				numberSensorNotKnown--;
				sensorStatus = STATUS_NORMAL;

				EV << "     numberSensorNotKnown = " <<numberSensorNotKnown <<endl;
				// Generate a request  after a random time
				//double randomTime = ((rand() %  numberSensorNotKnown+1) + 1.0)*TIME_BETWEEN_ALERTS;
			    //EV << "     Scheduling to send request message in "<< randomTime << " seconds." <<endl;
			    //scheduleAt(simTime()+randomTime,selfGenerateRequestMsg);
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
	scheduleMsgToSendNic(nextHop,alertMsg,0);
}

// Handle normal messages coming from the network
void SR::handleNicNormalMsg(SRPacket *msg){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	//TODO : Fonctionnement normale
}

// Generate a random request and send it in the network
void SR::generaterRandomRequest(){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	// TODO: generate a random request
    // Create an hello world packet
    SRPacket *sr = new SRPacket("NORMAL-REQUEST");
    sr->setId(myID);
    sr->setMsgType(MSG_NORMAL);
    sr->setQueryType(Q_REQUEST);
    scheduleMsgToSendNic(MACAddress::BROADCAST_ADDRESS,sr,0);
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

		// If I am the sink, I should start the simulation
		// SinkDistance=0, Area=0
		iDBuilder->setMACExt(myMACAddress.str());
		iDBuilder->setSinkDistance(0);
		iDBuilder->setArea(0);
		myID = iDBuilder->getID();
	    // Create an hello world packet
	    SRPacket *sr = new SRPacket("DISCO-REQUEST");
	    sr->setId(myID);
	    sr->setMsgType(MSG_DISCOVERY);
	    sr->setQueryType(Q_REQUEST);
	    scheduleMsgToSendNic(MACAddress::BROADCAST_ADDRESS,sr,0);
	    sensorStatus = STATUS_NORMAL;

    }else if (msg == internalAlertMsg)
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
		EV << "     Received Q_REQUEST from " << srcMACAddress << endl;
		// Add the sensor as neighbor
		addNeighbor(srcMACAddress,srcSensorID);

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
	    scheduleMsgToSendNic(MACAddress::BROADCAST_ADDRESS,sr,10);

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

}



