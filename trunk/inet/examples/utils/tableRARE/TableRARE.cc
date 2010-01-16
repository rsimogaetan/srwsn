
using namespace std;
#include <iostream>
#include <vector>
#include "TableRARE.h"
#include <stdlib.h>  // For rand
#include <time.h>  // For rand


// Paramètres
const int MAXQUERIES=6;  // Temperature, Pression, Son, Lumiere, Humidite, Puit
const int MAXTYPES=4;    // Relation math, Feu, Intru, Pluie

// On definit la table des requetes
int Queries[MAXQUERIES][MAXTYPES]={/*T*/{1,1,0,0},
								   /*P*/{1,0,0,0},
								   /*S*/{0,0,1,1},
								   /*L*/{0,1,1,0},
								   /*H*/{0,0,0,1},
								   /*P*/{0,0,0,0}}; //A definir


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

	srand(time(NULL));

}

TableRARE::~TableRARE() {
	//delete [] table;
}

void TableRARE::initialize()
{
	/*ready = par("ready");
	maxPeers = par("maxPeers");
*/
}

// Une methode qui prend en entree l'ID dans le reseau et qui donne en sortie un ID logique (local)
int TableRARE::IDnettoIDlocal(int IDnet)
{

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


/*
//On définit une méthode qui renvoit une liste de n pairs pertinents
int* TableRare::LearningPeerSelection(int QueryId, int n){

	int *IdPeers;

	if (n<maxEntry) {
		int i=1, IdPeer;
		int Max = table[QueryId][0];
		vector<int> temp;
		temp.resize(maxEntry);

		// On copie le tableau a l indice QueryId
		for(int j = 0; j < 0; j++) {
			temp.at(j)=table[QueryId][0];
		}


	}
	else
	{
		// On renvoit un tableau vide
		IdPeers = NULL;
	}



	return IdPeer;
}
*/


void TableRARE::toString(){
	/*EV << "[TableRARE] Hello, I am ready ? " << ready
	<<" ; max peers :" << maxPeers << endl;
	*/
}
