using namespace std;
#include <iostream>
#include "IDBuilder.h"
#include <stdint.h>  // Use [u]intN_t if you need exactly N bits.
#include <sstream>  // For std::stringstream
#include <assert.h>  // For assert

// The Constructor
IDBuilder::IDBuilder() {
}
  // Another Constructor
IDBuilder::IDBuilder(uint16_t id){
	assert(id < 65535);  // 16 bits
	my_id = id;
}

// The Destructor
IDBuilder::~IDBuilder() {
}

//Print information about this object
void IDBuilder::toString(){
	std::cout << "[IDBuilder]::" << __FUNCTION__ << endl;
	std::cout << "     ID=" << my_id << " => mac ext=" << getMACExt()
		<< ", sinkDistance=" << getSinkDistance() << ", area=" << getArea() << endl;
}

void IDBuilder::setMACExt(std::string mac){
	uint16_t dec_n = getLastMACNumberToInt(mac);
	assert(dec_n < 1024); // 10 bits
	my_id &= 63; // Erase all the 10 last bits : 0000 0000 00xx xxxx
	my_id += (dec_n<<6);
}

void IDBuilder::setSinkDistance(uint16_t sinkDistance){
	assert(sinkDistance < 16);  // 4 bits
	my_id &= 65475; // Erase all the bits in the middle : xxxx xxxx xx00 00xx
	my_id += (sinkDistance<<2);
}

void IDBuilder::setArea(uint16_t area){
	assert(area < 4); // 2 bits
	my_id &= 65532; // Erase all the bits at the end : xxxx xxxx xxxx xx00
	my_id += area;
}

// Return the last number of the MAC Address in hexa (in a string)
// i.e. 00-12-EF-32-AA-AE  will return AE
uint16_t IDBuilder::getLastMACNumberToInt(std::string mac)
{
	std::stringstream convertor;
	uint16_t dec_n ;
	unsigned int cutAt;
	std::string delim= "-";

	while( (cutAt = mac.find_first_of(delim)) != mac.npos )	{
		mac = mac.substr(cutAt+1);
	}

	// Convert hexa to decimal
	convertor << mac;
	convertor >> std::hex >> dec_n;

	return dec_n;
}
