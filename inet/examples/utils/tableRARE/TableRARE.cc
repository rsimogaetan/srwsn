//#include <omnetpp.h>
#include "TableRARE.h"
#include <iostream>
#include <stdint.h>  // Use [u]intN_t if you need exactly N bits.
#include <assert.h>
using namespace std;


TableRARE::TableRARE(){
	ready = true;

	queriesSimilarity[0] = 5; //  0 1 0 1
	queriesSimilarity[1] = 6; //  0 1 1 0
	queriesSimilarity[2] = 8; //  1 0 0 0
	queriesSimilarity[5] = 0; //  0 0 0 0
	queriesSimilarity[3] = 3; //  0 0 1 1
	queriesSimilarity[4] = 13; // 1 1 0 1
}

// On definit la methode qui permet de trouver la requete la plus proche d une autre
int TableRARE::QueryRelaxation(int queryId) {
//	cout <<"[TableRARE]::"<< __FUNCTION__ <<endl;

	if(queriesSimilarity.find(queryId) == queriesSimilarity.end())
		return queryId;

	uint64_t myQueryIdNumber = (*queriesSimilarity.find(queryId)).second;
	uint64_t lastQueryNumber = 0;
	uint64_t lastQueryId = queryId;

	// On cherche la requête qui a le plus de types en communs avec la requete courante
	QueriesSimilarity_t::iterator it;
	for(it = queriesSimilarity.begin(); it !=  queriesSimilarity.end(); ++it){
		int anotherQueryId = (*it).first;
		uint16_t anotherQueryNumber = (*it).second;
		if(anotherQueryId == queryId) continue;

		// TODO : Tu compare les mauvaises valeurs
		if(bitSum(myQueryIdNumber & anotherQueryNumber ) > bitSum(myQueryIdNumber & lastQueryNumber)){
			lastQueryNumber = anotherQueryNumber;
			lastQueryId = (*it).first;
			cout << " " <<(*it).first << "=" << bitSum(myQueryIdNumber & anotherQueryNumber ) << " ,";
		}
	}

	return lastQueryId;
}

// Increase the similarity between 2 queries in the query similarity map
void TableRARE::increaseSimilarity(int queryId1, int queryId2){
	if((queriesSimilarity.find(queryId1) == queriesSimilarity.end())
			|| (queriesSimilarity.find(queryId2) == queriesSimilarity.end()))
		return;
	uint64_t Q1 = (*queriesSimilarity.find(queryId1)).second;
	uint64_t Q2 = (*queriesSimilarity.find(queryId2)).second;

	// Find the position there is no similarity on every body
	uint64_t pos = 0;
	QueriesSimilarity_t::iterator it;
	for(it = queriesSimilarity.begin(); it !=  queriesSimilarity.end(); ++it){
		pos |= (*it).second;
	}
	if(pos == 0) return;
	assert(pos < (pos + 1 ));  // Check if the position has already exceed 2exp64-1

	pos += 1;

	// Set the must significant bit to 1 on both Q1 and Q2
	Q1 |= pos;
	Q2 |= pos;
	// Put them back in the map
	queriesSimilarity[queryId1] = Q1;
	queriesSimilarity[queryId2] = Q2;
}

// Return the sum of bits in number
int TableRARE::bitSum(uint64_t number){
	int sum = 0;
	while(number != 0){
		sum += number % 2;
		number = number >> 1;
	}
	return sum;
}

void TableRARE::toString(){
	cout << "[TableRARE] Hello, I am ready ? " << ready << endl;

	QueriesSimilarity_t::iterator it;
	for(it = queriesSimilarity.begin(); it !=  queriesSimilarity.end(); ++it){
		cout << "" << (*it).first << "";
		cout << " is close to " << QueryRelaxation((*it).first)
		<< endl;
	}



}
