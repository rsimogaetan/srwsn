/*
 * The purpose of this file is to implement the real case usage of the RARE Table.
 * It must test all the features of the TableRare class :
 * It would be nice if it could take as argument an element (request), and return another element (response).
 * Something like :
 *  Command : ./table sink
 *  Result  :  Sensor id which goes to the sink are : 876, 1931
 */

#include <iostream>
using namespace std;
#include <cstdlib>
#include "TableRARE.h"

// Print the usage format and exit with an error.
void usage(char *arg0){
	cout << "Usage : " << arg0 << " maxNeighbor" <<endl;
	exit(-1);
}

// The function main
int main(int argc, char ** argv)
{
	if(argc != 2) usage(argv[0]);


	int maxNeighbor=3;// *argv[1];

	//printf("max : %c argv %c\n", maxNeighbor, *argv[1]);

	TableRARE *TableR = new TableRARE(maxNeighbor);
	//TableR->toString();

	// On recoit une reponse a la requete QueryId (requete 1) par le pair PeerId
	int QueryId=1, PeerId=1;
	// On met donc a jour la table
	TableR->UpdateTable(QueryId, PeerId);
	printf("\nOn recoit une reponse venant du pair %d\n", PeerId);

	// On recoit la requete 1 on desire la transmettre a PeerIdDest
	int QueryIdReceived = 1;
	int PeerIdDest = TableR->LearningPeerSelection(QueryIdReceived); // Pair 1

	printf("Pour la requete %d le pair pertinent est le pair %d\n\n", QueryIdReceived, PeerIdDest);

	// On recoit deux memes reponses a la requete 1 mais cette fois par le pair 2
	PeerId = 2;
	// On met donc a jour la table deux fois
	TableR->UpdateTable(QueryId, PeerId);
	printf("On recoit une reponse venant du pair %d\n", PeerId);

	TableR->UpdateTable(QueryId, PeerId);
	printf("On recoit une reponse venant du pair %d\n", PeerId);


	// On recoit la requete 1 on desire la transmettre a PeerIdDest
	QueryIdReceived = 1;
	PeerIdDest = TableR->LearningPeerSelection(QueryIdReceived);  // Pair 2

	printf("Pour la requete %d le pair pertinent est le pair %d\n\n", QueryIdReceived, PeerIdDest);

	// On recoit une requete qui n'a pas ete traite
	QueryIdReceived = 0;
	PeerIdDest = TableR->LearningPeerSelection(QueryIdReceived);  // Pair aleatoire
	printf("Pour la requete %d le pair pertinent est le pair %d\n\n", QueryIdReceived, PeerIdDest);


=======
//	int maxNeighbor=atoi(argv[1]);
	int maxNeighbor=3;// *argv[1];

	//printf("max : %c argv %c\n", maxNeighbor, *argv[1]);

	TableRARE *TableR = new TableRARE(maxNeighbor);
	//TableR->toString();

	// On recoit une reponse a la requete QueryId (requete 1) par le pair PeerId
	int QueryId=1, PeerId=1;
	// On met donc a jour la table
	TableR->UpdateTable(QueryId, PeerId);
	cout <<"\nOn recoit une reponse venant du pair "<< PeerId << "\n";

	// On recoit la requete 1 on desire la transmettre a PeerIdDest
	int QueryIdReceived = 1;
	int PeerIdDest = TableR->LearningPeerSelection(QueryIdReceived); // Pair 1

	cout << "Pour la requete "<< QueryIdReceived <<" le pair pertinent est le pair "<< PeerIdDest<<"\n\n";

	// On recoit deux memes reponses a la requete 1 mais cette fois par le pair 2
	PeerId = 2;
	// On met donc a jour la table deux fois
	TableR->UpdateTable(QueryId, PeerId);
	cout <<  "On recoit une reponse venant du pair "<< PeerId << endl;

	TableR->UpdateTable(QueryId, PeerId);
	cout << "On recoit une reponse venant du pair "<< PeerId << endl;


	// On recoit la requete 1 on desire la transmettre a PeerIdDest
	QueryIdReceived = 1;
	PeerIdDest = TableR->LearningPeerSelection(QueryIdReceived);  // Pair 2

	cout << "Pour la requete "<< QueryIdReceived <<" le pair pertinent est le pair "<< PeerIdDest <<"\n\n";

	// On recoit une requete qui n'a pas ete traite
	QueryIdReceived = 0;
	PeerIdDest = TableR->LearningPeerSelection(QueryIdReceived);  // Pair aleatoire
	cout << "Pour la requete "<< QueryIdReceived << " le pair pertinent est le pair "<< PeerIdDest <<"\n\n";


>>>>>>> .r17
	return EXIT_SUCCESS;
}
