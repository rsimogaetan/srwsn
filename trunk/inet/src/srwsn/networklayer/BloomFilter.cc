#include <omnetpp.h>
#include <iostream>
#include <cstring>
#include "BloomFilter.h"
#include <cstdlib>
#include <climits>
#include <cstdarg>
#include <exception>
using namespace std;

#define SETBIT(a, n) (a[n/CHAR_BIT] |= (1<<(n%CHAR_BIT)))
#define GETBIT(a, n) (a[n/CHAR_BIT] & (1<<(n%CHAR_BIT)))


// The module class needs to be registered with OMNeT++
Define_Module(BloomFilter);

// Another Constructor
BloomFilter::BloomFilter(size_t size, size_t nfuncs, ...) {


	  va_list l;
	  unsigned int n;

	  try {
	    this->a=new char[(size+CHAR_BIT-1)/CHAR_BIT];
	  }
	  catch(const std::exception& e)
	  {
	    // Si on arrive ici c'est qu'il y a une erreur d'allocation.
	    // On doit donc libérer la mémoire.
	    delete(this);
	    //std::cerr << "ERREUR: " << e.what() << endl;
	    // On relance alors l'exception pour indiquer qu'une erreur est survenue.
	    throw;
	  }


	  try {
		  this->funcs= new hashfunc_t[nfuncs];
		  }
	  catch(const std::exception& e){
	    delete(this->a);
	    delete(this);
	      }

	  va_start(l, nfuncs);
	  for(n=0; n < nfuncs; ++n) {
	    this->funcs[n]=va_arg(l, hashfunc_t);
	  }
	  va_end(l);

	  this->nfuncs=nfuncs;
	  this->asize=size;

}

// The Destructor
BloomFilter::~BloomFilter() {
//	delete(this->a);
//	  delete(this->funcs);
//	  delete(this);
}

int BloomFilter::Add(const char *s)
{
  size_t n;

  for(n=0; n<this->nfuncs; ++n) {
    SETBIT(this->a, this->funcs[n](s)%this->asize);
  }

  return 0;
}

int BloomFilter::Check(const char *s)
{
  size_t n;

  for(n=0; n< this->nfuncs; ++n) {
    if(!(GETBIT(this->a, this->funcs[n](s)%this->asize))) return 0;
  }

  return 1;
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
