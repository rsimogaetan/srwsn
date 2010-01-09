/*
 * The purpose of this file is to implement an object to manipulate ID.
 */

#include <iostream>
using namespace std;
#include <stdlib.h>
#include "IDBuilder.h"
#include <stdint.h>  // Use [u]intN_t if you need exactly N bits.

// Print the usage format and exit with an error.
void usage(char *arg0){
	cout << "Usage :  " << arg0 << "mac sink_distance area" <<endl;
	exit(-1);
}

// The function main
int main(int argc, char ** argv)
{
	if(argc != 4) usage(argv[0]);

	IDBuilder *idBuilder = new IDBuilder();
	idBuilder->setMACExt(argv[1]);
	idBuilder->toString();
	uint16_t sinkDistance = (uint16_t)atoi(argv[2]);
	idBuilder->setSinkDistance(sinkDistance);
	idBuilder->toString();
	uint16_t area = (uint16_t)atoi(argv[3]);
	idBuilder->setArea(area);
	idBuilder->toString();
	idBuilder->setID(72);
	idBuilder->toString();
	return EXIT_SUCCESS;
}
