#include <iostream>
using namespace std;
#include <string.h>
//#include <omnetpp.h>
#include "BloomFilter.h"

// The Constructor
BloomFilter::BloomFilter(const char *name) {
	my_name = strdup(name);
}

// The Destructor
BloomFilter::~BloomFilter() {
	delete my_name;
}

// The module class needs to be registered with OMNeT++
//Define_Module(BloomFilter);
//Register_Class(BloomFilter);

//Print information about this object
void BloomFilter::toString(){
	cout << " BloomFilter: Hello, my name is : " << my_name << endl;
}
