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
#include "BloomFilterAccess.h"
#include "TableRAREAccess.h"
#include "BloomTableAccess.h"
#include "InterfaceTableAccess.h"

Define_Module(SR);

void SR::initialize()
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;

    QueueBase::initialize();
    myID = 0;

    bf = BloomFilterAccess().get();
    tr = TableRAREAccess().get();

    queueOutGate = gate("queueOut");
    isSink = par("isSink");
    hasSentAdvertMsg = false;

	myID = 65535 ;  // My ID is not initialize yet
    iDBuilder = new IDBuilder();  // An utility to manipulate ids

    areaNumber = 0 ; // Just the sink need to use this parameter
    firstSinkMsg = true; // To know if this is the first message of the sink

	// Initialize the pseudo-random number generator
    srand( time( NULL ) );

    cMessage *msg = new cMessage("SelfInitialization");
	scheduleAt(simTime()+1.0, msg);
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

void SR::handleSelfMsg(cMessage *msg)
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	if (strcmp(msg->getName(),"SelfInitialization")==0)
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
		if(isSink){
			// SinkDistance=0, Area=0
			iDBuilder->setMACExt(myMACAddress.str());
			iDBuilder->setSinkDistance(0);
			iDBuilder->setArea(0);
			myID = iDBuilder->getID();
			MACAddress BroadcastAddress ;
			BroadcastAddress.setBroadcast();
			sendLater(BroadcastAddress,0);
		}
    }else if (strcmp(msg->getName(),"InternalAlerte")==0)
    {
    	EV << "     An alert just occurred here" <<endl;
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

// I am the sink ?
// Yes: receive data
	// Is it a request ?
	// Yes : add the sensor as neighbor
		// send him an area message
	// No : do nothing
// No : receive data
	// Is it the sink sending ?
	// Yes : do I know him ?
		// Yes : have I already sent an advertisement message ?
			// Yes : do nothing
			// No : Get an ID
				// Send an advertisement message
		// No : Add the sink as neighbor
			// Send him a request
	// No : do I knom him ?
		// Yes : do nothing
		// No :  Add this sensor as neighbor
			// have I already sent an advertisement message ?
			// Yes : do nothing
			// No : Send an advertisement message

void SR::handleSRMsg(SRPacket *msg)
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	MACAddress broadcastAddress ;
	broadcastAddress.setBroadcast();

	// delete old control info
    delete msg->removeControlInfo();
	MACAddress srcMACAddress = msg->getSrcMACAddress();
	uint16_t srcSensorID = msg->getId();

	// I am the sink ?
	if(isSink){
	// Yes: Is it a request ?
		switch (msg->getOpcode()){
		case SR_REQUEST: {
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
		}
		case SR_REPLY :{
			// do nothing
			break;
		}
		}
	}else{
		// No :
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
					sendMsgToNic(broadcastAddress,SR_INFO,10);
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
			// No : do I knom him ?
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
					sendMsgToNic(broadcastAddress,SR_INFO,10*iDBuilder->getSinkDistance());
					hasSentAdvertMsg = true;
				}
			}

		}

	}

}

/*
void SR::handleSRMsg(SRPacket *msg)
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
10
	// delete old control info
    delete msg->removeControlInfo();

	MACAddress srcMACAddress = msg->getSrcMACAddress();
	uint16_t srcSensorID = msg->getId();

	EV << "     received message from sensor ID " << srcSensorID << endl;
	// Else, check if I already know that node
	if(neighborList.find(srcSensorID) != neighborList.end())
	{
		EV << "     neighborList["<<srcSensorID<<"] = " <<neighborList[srcSensorID] << " . Already registered here!" << endl;
	}else if(isSink){
		EV << "     I am the sink, I should assign areas. You are the area number " << areaNumber <<endl;
		// So I add it to my neighbor list
		neighborList[srcSensorID] = srcMACAddress;
		EV << "     New neighbor <" << srcSensorID <<","<< srcMACAddress.str() << "> added "<< endl;

		iDBuilder->setID(myID);
		iDBuilder->setArea(areaNumber);
		myID = iDBuilder->getID();
		sendLater(srcMACAddress,0);
		areaNumber++;
	}else{
		// Here, I do not know the sender
		iDBuilder->setID(srcSensorID);
		// Is this the sink
		if(iDBuilder->getSinkDistance()==0){
			// Was I waiting for a message from him ?
			if(myID == 0){ // Yes
				// I should send back a message to the sink to ask my area
				myID = 65535;
				sendLater(srcMACAddress,0);
			}else{ // No
				// The sink just assigned my area
				iDBuilder->setID(srcSensorID);
				iDBuilder->setMACExt(myMACAddress.str());
				iDBuilder->setSinkDistance(iDBuilder->getSinkDistance()+1);
				myID = iDBuilder->getID();

				// TODO: send the broadcast message
			}

		}else{
			// So I add it to my neighbor list
			neighborList[srcSensorID] = srcMACAddress;
			EV << "     New neighbor <" << srcSensorID <<","<< srcMACAddress.str() << "> added "<< endl;

			// Check if I already have sent an advertisement message
			if (hasSentAdvertMsg){
				// He should already have received my first advertisement message
				EV << "     I already have sent an Advertisement message" << endl;
			}else{
				// This should be the time to send the advertisement message
				EV << "     Sending my first Advertisement message" << endl;
				iDBuilder->setID(srcSensorID);
				iDBuilder->setMACExt(myMACAddress.str());
				iDBuilder->setSinkDistance(iDBuilder->getSinkDistance()+1);
				myID = iDBuilder->getID();
				dumpNeighbors();
				MACAddress BroadcastAddress ;
				BroadcastAddress.setBroadcast();
				sendLater(BroadcastAddress,10*iDBuilder->getSinkDistance());
				hasSentAdvertMsg = true;
			}
		}
	}
}
*/

void SR::handleDiscoveryMsg(SRPacket *msg){

}

void SR::sendLater(MACAddress dstMACAddress,uint16_t delayMin){
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	EV << "     myMACAddress:" << myMACAddress << "; dstMACAddress:" << dstMACAddress << endl;
    // Must be set
    ASSERT(!myMACAddress.isUnspecified());

    // fill out everything in ARP Request packet except dest MAC address
    SRPacket *sr = new SRPacket("HelloWorld");
    sr->setSrcMACAddress(myMACAddress);
    sr->setId(myID);

    // add control info with MAC address
    Ieee802Ctrl *controlInfo = new Ieee802Ctrl();
    controlInfo->setDest(dstMACAddress);
    sr->setControlInfo(controlInfo);

	double randomTime = (rand() % 10) + 1.0 + (double)delayMin;
    EV << "     Scheduling to send the message in "<< randomTime << " seconds." <<endl;
    scheduleAt(simTime()+randomTime,sr);
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
	}
    // fill out everything in ARP Request packet except dest MAC address
    sr->setSrcMACAddress(myMACAddress);
    sr->setId(myID);
    sr->setOpcode(srOpcode);

    // add control info with MAC address
    Ieee802Ctrl *controlInfo = new Ieee802Ctrl();
    controlInfo->setDest(destMACAddress);
    sr->setControlInfo(controlInfo);

	double randomTime = (rand() % 10) + 1.0 + (double)delayMin;
    EV << "     Scheduling to send the message in "<< randomTime << " seconds." <<endl;
    scheduleAt(simTime()+randomTime,sr);
}
