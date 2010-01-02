#ifndef __BLOOMFILTER_H
#define __BLOOMFILTER_H

class BloomFilter {
public:
	BloomFilter(const char * name);  // The Constructor
	~BloomFilter();                  // The Destructor
	void toString();				 // For information
private:
	char * my_name;
};

#endif

