#include "srwsn802154Net.h"

Define_Module(srwsn802154Net);

/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ===================================
/**
 * Initialization routine
 */
void srwsn802154Net::initialize(int aStage)
{
	cSimpleModule::initialize(aStage); //DO NOT DELETE!!
	if (0 == aStage)
	{
		// WirelessMacBase stuff...
        	mUppergateIn  = findGate("uppergateIn");
		mUppergateOut = findGate("uppergateOut");
		mLowergateIn  = findGate("lowergateIn");
		mLowergateOut = findGate("lowergateOut");

		m_moduleName	= getParentModule()->getFullName();

		m_debug			= par("debug");
		isPANCoor		= par("isPANCoor");

		numForward		= 0;
		stop			= 0;
	}
// If I am TIC, I send the first ping
	if(strcmp("host[0]",m_moduleName)==0){
		cMessage *msg = new cMessage("pingpong");
		scheduleAt(simTime()+3.0, msg);
	}
}

void srwsn802154Net::finish()
{
	recordScalar("num of pkts forwarded", numForward);
}

/////////////////////////////// Msg handler ///////////////////////////////////////
void srwsn802154Net::handleMessage(cMessage* msg)
{

	EV << "srwsn802154Net::handleMessage\n";
	EV << "m_moduleName : " << m_moduleName <<"\n";

	while(stop < 10){
		stop++;
		EV << "m_moduleName : " << m_moduleName <<"\n";
//		handleMessage(msg);
	}

	// coming from MAC layer
	if (msg->getArrivalGateId() == mLowergateIn)
	{
//		error("Handle Fromw Lower layer");
		sendTo("host[2]");
	}else{ // Is self msg
		sendTo("host[1]");
	}

}


void srwsn802154Net::sendTo(const char * dest)
{
	Ieee802154AppPkt* appPkt = new Ieee802154AppPkt("srwsn802154Net_MSG");

	appPkt->setBitLength(1*8);
	appPkt->setSourceName(m_moduleName);
	appPkt->setDestName(dest);
	appPkt->setCreationTime(simTime());

	Ieee802154NetworkCtrlInfo *control_info = new Ieee802154NetworkCtrlInfo();
	control_info->setToParent(false);
	control_info->setDestName(appPkt->getDestName());
	appPkt->setControlInfo(control_info);
	send(appPkt, mLowergateOut);
}
