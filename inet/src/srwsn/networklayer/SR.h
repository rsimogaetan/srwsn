#ifndef __INET_SR_H
#define __INET_SR_H
#include <iostream>
#include <stdint.h>  // Use [u]intN_t if you need exactly N bits.

#include "QueueBase.h"
#include "BloomFilter.h"
#include "TableRARE.h"
#include "SRPacket_m.h"
#include "InterfaceEntry.h"
#include "IDBuilder.h"

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

    uint16_t myID;  // The unique identifier of a node on the whole network
    IDBuilder * iDBuilder; // An utility to manipulate ids
    uint16_t areaNumber; // The number of areas in this network
						 // It should not exceed 4

    bool firstSinkMsg;  // To know if this is the first message of the sink

	/**
	* @name Container used to list my neighbors' mac addresses
	*/
	NeighborList_t neighborList;

	bool hasSentAdvertMsg;  // For the advertisement message
  public:
    SR() {};

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
    virtual void handleSRMsg(SRPacket *msg);
    virtual void handleDiscoveryMsg(SRPacket *msg);
    virtual void sendLater(MACAddress dstMACAddress,uint16_t delayMin);
    virtual void dumpNeighbors();
    virtual void addNeighbor(MACAddress neighborMACAddress, uint16_t neighborID);  // Add a neighbor
    virtual void sendMsgToNic(MACAddress destMACAddress, SROpcode srOpcode,int16_t delayMin);
};

#endif

