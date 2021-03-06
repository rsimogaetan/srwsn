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
typedef std::map<uint16_t, MACAddress> RecordTable_t;

enum STATUS
{
    STATUS_DISCO = 1,      // Network Discovery.
    STATUS_NORMAL = 2,      // Request.
    STATUS_ALERT = 3      // Reply.
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

	/**
	* Record Table to know to which neighbor we forward the answer
	*/
	RecordTable_t RecordTable;

	bool hasSentAdvertMsg;  // For the advertisement message

	STATUS sensorStatus;  // The status of the sensor.
	uint16_t MAX_ALERT_LIVETIME;  // An alert must not be relevant after this amount of time
	uint16_t MAX_ENTRY_RARE; // Maximum number of entry in the tableRARE allocated for alerts
	uint16_t FALSE_ALERT_TIMEOUT; // Time after which this alert become a false alert
	uint16_t LAST_ALERT_TIMESTAMP; // Contains the timestamp of the last alert
	double LAST_ALERT_ABSOLUTE_TIME; // The absolute time of the last alert
	uint16_t TIME_BETWEEN_ALERTS;  // The minimum difference bettween alerts timestamps

	uint32_t LIVETIME;  // An event must not be relevant after this amount of time
	uint32_t NB_EVENT_IN_LIVETIME; // Maximum number of events in one livetime
	uint32_t EVENT_MAX_LIVETIME; // Time after which another event can be thrown
	uint32_t currentRequestTimeStamp; // The timestamp of the current request on the network
	static uint64_t generatorBitmap;  // A bitmap used to know in which time slot I can generate a request
	static uint32_t lastPosition;
	static uint16_t areaBitmap;


	cMessage *selfInitializationMsg;  // Message to send for self initialization
	cMessage *internalAlertMsg;      // Message to throw when an internal anomaly is detected
	cMessage *falseAlertTimeoutMsg;  // Message to schedule as timeout for false alert
	cMessage *selfGenerateRequestMsg;  // Message to schedule a random request
	cMessage *updateStatsMsg;  // To update statistics

	static uint16_t numberSensorNotKnown; // The number of sensors not known in the network yet
	static uint16_t numberSensorInNetwork; // The total number of connected sensor in the network
	static bool iCanSendRequest;   // To know if I can send a request now. Only one request at the time on the network
	static bool stopSim;
	bool iAmTheLast;

	/*
     *  Statistic numbers
     */
    long numSent;      // The number of packets sent
    long numReceived;  // The number of packets received
    cLongHistogram hopCountPerRequestStats;  // Count the number of hops until reaches destination
    cOutVector hopCountPerRequestVector;
    cOutVector NBMsgSentPerRequestVector;
    cOutVector NBMsgRcvdPerRequestVector;
    cOutVector NBMsgRcvdPerRequestVector2;

	static long NBTotalMessageSent;
	static long NBTotalMessageRcvd;
	static long hopCountPerRequest;
    static long NBMsgSentPerRequest;
    static long NBMsgRcvdPerRequest;

  public:
    SR() {};

  protected:
	  /*
	   * Methodes used by every node in the network
	   */

    // Initialization
    virtual void initialize();
    // This method receives first packets on the queue
    virtual void endService(cPacket*);
    // The time to print statistics
    virtual void finish();
    // This method receives all message sent to this module
    virtual void handleMessage(cMessage *msg);

    // Send the SRPacket to the destination node after some time
    virtual void scheduleMsgToSendNic( SRPacket * srPacket,uint16_t delayMin);

	// Add a neighbor
    virtual void addNeighbor(MACAddress neighborMACAddress, uint16_t neighborID);
    virtual void addNeighbor(MACAddress neighborMACAddress, uint16_t neighborID,BloomFilter neighborBloom);

    // return true if an alert can be thrown at this time
    virtual bool isAlertAllowed();
    // Notify all my neighbor of an anomaly I detected
    virtual void broadcastAlert();
    // Return true if the neighbor alert timestamp is close enough to ours
    virtual bool isNeighborAlertRelevant(uint16_t neighborAlertTimeStamp);

    // Print all my neighbors
    virtual void dumpNeighbors();
    // Change the color of my display
    virtual void changeDisplay();
    // Display informations on the node
    virtual void updateDisplay();
    // Return the mac Addresse corresponding to that id
    virtual MACAddress idToMACAddress(uint16_t id);
    // This method helps to schedule a time when to generate a request
    virtual void scheduleTimeToGenerateRequest();
  protected:
	  /*
	   * Methodes used only by simple nodes
	   */

	    // Handle messages generated by me
	    virtual void handleSelfMsg(cMessage *msg);
	    // Handle Alert message generated by me
	    virtual void handleInternalAlertMsg(cMessage *msg);
	    // Handle False alert generated my me
	    virtual void handleFalseAlertMsg(cMessage *msg);

	    // Handle message coming from the network
	    virtual void handleSRMsg(SRPacket *msg);
	    // Handle discovery message coming from the network
	    virtual void handleNicDiscoveryMsg(SRPacket *msg);
	    // Handle Alert messages coming from the network
	    virtual void handleNicAlertMsg(SRPacket *msg);
	    // Handle normal messages coming from the network
	    virtual void handleNicNormalMsg(SRPacket *msg);

	    // Generate a random request and send it in the network
	    virtual void generaterRandomRequest();
  protected:
	    /*
	     * Methodes used by the sink only
	     */
	  // handle all messages coming to this module, for a sink
	    virtual void SINK_handleMessage(cMessage *msg);

	    // Handle messages generated by me
	    virtual void SINK_handleSelfMsg(cMessage *msg);

	    // Handle messages coming from the network
	    virtual void SINK_handleSRMsg(SRPacket *msg);
	    // Handle discovery message coming from the network
	    virtual void SINK_handleNicDiscoveryMsg(SRPacket *msg);
	    // Handle Alert messages coming from the network
	    virtual void SINK_handleNicAlertMsg(SRPacket *msg);
	    // Handle normal messages coming from the network
	    virtual void SINK_handleNicNormalMsg(SRPacket *msg);

	    virtual void SINK_updateStats();
};

#endif

