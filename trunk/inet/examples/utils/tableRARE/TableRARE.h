#ifndef __TABLERARE_H
#define __TABLERARE_H

<<<<<<< .mine
#include <omnetpp.h>
#include <vector>

using namespace std;

class TableRARE //:  public cSimpleModule
{
=======
#include <vector>

using namespace std;

class TableRARE //:  public cSimpleModule
{
>>>>>>> .r17
public:
	TableRARE(int Neighboor);
	~TableRARE();
	void toString();
<<<<<<< .mine



	int maxPeers;
	int maxQueries;
	int maxTypes;
	vector < vector<int> > table;
	void UpdateTable(int QueryId,int PeerId);
	int LearningPeerSelection(int QueryId);
    int QueryRelaxation(int QueryId);
	virtual void initialize();
	bool ready;

=======

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
>>>>>>> .r17
};

#endif
