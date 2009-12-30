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
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <cmath> 

// Print the usage format and exit with an error.
void usage(char *arg0){
	cout << "Usage :  " << arg0 << "  MAC_Address(XX-XX-XX-XX-XX-XX)" <<endl;
	exit(-1);
}

long convertMACToID(const char * mac);
int getLastNumber();

// The function main
int main(int argc, char ** argv)
{
	if((argc != 2)) usage(argv[0]);

//	convertMACToID(argv[1]);
	cout << getLastNumber() << endl;

	return EXIT_SUCCESS;
}

// Convert a MAC address into a unique ID
long convertMACToID(const char * mac){
	char *str = strdup(mac);
	char delims[] = "-";
	char *hex_n = NULL;
	hex_n = strtok( str, delims );
	long ID = 0;

	// Loop on each term of the mac address
	while( hex_n != NULL ) {
		std::stringstream convertor;
		long dec_n = 0;

		// Convertion Hexa (char*) to decimale (int)
		cout << "Hexadecimal : "<<hex_n;
		convertor << hex_n;
		convertor >> std::hex >> dec_n;
		cout << "  ; Decimal : "<< dec_n ;

		// If the decimal number = 0 while the hexadecimal one is not 00
		if((dec_n == 0) && (strcmp(hex_n,"00")!=0))
			return -1;

		ID = (ID<=0)?dec_n:(ID<<8)+dec_n;
		cout << " ==> ID : "<< ID << endl;

		hex_n = strtok( NULL, delims );
	}
	return ID;
}

int getLastNumber(){

    string str = "26-12-ea-25-ae-fd";
    // Create a copy a buffer
    size_t size = str.size() + 1;
    char * buffer = new char[ size ];
    // copier la chaîne
    strncpy( buffer, str.c_str(), size );

	std::stringstream convertor;
	long dec_n = 0;
	char * hex_n = buffer+16;

	// Convertion Hexa (char*) to decimale (int)
	convertor << hex_n;
	convertor >> std::hex >> dec_n;
	
    // free memory
    delete [] buffer;

	return dec_n;
}
