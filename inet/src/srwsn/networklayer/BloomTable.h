#ifndef __BLOOMTABLE_H
#define __BLOOMTABLE_H


#include <cstdlib>
#include <omnetpp.h>
#include "BloomFilter.h"

typedef unsigned int (*hashfunc_t)(const char *);

class BloomTable :  public cSimpleModule
{
public:

	BloomTable(){};
	BloomTable(int Neighbors);
	~BloomTable();

private:
    virtual void initialize();
	   virtual void handleMessage(cMessage *msg);
public:
    void toString();
	    int GetIDlocal (int IDnet);
	    int GetIDnet (int IDlocal);
	    int Get(int QueryId);
	    void AddFilter(BloomFilter* BloomNeighbor, int IDnet);
private:
	    BloomFilter * BloomPerso;
	    std::vector <BloomFilter*> NeighborsTable;
	    std::vector <std::string> QueryTranslation;
	    std::map <int,int> TableID;
	    int IDlocalmax;                  // Taille dynamique de TableID

    int GetIDlocal (int IDnet);
    int GetIDnet (int IDlocal);
    int Get(int QueryId, bool Source);
    void AddFilter(BloomFilter* BloomNeighbor, int IDnet);

    BloomFilter * BloomPerso;
    std::vector <BloomFilter*> NeighborsTable;
    std::vector <std::string> QueryTranslation;
    std::map <int,int> TableID;
    int IDlocalmax;                  // Taille dynamique de TableID


};

#endif

