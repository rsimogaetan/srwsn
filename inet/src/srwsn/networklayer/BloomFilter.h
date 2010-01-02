#ifndef __BLOOMFILTER_H
#define __BLOOMFILTER_H

#include <omnetpp.h>

class BloomFilter : public cSimpleModule
{
public:
	BloomFilter();  // The Constructor
	~BloomFilter();                  // The Destructor
	void toString();				 // For information
private:
	virtual void initialize();
	bool ready;
	int maxEntry;
};

#endif

