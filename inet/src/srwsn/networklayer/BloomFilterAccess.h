#ifndef __INET_BLOOM_FILTER_ACCESS_H
#define __INET_BLOOM_FILTER_ACCESS_H

//  Cleanup and rewrite: Andras Varga, 2004

#include <omnetpp.h>
#include "ModuleAccess.h"
#include "BloomFilter.h"


/**
 * Gives access to the BloomFilter.
 */
class INET_API BloomFilterAccess : public ModuleAccess<BloomFilter>
{
    public:
    	BloomFilterAccess() : ModuleAccess<BloomFilter>("bloomFilter") {}
};

#endif

