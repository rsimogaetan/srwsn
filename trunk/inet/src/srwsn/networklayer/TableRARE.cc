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
	IDlocal=0;

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


// Une methode qui prend en entree l'ID dans le reseau et qui donne en sortie un ID logique (local)
int TableRARE::IDnettoIDlocal(int IDnet){

	// On alloue l espace a la nouvelle entree
	IDtable.resize(IDlocal+1);

	// On attribue un id local a l'id du reseau
	IDtable[IDlocal]=IDnet;

	// On passe a l id local libre suivant
	IDlocal++;

	return IDlocal-1;
}

void TableRARE::UpdateTable(int QueryId,int PeerId)
{
	// On incrémente la valeur
	table[QueryId][PeerId]++;
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
int TableRARE::LearningPeerSelection(int QueryId){

	int i=0, IdPeer=-1;
	int Max = table[QueryId][0];

	// On parcourt la table jusqu'à trouver le pair le plus pertinent
	// On choisit une comparaison stricte afin de choisir le minimum des id
	// dans le cas de choix multiples (le plus proche au niveau routage)
	while(i<maxPeers){
		if(table[QueryId][i] > Max){
		   Max = table[QueryId][i];
		   IdPeer = i;
		}
		i++;
	}

	// Dans le cas ou il n'y a pas de pairs pertinents
	// On cherche la requete précédente la plus proche de la requête courante
	// et on itère le processus
	if(IdPeer == -1) {
		int newQueryId;
		newQueryId = QueryRelaxation(QueryId);
		if(newQueryId!=QueryId){
		IdPeer = LearningPeerSelection(newQueryId);
		}
	}


	// Sinon on passe a la selection aleatoire
	if(IdPeer == -1) {
			IdPeer = (int)((double)rand() / ((double)RAND_MAX + 1) * 3);
			printf("--> On passe a la selection aleatoire \n");
		}

	return IdPeer;
}


void TableRARE::toString(){
	EV << "[TableRARE] Hello, I am ready ? " << ready
	<<" ; max peers :" << maxPeers << endl;

}
