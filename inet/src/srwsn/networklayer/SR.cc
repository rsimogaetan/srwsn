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

void SR::initialize()
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;

    QueueBase::initialize();

    bt = BloomTableAccess().get(); // Retrive the BloomTable
    tr = TableRAREAccess().get();  // Retrive the TableRARE

    queueOutGate = gate("queueOut");
    isSink = par("isSink");
    hasSentAdvertMsg = false;

	myID = 65535 ;  // My ID is not initialize yet
    iDBuilder = new IDBuilder();  // An utility to manipulate ids

    areaNumber = 0 ; // Just the sink need to use this parameter
    firstSinkMsg = true; // To know if this is the first message of the sink

	// Initialize the pseudo-random number generator
    srand( time( NULL ) );

    sensorStatus = STATUS_NORMAL; // The sensor status
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

	// Schedule the self initialization
	scheduleAt(simTime()+1.0, selfInitializationMsg);
}


void SR::endService(cPacket *msg)
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
}
void SR::finish(){
	EV <<"[SR]::"<< __FUNCTION__ ;
	iDBuilder->setID(myID);
	EV <<" Sensor:" << iDBuilder->getID()
	<< " => mac extended=" << iDBuilder->getMACExt()
	<< ", sink Distance=" << iDBuilder->getSinkDistance()
	<< ", area=" << iDBuilder->getArea() << endl;
	dumpNeighbors();
}

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
    	// Error
    }
}


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

		// This is all I need to do in the initialization process
    }else if (msg == internalAlertMsg)
    {
    	EV << "     An alert just occurred here" <<endl;
    	handleInternalAlertMsg(msg);
    }else if (msg == falseAlertTimeoutMsg){
    	handleFalseAlertMsg(msg);
    }else if (dynamic_cast<SRPacket *>(msg)){
    	// This packet has been schedule by me to send now
    	SRPacket * sr = (SRPacket*)msg;
    	if(myMACAddress.compareTo(sr->getSrcMACAddress())==0){
        	EV << "     Sending message now. " << "myMACAddress:" << myMACAddress <<endl;
			// I am the sender of this packet
    	    // send out
    	    sendDirect(sr, getParentModule(), "ifOut",0);

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
    }
}
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

		// If I am the sink, I should start the simulation
		// SinkDistance=0, Area=0
		iDBuilder->setMACExt(myMACAddress.str());
		iDBuilder->setSinkDistance(0);
		iDBuilder->setArea(0);
		myID = iDBuilder->getID();
		sendLater(MACAddress::BROADCAST_ADDRESS ,0);
    }else if (msg == internalAlertMsg)
    {
    	EV << "     An alert just occurred here. I am the sink, I take care of everything !" <<endl;
    }else if (dynamic_cast<SRPacket *>(msg)){
    	// This packet has been schedule by me to send now
    	SRPacket * sr = (SRPacket*)msg;
    	if(myMACAddress.compareTo(sr->getSrcMACAddress())==0){
        	EV << "     Sending message now. " << "myMACAddress:" << myMACAddress <<endl;
			// I am the sender of this packet
    	    // send out
    	    sendDirect(sr, getParentModule(), "ifOut",0);

    		// change icon displayed in Tkenv
    		cDisplayString* display_string = &getParentModule()->getParentModule()->getDisplayString();
    		iDBuilder->setID(myID);
    		std::string color = "#ffffff";

    		size_t size = color.size() + 1;
   		    char * buffer = new char[ size ];
   		    strncpy( buffer, color.c_str(), size );
    		display_string->setTagArg("i", 1, buffer);
    		delete [] buffer;
    	}
    }

}

// This function should notice the sink from this error
void SR::handleInternalAlertMsg(cMessage *msg){
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

// This function is called when the FALSE_ALERT_TIMEOUT expires
void SR::handleFalseAlertMsg(cMessage *msg){
	// This message get here if I didn't get any response
	// from my neighbors after the FALSE_ALERT_TIMEOUT time

	// Do nothing if my status is not alert
	if(sensorStatus != STATUS_ALERT)
		return;

	// So this alert was just a false one
	// Set my status back to normal
	sensorStatus = STATUS_NORMAL;
}

void SR::handleSRMsg(SRPacket *msg)
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;

	// delete old control info
	delete msg->removeControlInfo();

	// We do not treat alert messages here
	if (msg->getOpcode() == SR_ALERT){
		handleNicAlertMsg(msg);
		return;
	}

	MACAddress srcMACAddress = msg->getSrcMACAddress();
	uint16_t srcSensorID = msg->getId();


	// I am not the sink
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

				// No : Send an advertisement message
				sendMsgToNic(MACAddress::BROADCAST_ADDRESS,SR_INFO,10);
				hasSentAdvertMsg = true;
			}
		}else {
			// No : Add the sink as neighbor
			addNeighbor(srcMACAddress,srcSensorID);
			// Send him a request
			sendMsgToNic(srcMACAddress,SR_REQUEST,0);
			firstSinkMsg = false;
		}
	}else{
		// It is not eh sink sending
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

				// No : Send an advertisement message
				sendMsgToNic(MACAddress::BROADCAST_ADDRESS,SR_INFO,10*iDBuilder->getSinkDistance());
				hasSentAdvertMsg = true;
			}
		}

	}
}

