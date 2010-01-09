#ifndef __TABLERARE_H
#define __TABLERARE_H

#include <vector>
#include <omnetpp.h>

class TableRARE :  public cSimpleModule
{
public:
	TableRARE(){}
	TableRARE(int Neighboor);
	~TableRARE();
private:
	bool ready;
	int maxEntry;
	int maxPeers;
	int maxQueries;
	int maxTypes;
	std::vector < std::vector<int> > table;

private:
	virtual void initialize();
public:
	virtual void UpdateTable(int QueryId,int PeerId);
	virtual int LearningPeerSelection(int QueryId);
	virtual int QueryRelaxation(int QueryId);
	void toString();
};

#endif

