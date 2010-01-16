#ifndef __TABLERARE_H
#define __TABLERARE_H

#include <vector>
#include <omnetpp.h>
#include"SR.h"
#include "SRPacket_m.h"
#include "InterfaceEntry.h"

class TableRARE :  public cSimpleModule
{
public:
	TableRARE(){};
	TableRARE(int Neighboor);
	~TableRARE();

protected:

	int maxPeers;
	int maxQueries;
	int maxTypes;
	bool ready;
	int IDlocalmax;
	std::vector<MACAddress> MACtoIdtable;

	std::vector < std::vector<int> > table;

private:
	virtual void initialize();
public:
	virtual void UpdateTable(int QueryId, MACAddress MAC);
	virtual MACAddress LearningPeerSelection(int QueryId);
	virtual int QueryRelaxation(int QueryId);
	virtual int MACtoIDlocal(MACAddress MAC);
	void toString();
};

#endif
