/*
 * The purpose of this file is to implement the real case usage of the Bloom Filter.
 * It must test all the features of the BloomFilter class :
 * It would be nice if it could take as argument an element (request), and return another element (response).
 * Something like :
 *  Command : ./bloomFilter temperature
 *  Result  :  Sensor id which sense temperature are : 2810, 98237, 123
 */

#include <iostream>
using namespace std;
#include <stdlib.h>
#include "BloomFilter.h"

// Print the usage format and exit with an error.
void usage(char *arg0){
	cout << "Usage :  " << arg0 << " name" <<endl;
	exit(-1);
}

// The function main
int main(int argc, char ** argv)
{
	if(argc != 2) usage(argv[0]);

	BloomFilter *BloomF = new BloomFilter(argv[1]);
	BloomF->toString();
	return EXIT_SUCCESS;
}
