#include <omnetpp.h>
#include "BloomTable.h"
#include <cstring>


using namespace std;


// The module class needs to be registered with OMNeT++
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


// The Destructor
BloomTable::BloomTable(int Neighbors) {// Le nombre de voisins DIRECT est a mettre en paramètre

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

// The Destructor
BloomTable::~BloomTable() {
	// TODO - Generated method body
}

void BloomTable::initialize()
{
}

void BloomTable::handleMessage(cMessage *msg){};


// Conversion IDnet -> IDlocal
int BloomTable::GetIDlocal (int IDnet){

	// On alloue une correspondance supplementaire
	TableID[IDlocalmax]=IDnet;

	IDlocalmax++;

	return IDlocalmax-1;
}


// Conversion IDlocal -> IDnet
int BloomTable::GetIDnet (int IDlocal){
	return TableID[IDlocal];
}


// Fonction qui permet de retourner le noeud VOISIN DIRECT qui peut repondre a la requete
// Renvoie 0 si le capteur lui meme peut répondre à la requete
// La variable Source permet d'indiquer si le capteur est la source du message
int BloomTable::Get(int QueryId, bool Source)
{
	int PeerIdlocal=-1;

	// On transforme la requete en chaine de caractere
	const char * Query = QueryTranslation[QueryId].c_str();

	// On regarde d'abord si le capteur n'est pas la source du message
	if (!Source) {
		// Si le capteur peut y repondre directement
		if (BloomPerso->Check(Query)) {
			PeerIdlocal = 0;   // J'ai la reponse a la requete je peux y repondre
		}
		else {
				// On parcourt le tableau et on cherche le premier pair qui peut repondre DIRECTEMENT a la requete
				for (unsigned int i=0; i<NeighborsTable.size();i++){
					if (NeighborsTable[i]->Check(Query))  {
						PeerIdlocal = i+1;     // En effet car le 0 représente le noeud lui même
						break;
					}
				}
			}
	}

	// Sinon on est la source et dans ce cas il n'est pas nécessaire de verifier si on peut y repondre
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


// Fonction qui permet d'ajouter un filtre dans la table de Bloom
void BloomTable::AddFilter(BloomFilter* BloomNeighbor, int IDnet){
	// On ajoute le propietaire du filtre dans le tableau de correspondance
	int IDlocal = GetIDlocal(IDnet);

	// On alloue la mémoire pour stocker un nouveau filtre (pointeur)
	// Il faut noter que la taille de ce vecteur vaut IDlocalmax (un fitlre de bloom par id)
	NeighborsTable.resize(IDlocal+1);

	// On insere le nouveau filtre dans le tableau
	NeighborsTable[IDlocal] = BloomNeighbor;

}


//Print information about this object
void BloomTable::toString(){
	/*
	EV << "[BloomTable] Hello, I am ready ? " << ready
	<<" ; max entry :" << IDlocalmax << endl;
	*/
}
