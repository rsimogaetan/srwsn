/*
 * The purpose of this file is to implement the real case usage of the Bloom Filter.
 * It must test all the features of the BloomFilter class :
 * It would be nice if it could take as argument an element (request), and return another element (response).
 * Something like :
 *  Command : ./bloomFilter temperature
 *  Result  :  Sensor id which sense temperature are : 2810, 98237, 123
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "BloomFilter.h"
using namespace std;

// Print the usage format and exit with an error.
void usage(char *arg0){
	cout << "Usage :  " << arg0 << " name" <<endl;
	exit(-1);
}

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


// The function main
int main(int argc, char ** argv)
{
	if(argc != 2) usage(argv[0]);

	FILE *fp;
	char line[1024];
	char *p;
	char *name = argv[1];
	unsigned int size = 12;
	unsigned int nfuncs = 2;

	BloomFilter *BloomF = new BloomFilter(size, nfuncs, sax_hash, sdbm_hash);

	// On verifie que le fichier en argument s ouvre
	if(!(fp=fopen(name, "r"))) {
		printf("ERROR: Could not open file %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	// On rajoute dans le filtre de Bloom les mots du fichier
	while(fgets(line, 1024, fp)) {

		if((p=strchr(line, '\r'))) *p='\0';
		if((p=strchr(line, '\n'))) *p='\0';

		fprintf(stderr,"Mot Entre dans la table %s\n", line);

		BloomF->Add(line);
	}

	// On choisit le mot sur lequel on veut verifier sa presence dans la table
	fprintf(stderr, "Choisir un mot :\n");

	// On verifie sa presence
	while(fgets(line, 1024, stdin)) {

		if((p=strchr(line, '\r'))) *p='\0';
		if((p=strchr(line, '\n'))) *p='\0';

		p=strtok(line, " \t,.;:\r\n?!-/()");
		while(p) {
			if(BloomF->Check(p)) {
				fprintf(stderr, "Le mot \"%s\" est bien present :\n", p);
			}
			else {
				printf("No match for ford \"%s\"\n", p);
			}
			p=strtok(NULL, " \t,.;:\r\n?!-/()");
		}

		fprintf(stderr, "Choisir un mot :\n");
	}

	BloomF->~BloomFilter();

	return EXIT_SUCCESS;
}
