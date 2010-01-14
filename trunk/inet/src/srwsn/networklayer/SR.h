#ifndef __INET_SR_H
#define __INET_SR_H
#include <iostream>
#include <stdint.h>  // Use [u]intN_t if you need exactly N bits.

#include "QueueBase.h"
#include "BloomTable.h"
#include "TableRARE.h"
#include "SRPacket_m.h"
#include "InterfaceEntry.h"
#include "IDBuilder.h"

typedef std::map<int, MACAddress> NeighborList_t;

enum STATUS
{
    STATUS_NORMAL = 1,      // Request.
    STATUS_ALERT = 2      // Reply.
};
/**
 * Implements the SR protocol.
 */

class INET_API SR : public QueueBase
{
  protected:
	BloomTable *bt;
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

	STATUS sensorStatus;  // The status of the sensor.
	uint16_t MAX_ALERT_LIVETIME;  // An alert must not be relevant after this amount of time
	uint16_t MAX_ENTRY_RARE; // Maximum number of entry in the tableRARE allocated for alerts
	uint16_t FALSE_ALERT_TIMEOUT; // Time after which this alert become a false alert
	uint16_t LAST_ALERT_TIMESTAMP; // Contains the timestamp of the last alert
	uint16_t TIME_BETWEEN_ALERTS;  // The minimum difference bettween alerts timestamps

	cMessage *selfInitializationMsg;  // Message to send for self initialization
	cMessage *internalAlertMsg;      // Message to throw when an internal anomaly is detected
	cMessage *falseAlertTimeoutMsg;  // Message to schedule as timeout for false alert

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
    virtual void SINK_handleMessage(cMessage *msg);
    virtual void handleSelfMsg(cMessage *msg);
    virtual void SINK_handleSelfMsg(cMessage *msg);
    virtual void handleSRMsg(SRPacket *msg);
    virtual void SINK_handleSRMsg(SRPacket *msg);
    virtual void handleNicAlertMsg(SRPacket *msg);
    virtual void SINK_handleNicAlertMsg(SRPacket *msg);
    virtual void handleInternalAlertMsg(cMessage *msg);
    virtual void handleFalseAlertMsg(cMessage *msg);
    virtual void handleDiscoveryMsg(SRPacket *msg);

    virtual void sendLater(MACAddress dstMACAddress,uint16_t delayMin);
    virtual void sendMsgToNic(MACAddress destMACAddress, SROpcode srOpcode,int16_t delayMin);
    virtual void scheduleMsgToSendNic(MACAddress destMACAddress, SRPacket * srPacket,uint16_t delayMin);

    virtual void dumpNeighbors();
    virtual void addNeighbor(MACAddress neighborMACAddress, uint16_t neighborID);  // Add a neighbor

    virtual bool isAlertAllowed();  // return true if an alert can be through at time time
    virtual void broadcastAlert();  // Notify all my neighbor of an anomaly I detected
    virtual bool isNeighborAlertRelevant(uint16_t neighborAlertTimeStamp); // Return true if the neighbor alert timestamp is close enough to ours
};

#endif

