#ifndef __BLOOMFILTER_H
#define __BLOOMFILTER_H

#include<cstdlib>
#include<cstdio>

typedef unsigned int (*hashfunc_t)(const char *);

class BloomFilter {
public:
	BloomFilter(size_t size, size_t nfuncs, ...);  // The Constructor
	~BloomFilter();                  // The Destructor
	void toString();				 // For information
	void PrintFilter(int size);

	size_t asize;
	char *a;
	size_t nfuncs;
	hashfunc_t *funcs;
	int Add(const char *s);
	int Check(const char *s);

private:
	char * my_name;
};

#endif

