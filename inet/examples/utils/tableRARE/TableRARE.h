#ifndef __TABLERARE_H
#define __TABLERARE_H

#include <vector>

using namespace std;

class TableRARE //:  public cSimpleModule
{
public:
	TableRARE(int Neighboor);
	~TableRARE();
	void toString();

	std::vector < std::vector<int> > table;
	void UpdateTable(int QueryId,int PeerId);
	int LearningPeerSelection(int QueryId);
    int QueryRelaxation(int QueryId);
	virtual void initialize();
private:
	int maxPeers;
	int maxQueries;
	int maxTypes;
	bool ready;
};

#endif
