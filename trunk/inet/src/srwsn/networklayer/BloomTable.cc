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


void BloomTable::initialize()
{
	EV <<"[BloomTable]::"<< __FUNCTION__ <<endl;
	tailleFiltre = par("tailleFiltre");

	// On instancie son propre filtre de bloom
	BloomPerso = new BloomFilter((size_t) tailleFiltre, 2, sax_hash, sdbm_hash);

	// On remplit le tableau de traduction de requete int QueryId <-> char * Query
	string temperature="TEMPERATURE", pression = "PRESSION", son = "SON", lumiere = "LUMIERE", humidite = "HUMIDITE", puit = "PUIT";
	QueryTranslation.push_back(temperature);
	QueryTranslation.push_back(pression);
	QueryTranslation.push_back(son);
	QueryTranslation.push_back(lumiere);
	QueryTranslation.push_back(humidite);
	QueryTranslation.push_back(puit);

	QueryTranslation2[SENSOR_HUMIDITY] = "HUMIDITY";
	QueryTranslation2[SENSOR_LIGTH] = "LIGTH";
	QueryTranslation2[SENSOR_PRESSURE] = "PRESSURE";
	QueryTranslation2[SENSOR_SONG] = "SONG";
	QueryTranslation2[SENSOR_TEMPERATURE] = "TEMPERATURE";
}

// Fonction qui permet de retourner le noeud VOISIN DIRECT qui peut repondre a la requete
// Renvoie 0 si le capteur lui meme peut répondre à la requete
// La variable Source permet d'indiquer si le capteur est la source du message
MACAddress BloomTable::GetNextHop(int QueryId, bool Source,MACAddress myMAC)
{
	EV <<"[BloomTable]::"<< __FUNCTION__ <<endl;
	MACAddress neighborMAC;
	BloomFilter neighborBloom;
	//NeighborsTable[neighborMAC] = neighborBloom;

	// On transforme la requete en chaine de caractere
	std::string query_str = (*QueryTranslation2.find(QueryId)).second;
	EV << "     Looking for a neighbor which does "<< query_str<<endl;

	// On regarde d'abord si je ne suis pas la source du message
	// Et si je peut y repondre directement
	if ( (!Source) && ((BloomPerso->Check(query_str)))) {
		EV << "     OK. I do "<< query_str << " I can respond" <<endl;
		// J'ai la reponse a la requete je peux y repondre
		return myMAC;
	}

	EV << "    Sorry I do not do "<< query_str << endl;
	// Sinon on est la source et dans ce cas il n'est pas nécessaire de verifier si on peut y repondre
	// On parcourt le tableau et on cherche le premier voisin DIRECT qui peut repondre à la requete
	NeighborsTable_t::iterator it;
	for (it = NeighborsTable.begin(); it != NeighborsTable.end (); ++it)
	{
		BloomFilter bloom = (*it).second;
		if(bloom.Check(query_str)){
			std::string mac_str= (*it).first;
			size_t size = mac_str.size() + 1;
			char * buffer = new char[ size ];
			strncpy( buffer, mac_str.c_str(), size );
			EV << "    But I know that "<< mac_str << " does " << query_str << endl;
			return (*(new MACAddress(buffer)));
		}
	}

	EV << "Sorry, I do not know anybody who does " << query_str << endl;
	// On renvoie un MAC null
	return MACAddress::UNSPECIFIED_ADDRESS;
}

// Fonction qui permet d'ajouter un filtre dans la table de Bloom
void BloomTable::AddFilter(BloomFilter neighborBloom, MACAddress neighborMAC){
	EV <<"[BloomTable]::"<< __FUNCTION__ <<endl;
	// On insere le nouveau filtre dans le tableau
	NeighborsTable[neighborMAC.str()] = neighborBloom;
}


int BloomTable::AddToBloomPerso(const char *info){
	EV <<"[BloomTable]::"<< __FUNCTION__ <<endl;
	return BloomPerso->Add(info);
}

// Print the ID of all the sensor in this BloomTable
void BloomTable::printFilters(){
	EV <<"[BloomTable]::"<< __FUNCTION__ <<endl;
	//TODO: This function should print the contain of the bloomfilter of all its neighbor
	EV << "[BloomTable] Neighbor MAC : ";
	NeighborsTable_t::iterator it;
	for (it = NeighborsTable.begin(); it != NeighborsTable.end (); ++it)
	{
		EV << ((*it).first) << ", ";
	}
	EV << endl;
}

// Update the display of the sensor, depending on its nature
void BloomTable::updateDisplay(std::string myIcon){
	EV <<"[BloomTable]::"<< __FUNCTION__ <<endl;
	// change icon displayed in Tkenv
	cDisplayString* display_string = &getParentModule()->getDisplayString();
	size_t size = myIcon.size() + 1;
	char * buffer = new char[ size ];
	strncpy( buffer, myIcon.c_str(), size );
	display_string->setTagArg("i", 0, buffer);
}

// Select random type for this Bloom Filter (BloomPerso)
void BloomTable::selectRandomType(){
	EV <<"[BloomTable]::"<< __FUNCTION__ <<endl;
	// Chose the type of the sensor
	//(int)QueryTranslation2.size()
	int param = intuniform(0,4);
	//int param = 4;
	QueryTranslation_t::iterator it;
	it =  QueryTranslation2.find(param);
	ASSERT(it != QueryTranslation2.end());

	std::string sensorType = (*it).second;
	BloomPerso->Add(sensorType);
	EV << " My sensor type is : " << sensorType << endl;

	if(sensorType.compare("HUMIDITY") == 0)
		updateDisplay("srwsn/sensor_humidity");
	if(sensorType.compare("LIGTH") == 0)
		updateDisplay("srwsn/sensor_ligth");
	if(sensorType.compare("PRESSURE") == 0)
		updateDisplay("srwsn/sensor_pressure");
	if(sensorType.compare("SONG") == 0)
		updateDisplay("srwsn/sensor_song");
	if(sensorType.compare("TEMPERATURE") == 0)
		updateDisplay("srwsn/sensor_temperature");

}
