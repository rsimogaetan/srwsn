//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "BloomTable.h"
#include "BloomFilter.h"
#include <omnetpp.h>
#include <cstring>

using namespace std;

Define_Module(BloomTable);


// Definition des fonctions de hashage *******************
unsigned int sax_hash(const char *key)
{
	unsigned int h=0;

	while(*key) h^=(h<<5)+(h>>2)+(unsigned char)*key++;

	return h;
}

unsigned int sdbm_hash(const char *key)
{
	unsigned int h=0;
	while(*key) h=(unsigned char)*key++ + (h<<6) + (h<<16) - h;
	return h;
}
//*********************************************************


BloomTable::BloomTable(int Neighbors) { // Le nombre de voisins DIRECT est a mettre en paramètre

	// On instancie son propre filtre de bloom
	BloomPerso = new BloomFilter((size_t) Neighbors, 2, sax_hash, sdbm_hash);

	// On instancie le tableau des filtres de blooms voisin
	NeighborsTable.resize(0);

	// On instancie le tableau de conversion d'ID (net<->local)
	TableID[0] = 0;
	IDlocalmax = 1;                // On commence à 1 car 0 représente le noeud lui même

	// On remplit le tableau de traduction de requete int QueryId <-> char * Query
	string temperature="TEMPERATURE", pression = "PRESSION", son = "SON", lumiere = "LUMIERE", humidite = "HUMIDITE", puit = "PUIT";
	QueryTranslation.push_back(temperature);
	QueryTranslation.push_back(pression);
	QueryTranslation.push_back(son);
	QueryTranslation.push_back(lumiere);
	QueryTranslation.push_back(humidite);
	QueryTranslation.push_back(puit);
}



void BloomTable::initialize()
{
	// TODO - Generated method body
}

// Conversion IDnet -> IDlocal
int BloomTable::GetIDlocal (int IDnet) {

	// On alloue une correspondance supplementaire
	TableID[IDlocalmax]=IDnet;

	IDlocalmax++;

	return IDlocalmax-1;
}

// Conversion IDlocal -> IDnet
int BloomTable::GetIDnet (int IDlocal) {

	return TableID[IDlocal];
}

// Fonction qui permet d'ajouter un filtre dans la table de Bloom
void BloomTable::AddFilter(BloomFilter* BloomNeighbor, int IDnet)
{
	// On ajoute le propietaire du filtre dans le tableau de correspondance
	int IDlocal = GetIDlocal(IDnet);

	// On alloue la mémoire pour stocker un nouveau filtre (pointeur)
	// Il faut noter que la taille de ce vecteur vaut IDlocalmax (un fitlre de bloom par id)
	NeighborsTable.resize(IDlocal+1);

	// On insere le nouveau filtre dans le tableau
	NeighborsTable[IDlocal] = BloomNeighbor;
}


// Fonction qui permet de retourner le noeud VOISIN DIRECT qui peut repondre a la requete
// Renvoie 0 si le capteur lui meme peut répondre à la requete
int BloomTable::Get(int QueryId)
{
	int PeerIdlocal=-1;

	// On transforme la requete en chaine de caractere
	const char * Query = QueryTranslation[QueryId].c_str();

	// On regarde d'abord si le capteur personnel peut répondre à la requête
	if (BloomPerso->Check(Query)) {
		PeerIdlocal = 0;   // J'ai la requete je peux y repondre
	}
	// Sinon
	else {
		// On parcourt le tableau et on cherche le premier pair qui peut repondre DIRECTEMENT a la requete
		for (unsigned int i=0; i<NeighborsTable.size();i++){
			if (NeighborsTable[i]->Check(Query))  {
				PeerIdlocal = i+1;     // En effet car le 0 représente le noeud lui même
				break;
			}
		}
	}

	int PeerIDnet = GetIDnet(PeerIdlocal);

	// On renvoie l'IDnet
	return PeerIDnet;
}

//Print information about this object
void BloomTable::toString(){
	/*
	EV << "[BloomTable] Hello, I am ready ? " << ready
	<<" ; max entry :" << IDlocalmax << endl;
	*/
}


