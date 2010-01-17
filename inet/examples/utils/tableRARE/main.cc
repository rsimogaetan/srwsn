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
	TableRARE * tr = new TableRARE();
	tr->toString();
	tr->increaseSimilarity(0,3);
	tr->toString();
	tr->increaseSimilarity(1,4);
	tr->toString();

	return EXIT_SUCCESS;
}
