#include <omnetpp.h>
#include <stdlib.h>
#include <string.h>

#include "SR.h"
#include "IPv4InterfaceData.h"
#include "ARPPacket_m.h"
#include "Ieee802Ctrl_m.h"

// Each node has an unique identifier on the network
// We ca have up to 2^16 node in this network
long SR::addrCount = 0;

Define_Module(SR);

void SR::initialize()
{
    QueueBase::initialize();
    sensoID = addrCount++;

    ift = InterfaceTableAccess().get();

    queueOutGate = gate("queueOut");
    isSink = par("isSink");

// If I am TIC, I send the first ping
	if(isSink){
		cMessage *msg = new cMessage("sendPing");
		scheduleAt(simTime()+3.0, msg);
	}
}


void SR::endService(cPacket *msg)
{
	EV << "SR::endService\n";
}

void SR::handleMessage(cMessage *msg)
{
	EV << "SR::handleMessage\n";

    if ((msg->isSelfMessage()))
    {
		InterfaceEntry *ie;
		int ii = 0;
		do{
			ie = ift->getInterface(ii);
			ii++;
		}while(ie->isLoopback());

    	MACAddress dstMACAddress = "FF-FF-FF-FF-FF-FF";
		sendPingPong(ie, dstMACAddress);
    }else 	if (dynamic_cast<ARPPacket *>(msg))
    {
        // dispatch ARP packets to ARP
        handleARP((ARPPacket *)msg);
    }else{
		InterfaceEntry *ie;
		int ii = 0;
		do{
			ie = ift->getInterface(ii);
			ii++;
		}while(ie->isLoopback());

    	MACAddress dstMACAddress = "FF-FF-FF-FF-FF-FF";
		sendPingPong(ie, dstMACAddress);
    }
}

InterfaceEntry *SR::getSourceInterfaceFrom(cPacket *msg)
{
	EV << "SR::getSourceInterfaceFrom\n";

    cGate *g = msg->getArrivalGate();
    return g ? ift->getInterfaceByNetworkLayerGateIndex(g->getIndex()) : NULL;
}

void SR::handleARP(ARPPacket *msg)
{
    // FIXME hasBitError() check  missing!
	EV << "SR::handleARP\n";

	// delete old control info
    delete msg->removeControlInfo();

    // dispatch ARP packets to ARP and let it know the gate index it arrived on
    InterfaceEntry *fromIE = getSourceInterfaceFrom(msg);
    ASSERT(fromIE);

    ARPPacket *arp_in = (ARPPacket *)msg;

	MACAddress srcMACAddress = arp_in->getSrcMACAddress();
	std::string mac = srcMACAddress.str();

	sensoID = getLastMACNumber(mac);

	// Check if I already know that node
	if (mac.compare(neighborList[sensoID])==0)
	{
		return;
	}

	EV << "neighborList[sensoID] = " <<neighborList[sensoID] << endl;
	neighborList[sensoID] = mac;

	MACAddress dstMACAddress = "FF-FF-FF-FF-FF-FF";

	sendPingPong(fromIE,dstMACAddress);
}

int SR::getLastMACNumber(std::string str)
{
    // Create a copy a buffer
    size_t size = str.size() + 1;
    char * buffer = new char[ size ];
    // copier la chaîne
    strncpy( buffer, str.c_str(), size );

	std::stringstream convertor;
	long dec_n = 0;
	char * hex_n = buffer+16;

	// Convertion Hexa (char*) to decimale (int)
	convertor << hex_n;
	convertor >> std::hex >> dec_n;

    // free memory
    delete [] buffer;

	return dec_n;
}

void SR::sendPingPong(InterfaceEntry *ie,MACAddress dstMACAddress){
	EV << "SR::sendPingPong\n";

	MACAddress myMACAddress = ie->getMacAddress();

	EV << "myMACAddress:" << myMACAddress << "; dstMACAddress:" << dstMACAddress << "\n";

    // Must be set
    ASSERT(!myMACAddress.isUnspecified());

    // fill out everything in ARP Request packet except dest MAC address
    ARPPacket *arp = new ARPPacket("HelloWorld");
    arp->setByteLength(ARP_HEADER_BYTES);
    arp->setOpcode(ARP_REQUEST);
    arp->setSrcMACAddress(myMACAddress);

    // add control info with MAC address
    Ieee802Ctrl *controlInfo = new Ieee802Ctrl();
    controlInfo->setDest(dstMACAddress);
    arp->setControlInfo(controlInfo);

    // send out
    sendDirect(arp, getParentModule(), "ifOut",
                                  ie->getNetworkLayerGateIndex());
}

