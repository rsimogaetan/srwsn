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
#include <stdlib.h>
#include "../../../src/srwsn/networklayer/TableRare.h"

// Print the usage format and exit with an error.
void usage(char *arg0){
	cout << "Usage : " << arg0 << " name" <<endl;
	exit(-1);
}

// The function main
int main(int argc, char ** argv)
{
	if(argc != 2) usage(argv[0]);

	TableRare *TableR = new TableRare(argv[1]);
	TableR->toString();
	return EXIT_SUCCESS;
}
