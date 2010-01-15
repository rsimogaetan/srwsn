#ifndef __BLOOMTABLE_H
#define __BLOOMTABLE_H


#include <cstdlib>
#include <omnetpp.h>
#include "BloomFilter.h"

typedef unsigned int (*hashfunc_t)(const char *);

typedef std::map<int, std::string> QueryTranslation_t;

class BloomTable :  public cSimpleModule
{
public:

	BloomTable(){};
	~BloomTable();

protected:
	void initialize();
	void toString();

private:
/*
    int GetIDlocal (int IDnet);
    int GetIDnet (int IDlocal);
    void AddFilter(BloomFilter* BloomNeighbor, int IDnet);
    // Returne our own BloomFilter
    virtual BloomFilter* GetBloomPerso(){return this->BloomPerso;}
    // Add information to our own BloomFilter

    BloomFilter * BloomPerso;
    std::vector <BloomFilter*> NeighborsTable;
    std::vector <std::string> QueryTranslation;
    std::map <int,int> TableID;
    int IDlocalmax;                  // Taille dynamique de TableID
*/

    int tailleFiltre;

};

#endif

