#ifndef __INET_SR_H
#define __INET_SR_H

#include "QueueBase.h"
#include "InterfaceTableAccess.h"
#include "IPControlInfo.h"

class ARPPacket;
//class SRNetDiscovery;

/**
 * Implements the SR protocol.
 */
class INET_API SR : public QueueBase
{
  protected:
    IInterfaceTable *ift;
    cGate *queueOutGate; // the most frequently used output gate
    bool isSink;   // Is this node the sink ?

    long sensoID;  // The unique identifier of a node on the whole network

    /** @brief global counter for generating unique MAC extended address */
    static long addrCount;

    int getLastMACNumber(std::string str);

	/**
	* @name Container used to list my neighbors' mac addresses
	*/
	typedef std::map<long, std::string > NeighborList;
	NeighborList neighborList;

  protected:
    // utility: look up interface from getArrivalGate()
    virtual InterfaceEntry *getSourceInterfaceFrom(cPacket *msg);

    /**
     * Handle incoming ARP packets by sending them over "queueOut" to ARP.
     */
    virtual void handleARP(ARPPacket *msg);

  public:
    SR() {}

  protected:
    /**
     * Initialization
     */
    virtual void initialize();

    /**
     * Processing of IP datagrams. Called when a datagram reaches the front
     * of the queue.
     */
    virtual void endService(cPacket *msg);
    virtual void handleMessage(cMessage *msg);
    virtual void sendPingPong(InterfaceEntry *ie,MACAddress dstMACAddress);
};

#endif

