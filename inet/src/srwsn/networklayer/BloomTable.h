#ifndef __BLOOMTABLE_H
#define __BLOOMTABLE_H


#include <cstdlib>
#include <omnetpp.h>
#include "BloomFilter.h"
#include "SRPacket_m.h"
#include "InterfaceEntry.h"

typedef unsigned int (*hashfunc_t)(const char *);

typedef std::map<int, std::string> QueryTranslation_t;


class BloomTable :  public cSimpleModule
{
public:

	BloomTable(){};

protected:
	void initialize();
	void toString();

public:
	virtual int GetIDlocal (MACAddress MAC);
	// This function has to be developed
	//virtual int Get(int QueryId){return 0;}
    virtual MACAddress Get(int QueryId, bool Source);
	virtual void AddFilter(BloomFilter* BloomNeighbor, MACAddress MAC);
	// Returne our own BloomFilter
	virtual BloomFilter GetBloomPerso(){return *this->BloomPerso;}
	// Add information to our own BloomFilter
	virtual int AddToBloomPerso(const char *info);
	// Print the ID of all the sensor in this BloomTable
	virtual void printFilters();
	// Update the display of the sensor, depending on its nature
	virtual void updateDisplay(std::string myIcon);
	// Select random type for this Bloom Filter (BloomPerso)
	virtual void selectRandomType();
private:
	BloomFilter * BloomPerso;
	std::vector <BloomFilter*> NeighborsTable;
	std::vector <std::string> QueryTranslation;
    QueryTranslation_t QueryTranslation2;
	std::map <int, MACAddress> IDtoMACtable;      // Conversion MAC, IDlocal
	int IDlocalmax;                               // Taille dynamique de MACtoIDtable
	int tailleFiltre;

};

#endif