void SR::SINK_handleSRMsg(SRPacket *msg){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;

	// delete old control info
	delete msg->removeControlInfo();

	// We do not treat alert messages here
	if (msg->getOpcode() == SR_ALERT){
		SINK_handleNicAlertMsg(msg);
		return;
	}

	MACAddress srcMACAddress = msg->getSrcMACAddress();
	uint16_t srcSensorID = msg->getId();

	// I am the sink ? YES
	// Yes: Is it a request ?
	switch (msg->getOpcode()){
	case SR_REQUEST:
		EV << "     Received SR_REQUEST from " << srcMACAddress << endl;
		// Add the sensor as neighbor
		addNeighbor(srcMACAddress,srcSensorID);

		// Set my ID in this area
		iDBuilder->setID(myID);
		iDBuilder->setArea(areaNumber);
		myID = iDBuilder->getID();
		areaNumber++;

		// send him an area message
		sendMsgToNic(srcMACAddress,SR_REPLY,10);
		break;
	case SR_REPLY :
		// do nothing
		break;
	}
}

void SR::handleNicAlertMsg(SRPacket *alertMsg){
	// I just received an alert message from a neighbor

	// Was this alert generated by my neighbor ?
	if(!alertMsg->getAmIAlertGenerator()){
		// This alert is not originated by my neighbor
		// So I just need to forward it
		//TODO : Send this message to the sink
		return;
	}


	// This alert was generated by my neighbor
	// I should take care of it
	// I am not if the alert mode, so I do nothing
	if(sensorStatus != STATUS_ALERT) return;

	// I am in the alert mode too. let see if we detect the same alert
	uint16_t neighborAlertTimeStamp = alertMsg->getAlertTimeStamp();
	// If the alert detected by my neighbor is too different from mine,
	// well I wont support it.
	if(!isNeighborAlertRelevant(neighborAlertTimeStamp)) return;

	// If my neighbor alert is near than mine,
	// That's not good. WE ARE IN ALERT !
	// First I must disable the FALSE_ALERT_TIMEOUT by canceling
	// the timeout message from coming back;
	cancelEvent(falseAlertTimeoutMsg);

	// I must set the message correctly
	alertMsg->setAmIAlertGenerator(false);
	// I send it to the sink ASAP (through my neighbor)

	alertMsg->setOpcode(SR_ALERT);
	alertMsg->setId(myID);

	// Find the next hop of this message
	// TODO : the next hop should be a node, not a broadcast Address (TableRARE)
	MACAddress nextHop = MACAddress::BROADCAST_ADDRESS;
	scheduleMsgToSendNic(nextHop,alertMsg,0);
}

// This function can (may) implement what the sink to when an alert
//  message reaches him
void SR::SINK_handleNicAlertMsg(SRPacket *msg){
	//TODO : implement this function
}

void SR::handleDiscoveryMsg(SRPacket *msg){
	// TODO : the discovery part is not well yet.
}

void SR::sendLater(MACAddress destMACAddress,uint16_t delayMin){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	EV << "     myMACAddress:" << myMACAddress << "; destMACAddress:" << destMACAddress << endl;
    // Must be set
    ASSERT(!myMACAddress.isUnspecified());

    // Create an hello world packet
    SRPacket *sr = new SRPacket("HelloWorld");
    sr->setId(myID);

    scheduleMsgToSendNic(destMACAddress,sr,delayMin);
}

// Print the content of my neighbor list
void SR::dumpNeighbors()
{
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

// Add a neighbor
void SR::addNeighbor(MACAddress neighborMACAddress, uint16_t neighborID){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	neighborList[neighborID] = neighborMACAddress;
	EV << "     New neighbor <" << neighborID <<","<< neighborMACAddress.str() << "> added "<< endl;
}

/*
void SR::sendRequestMsgToNic(SRPacket *srMsg){

}
void SR::sendReplyMsgToNic(SRPacket *srMsg){

}
void SR::sendInfoMsgToNic(SRPacket *srMsg){

}
*/

// Send a message
void SR::sendMsgToNic(MACAddress destMACAddress, SROpcode srOpcode,int16_t delayMin){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	SRPacket *sr;
	switch (srOpcode){
		case SR_REQUEST: {
			sr = new SRPacket("SR_REQUEST");
			break;
		}
		case SR_REPLY:{
			sr = new SRPacket("SR_REPLY");
			break;
		}
		case SR_INFO:{
			sr = new SRPacket("SR_INFO");
			break;
		}
		case SR_ALERT:{
			sr = new SRPacket("SR_ALERT");
			break;
		}
	}
    // fill out everything in ARP Request packet except dest MAC address
    sr->setOpcode(srOpcode);
    sr->setId(myID);

    scheduleMsgToSendNic(destMACAddress,sr,delayMin);
}

void SR::scheduleMsgToSendNic(MACAddress destMACAddress, SRPacket * srPacket, uint16_t delayMin){

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

// Return true if an alert can be through at time time
bool SR::isAlertAllowed(){
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
	// TODO : implement this function
}

// Return true if the neighbor alert timestamp is close enough to ours
bool SR::isNeighborAlertRelevant(uint16_t neighborAlertTimeStamp){
	// Just return tru for now;
	return true;
	/*
	 * TODO : implement this function
	if((neighborAlertTimeStamp != LAST_ALERT_TIMESTAMP)||
			(neighborAlertTimeStamp))
		return;
*/
}
