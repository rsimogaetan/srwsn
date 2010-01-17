#ifndef __TABLERARE_H
#define __TABLERARE_H

#include <cstdlib>
#include <omnetpp.h>
#include "SRPacket_m.h"
#include "InterfaceEntry.h"

typedef unsigned int (*hashfunc_t)(const char *);

typedef std::map<int, std::string> QueryTranslation_t;
typedef std::map<int, std::map<std::string,int> > LearningTable_t;
typedef std::map<int, uint64_t> QueriesSimilarity_t;

class TableRARE :  public cSimpleModule
{
public:
	TableRARE(){};
protected:

	int maxQueries;
	int maxTypes;
	bool ready;
	// L'ensemble des requêtes
	//int **Queries;

	// The maximum number of time we will try to 'expend the query'
	int QueryRelaxationMaxRetry;
	// The number of retry already done
	int QueryRelaxationRetryTime;
	// The table which learn from the network
	std::vector< std::map<std::string,int> > learningTable;
	// The vector wich helps us identify resemblance between queries
	QueriesSimilarity_t queriesSimilarity;
private:
	virtual void initialize();

public:
	virtual void UpdateTable(int QueryId, MACAddress MAC);
	virtual MACAddress LearningPeerSelection(int QueryId);
	virtual int QueryRelaxation(int QueryId);
	void toString();
	// Increase the similarity between 2 queries in the query similarity map
	virtual void increaseSimilarity(int queryId1, int queryId2);
};
#endif
