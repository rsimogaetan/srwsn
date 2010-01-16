#include <omnetpp.h>
#include "TableRARE.h"
#include <iostream>
#include <vector>
#include <stdlib.h>  // For rand
#include <time.h>  // For rand
using namespace std;


// The module class needs to be registered with OMNeT++
Define_Module(TableRARE);

// Paramètres
const int MAXQUERIES=6;  // Temperature, Pression, Son, Lumiere, Humidite, Puit
const int MAXTYPES=4;    // Relation math, Feu, Intru, Pluie

// On definit la table des requetes
int Queries[MAXQUERIES][MAXTYPES]={{1,1,0,0},
								   {1,0,0,0},
								   {0,0,1,1},
								   {0,1,1,0},
								   {0,0,0,1},
								   {0,0,0,0}}; //A definir


TableRARE::TableRARE(int Neighbors) {

	maxPeers=Neighbors;
	maxQueries=MAXQUERIES;
	maxTypes=MAXTYPES;

	// On definit la table d apprentissage
	table.resize(maxPeers);
	for(int i=0; i<maxQueries; i++){
		table[i].resize(maxQueries);
		for(int j=0; j<maxPeers; j++){
			table[i][j]=0;
			}
	}

	// On initialise l'id à 0
	IDlocalmax=0;

	srand(time(NULL));

}

TableRARE::~TableRARE() {
	//delete [] table;
}

void TableRARE::initialize()
{
/*	ready = par("ready");
	maxPeers = par("maxPeers");*/
}


// Une methode qui prend en entree la MAC et qui donne en sortie un ID logique (local)
int TableRARE::MACtoIDlocal(MACAddress MAC){

	// On attribue un id local a la MAC
	MACtoIdtable[IDlocalmax]=MAC;

	// On passe a l id local libre suivant
	IDlocalmax++;

	return IDlocalmax-1;
}



void TableRARE::UpdateTable(int QueryId,MACAddress MAC)
{
	// On convertit la MACAddress en Idlocal
	vector<MACAddress>::iterator IDlocal;
	IDlocal = find(MACtoIdtable.begin(), MACtoIdtable.end(), MAC);

	// On incrémente la valeur
	table[QueryId][*IDlocal]++;
}


// On definit la methode qui permet de trouver la requete la plus proche d une autre
int TableRARE::QueryRelaxation(int QueryId) {
	int newQueryId;
	int Somme, Max = -1;

	// On cherche la requête qui a le plus de types en communs avec la requete courante
	for(int i=0; i<maxQueries; i++) {
		for(int j=0; j<maxTypes; j++) {
			Somme = Queries[QueryId][j]*Queries[i][j];
			if(Max < Somme) {
				newQueryId = j;
			}
		}
	}
	return newQueryId;
}


//On définit tout d'abord une méthode qui renvoit un pair pertinent
MACAddress TableRARE::LearningPeerSelection(int QueryId){

	int i=0, IdPeer;
	MACAddress MACPeer=NULL;
	int Max = table[QueryId][0];

	// On parcourt la table jusqu'à trouver le pair le plus pertinent
	// On choisit une comparaison stricte afin de choisir le minimum des id
	// dans le cas de choix multiples (le plus proche au niveau routage)
	while(i<maxPeers){
		if(table[QueryId][i] > Max){
		   Max = table[QueryId][i];
		   MACPeer = MACtoIdtable[i];
		}
		i++;
	}

	// Dans le cas ou il n'y a pas de pairs pertinents
	// On cherche la requete précédente la plus proche de la requête courante
	// et on itère le processus
	if(MACPeer == NULL) {
		int newQueryId;
		newQueryId = QueryRelaxation(QueryId);
		if(newQueryId!=QueryId){
		MACPeer = LearningPeerSelection(newQueryId);
		}
	}


	// Sinon on passe a la selection aleatoire
	if(IdPeer == -1) {
			IdPeer = (int)((double)rand() / ((double)RAND_MAX + 1) * 3);
			printf("--> On passe a la selection aleatoire \n");
		}

	return MACPeer;
}


void TableRARE::toString(){
	EV << "[TableRARE] Hello, I am ready ? " << ready
	<<" ; max peers :" << maxPeers << endl;

}

