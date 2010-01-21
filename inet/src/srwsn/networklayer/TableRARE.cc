#include <omnetpp.h>
#include "TableRARE.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>  // For rand
#include <time.h>  // For rand
#include <stdint.h>  // Use [u]intN_t if you need exactly N bits.
using namespace std;


// The module class needs to be registered with OMNeT++
Define_Module(TableRARE);

// Paramètres
//TODO : mettre ceci dans le constructeur
//const int MAXQUERIES=6;  // Temperature, Pression, Son, Lumiere, Humidite, Puit
//const int MAXTYPES=4;    // Relation math, Feu, Intru, Pluie
/*
// On definit la table des requetes
int Queries[MAXQUERIES][MAXTYPES]={{1,1,0,0},
								   {1,0,0,0},
								   {0,0,1,1},
								   {0,1,1,0},
								   {0,0,0,1},
								   {0,0,0,0}}; //A definir
								   */


void TableRARE::initialize()
{
	EV <<"[TableRARE]::"<< __FUNCTION__ <<endl;
	// Paramètres
	QueryRelaxationMaxRetry = par("queryRelaxationMaxRetry");
	ready = par("ready");
	maxQueries= par("maxQueries");
	maxTypes= par("maxTypes");

	// We hve not done any relaxation yet
	QueryRelaxationRetryTime = 0;

	queriesSimilarity[SENSOR_HUMIDITY] = 1; //  0 0 0 1
	queriesSimilarity[SENSOR_LIGTH] = 6; // 0 1 1 0
	queriesSimilarity[SENSOR_PRESSURE] = 8; // 1 0 0 0
	queriesSimilarity[SENSOR_SINK] = 0; // 0 0 0 0
	queriesSimilarity[SENSOR_SONG] = 3; // 0 0 1 1
	queriesSimilarity[SENSOR_TEMPERATURE] = 13; // 1 1 0 1

	srand(time(NULL));
}

void TableRARE::UpdateTable(int QueryId,MACAddress MAC)
{
	EV <<"[TableRARE]::"<< __FUNCTION__ <<endl;
	// If this query id does not exists yet
	// We must create one row on the table and set this value to 1
	if(learningTable.size() <=  (unsigned int)QueryId){
		std::map<std::string,int> newQuery;
		newQuery[MAC.str()] = 1;
		learningTable.resize(QueryId+1);
		learningTable[QueryId] = newQuery;
		EV << "     Our neighbors " << MAC.str() << " has responded positivily to us for the first time. At him fo query " << QueryId
		<< ". He is the first one for this  request " << QueryId << endl;
	}
	// The query already exists
	// We must increase the value in the table
	else{
		std::map<std::string,int> mapMACInt = learningTable[QueryId];
		std::map<std::string,int>::iterator it2;
		it2 = mapMACInt.find(MAC.str());
		// If this peer is already associated to this query
		if(it2 != (mapMACInt.end())){
			mapMACInt[MAC.str()] += 1;
			learningTable[QueryId] = mapMACInt;
			EV << "     "<<MAC.str() << " is a good neighbor, He has respond to " << (*it2).second + 1
			<< "The last time was about " << QueryId << endl;
		}else { // This peer is not associated to this query yet
			// We set it
			mapMACInt[MAC.str()] = 1;
			learningTable[QueryId] = mapMACInt;
			EV << "     Our neighbor " << MAC.str() << " has responded positivily to us for the first time. For query " << QueryId
			<< " Along with " << mapMACInt.size()-1 << " other neighbors" << endl;
		}
	}
}


//On définit tout d'abord une méthode qui renvoit un pair pertinent
MACAddress TableRARE::LearningPeerSelection(int QueryId){
	EV <<"[TableRARE]::"<< __FUNCTION__ <<endl;
	int Max = 0;
	std::string mac_str = "";


	EV << "     Looking for a neighbor which can lead me to someone which does "<< QueryId <<endl;

	// On parcourt la table jusqu'à trouver le pair le plus pertinent
	// On choisit une comparaison stricte afin de choisir le minimum des id
	// dans le cas de choix multiples (le plus proche au niveau routage)
	if(learningTable.size() >  (unsigned int)QueryId){
		std::map<std::string,int> mapMACInt = learningTable[QueryId];
		std::map<std::string,int>::iterator it;
		for (it = mapMACInt.begin (); it != mapMACInt.end (); ++it)
		{
			if((*it).second > Max){
				Max = (*it).second;
				mac_str = (*it).first;
			}
		}
		// Si on a trouvé une mac qui convient
		if(mac_str.compare("")!=0){
			EV << "   OK I think " << mac_str << " may have the response to our request." <<endl;
			size_t size = mac_str.size() + 1;
			char * buffer = new char[ size ];
			strncpy( buffer, mac_str.c_str(), size );
			return (*(new MACAddress(buffer)));
		}
	}

	// Dans le cas ou il n'y a pas de pairs pertinents
	// On cherche la requete précédente la plus proche de la requête courante
	// et on itère le processus
	// TODO: Le mécanisme d'extention de requête devrai retourner queries ID par ressemblance décroissantes.
	int newQueryId;
	newQueryId = QueryRelaxation(QueryId);
	if((newQueryId!=QueryId)&&((QueryRelaxationRetryTime < QueryRelaxationMaxRetry))){
		EV << "    I do not know anyone for sure. Nevertheless, I will expend the request to have more luck." << endl;
		QueryRelaxationRetryTime++;
		return LearningPeerSelection(newQueryId);
	}
	QueryRelaxationRetryTime = 0;

	EV <<"   Not very lucky. After "<<QueryRelaxationMaxRetry<< " query expansions, I still can't find somebody. Nevertheless, I will randomly proposed somebody." << endl;
	// Sinon on passe a la selection aleatoire
	if(learningTable.size() > 0){
		int randomQuery = (int) (rand() % (learningTable.size()-1)) ;
		// On boucle tant que la ligne de requete ne contient pas de peer ( on vas sortir un jour car learningTable.size > 0)
		while(learningTable[randomQuery].size() == 0){
			randomQuery = (int) (rand() % (learningTable.size()-1)) ;
		}
		return LearningPeerSelection(randomQuery);
	}

	EV << "    Sorry, I cannot help you. You are going to have to pick a neighbor yourself." <<endl;
	// On renvoie un MAC null
	return MACAddress::UNSPECIFIED_ADDRESS;
}

// On definit la methode qui permet de trouver la requete la plus proche d une autre
int TableRARE::QueryRelaxation(int queryId) {
	EV <<"[TableRARE]::"<< __FUNCTION__ <<endl;

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
	ASSERT(pos < (pos + 1 ));  // Check if the position has already exceed 2exp64-1

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
	EV << "[TableRARE] Hello, I am ready ? " << ready
	<<endl;

}
