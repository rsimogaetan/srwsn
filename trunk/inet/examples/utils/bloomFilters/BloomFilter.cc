#include <iostream>
using namespace std;
#include <string.h>
#include "BloomFilter.h"

// The Constructor
BloomFilter::BloomFilter(const char *name) {
	my_name = strdup(name);
}

// The Destructor
BloomFilter::~BloomFilter() {
	delete my_name;
}

//Print information about this object
void BloomFilter::toString(){
	cout << " BloomFilter: Hello, my name is : " << my_name << endl;
}
