#include <omnetpp.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdlib.h>  // For rand
#include <time.h>  // For rand

#include "SR.h"
#include "IPv4InterfaceData.h"
#include "SRPacket_m.h"
#include "Ieee802Ctrl_m.h"
#include "BloomFilter.h"
#include "BloomFilterAccess.h"
#include "TableRARE.h"
#include "TableRAREAccess.h"
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
	EV <<"[SR]::"<< __FUNCTION__ <<" Sensor:" << myID <<endl;
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
   		handleSR((SRPacket*)msg);
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
		myID = LastMACNumberToInt(myMACAddress.str());

		// If I am the sink, I should start the simulation
		if(isSink){
			MACAddress BroadcastAddress ;
			BroadcastAddress.setBroadcast();
			sendPingPong(BroadcastAddress);
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
    		display_string->setTagArg("i", 1, "gold");
    	}
    }
}

void SR::handleSR(SRPacket *msg)
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;

	// delete old control info
    delete msg->removeControlInfo();

	MACAddress srcMACAddress = msg->getSrcMACAddress();
	int srcSensorID = msg->getId();

	EV << "     received message from sensor ID " << srcSensorID << endl;
	// Check if I already know that node
//	if (srcMACAddress.str().compare(neighborList[srcSensorID])==0)
	if(neighborList.find(srcSensorID) != neighborList.end())
	{
		EV << "     neighborList["<<srcSensorID<<"] = " <<neighborList[srcSensorID] << " . Already registered here!" << endl;
	}else{
		// Here, I do not know the sender
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
			dumpNeighbors();
			MACAddress BroadcastAddress ;
			BroadcastAddress.setBroadcast();
			sendPingPong(BroadcastAddress);
			hasSentAdvertMsg = true;
		}
	}
}


void SR::sendPingPong( MACAddress dstMACAddress){
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

	double randomTime = (rand() % 10) + 1.0;
    EV << "     Scheduling to send the message in "<< randomTime << " seconds." <<endl;
    scheduleAt(simTime()+randomTime,sr);
}

// Return the last number of the MAC Address in decimal
// i.e. 00-12-EF-32-AA-AE  will return AE = 174
int SR::LastMACNumberToInt(std::string mac)
{
	EV <<"[SR]::"<< __FUNCTION__ <<endl;
	std::stringstream convertor;
	uint16_t dec_n ;
	unsigned int cutAt;

	std::string delim= "-";

	while( (cutAt = mac.find_first_of(delim)) != mac.npos )
	{
		mac = mac.substr(cutAt+1);
	}
	if(mac.length() < 0)
		return -1;

	// Convert Hexa (string) to decimal (int)
	convertor << mac;
	convertor >> std::hex >> dec_n;

	return (dec_n-1);
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
