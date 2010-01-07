#ifndef __INET_SR_H
#define __INET_SR_H

#include "QueueBase.h"
#include "BloomFilter.h"
#include "TableRARE.h"
#include "SRPacket_m.h"
#include "InterfaceEntry.h"
#include <iostream>

typedef std::map<int, MACAddress> NeighborList_t;

/**
 * Implements the SR protocol.
 */

class INET_API SR : public QueueBase
{
  protected:
	BloomFilter *bf;
	TableRARE *tr;
	MACAddress myMACAddress;

    cGate *queueOutGate; // the most frequently used output gate
    bool isSink;   // Is this node the sink ?

    long myID;  // The unique identifier of a node on the whole network

    int LastMACNumberToInt(std::string str); // Return the last MAC number in decimal

	/**
	* @name Container used to list my neighbors' mac addresses
	*/
	NeighborList_t neighborList;

	bool hasSentAdvertMsg;  // For the advertisement message
  protected:
    /**
     * Handle incoming SR packets by sending them over "queueOut" to ARP.
     */
    virtual void handleSR(SRPacket *msg);

  public:
    SR() {}

  protected:
    /**
     * Initialization
     */
    virtual void initialize();

    /**
     * Processing of SR datagrams. Called when a datagram reaches the front
     * of the queue.
     */
    virtual void endService(cPacket *msg);
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    virtual void handleSelfMsg(cMessage *msg);
    virtual void sendPingPong(MACAddress dstMACAddress);
    virtual void dumpNeighbors();
};

#endif

