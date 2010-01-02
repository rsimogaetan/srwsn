#include <omnetpp.h>
#include "BloomFilter.h"

// The module class needs to be registered with OMNeT++
Define_Module(BloomFilter);

// The Constructor
BloomFilter::BloomFilter() {
}

// The Destructor
BloomFilter::~BloomFilter() {
//	delete my_name;
}

void BloomFilter::initialize()
{
	ready = par("ready");
	maxEntry = par("maxEntry");
}


//Print information about this object
void BloomFilter::toString(){
	EV << "[BloomFilter] Hello, I am ready ? " << ready
	<<" ; max entry :" << maxEntry << endl;
}
