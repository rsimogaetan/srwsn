#ifndef __BLOOMFILTER_H
#define __BLOOMFILTER_H


#include <cstdlib>
#include <omnetpp.h>

// The type of the sensor
enum SensoType {
    SENSOR_TEMPERATURE = 1,
    SENSOR_PRESSURE  = 2,
    SENSOR_SON  = 3,
    SENSOR_LIGTH  = 4,
    SENSOR_HUMIDITY = 5
};

typedef unsigned int (*hashfunc_t)(const char *);

class BloomFilter
{

public:

	BloomFilter(){};
	BloomFilter(size_t size, size_t nfuncs, ...);  // The Constructor
	~BloomFilter();                  // The Destructor
	void toString();				 // For information

private:
	bool ready;
	int maxEntry;
	size_t asize;
	char *a;
	size_t nfuncs;
	hashfunc_t *funcs;
public:
	virtual int Add(const char *s);
	virtual int Check(const char *s);

private:
	char * my_name;
};

#endif

