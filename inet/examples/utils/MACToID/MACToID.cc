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
#include <stdint.h>  // Use [u]intN_t if you need exactly N bits. 
#include <vector>

uint8_t moi;

// Print the usage format and exit with an error.
void usage(char *arg0){
	cout << "Usage :  " << arg0 << "  MAC_Address(XX-XX-XX-XX-XX-XX)" <<endl;
	exit(-1);
}

uint64_t convertMACToID(string str);
int getLast2BytesToInt(string str);

// The function main
int main(int argc, char ** argv)
{
uint16_t moi = 65534;
cout << "moi : "<< moi <<endl;
moi = (moi << 1)+6;
cout << "moi : "<<moi<<endl;

/*
	if((argc != 2)) usage(argv[0]);


	string str = argv[1];

	convertMACToID(str);
	getLast2BytesToInt(str);
*/
	return EXIT_SUCCESS;
}

// Convert a MAC address into a unique ID
uint64_t convertMACToID(string str){
	string::iterator it;
	unsigned int ii, cutAt;
	vector<string> results;
	uint64_t ID = 0;

	string delim= "-";

	while( (cutAt = str.find_first_of(delim)) != str.npos )
	{
		if(cutAt > 0)
		{
			results.push_back(str.substr(0,cutAt));
		}
		str = str.substr(cutAt+1);
	}
	if(str.length() > 0)
	{
		results.push_back(str);
	}
	for(ii=0;ii<results.size();ii++){
		std::stringstream convertor;
		uint16_t dec_n = 0;
		std::string hex_n = results[ii];

		// Convertion Hexa (char*) to decimale (int)
		cout << "Hexadecimal : "<< hex_n;
		convertor << hex_n;
		convertor >> std::hex >> dec_n;
		cout << "  ; Decimal : "<< dec_n ;
		// If the decimal number = 0 while the hexadecimal one is not 00
		if((dec_n == 0) && (hex_n.compare("00")!=0))
			return -1;

		ID = (ID<=0)?dec_n:(ID<<8)+dec_n;
		cout << " ==> ID : "<< ID << endl;
	}
	return ID;
}


int getLast2BytesToInt(string str){
	std::stringstream convertor;
	uint16_t dec_n ;
	std::string hex_n;
	unsigned int cutAt;

	string delim= "-";

	while( (cutAt = str.find_first_of(delim)) != str.npos )
	{
		str = str.substr(cutAt+1);
	}
	if(str.length() > 0)
		hex_n = str;

	// Convertion Hexa (char*) to decimale (int)
	cout << "The last 2 bytes : Hexa="<<hex_n;
	convertor << hex_n;
	convertor >> std::hex >> dec_n;
	cout << "  ; Dec="<< dec_n << endl;

	return dec_n;
}
