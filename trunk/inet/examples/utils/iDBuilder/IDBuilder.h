#include <iostream>
#include <stdint.h>  // Use [u]intN_t if you need exactly N bits.
#include <sstream>  // For std::stringstream
#include <assert.h>  // For assert
#ifndef __IDBUILDER_H
#define __IDBUILDER_H

// This
class IDBuilder {
public:
	IDBuilder();  // The Constructor
	IDBuilder(uint16_t id);  // Another Constructor
	~IDBuilder();            // The Destructor
	void toString();		 // For information
private:
	uint16_t my_id;		   // The ID, on 16 bits
	// The extention mac address is on 10 bits
	// The distance to the sink is on 4 bits
	// The logic area around the sink is on 2 bits
	uint16_t getLastMACNumberToInt(std::string mac);   // Return the last 2 bytes of the mac addresse in hexa
public:
	void setID(uint16_t id){my_id = id;}
	void setMACExt(std::string mac);
	void setSinkDistance(uint16_t sinkDistance);
	void setArea(uint16_t area);
	uint16_t getID() {return my_id;}
	uint16_t getMACExt(){return (my_id & 65472)>>6;} // Erase all but the mac extended bits : xxxx xxxx xx00 0000
	uint16_t getSinkDistance(){return ((my_id & 60)>>2);} // Erase all but the sinkDistance bits : 0000 0000 00xx xx00
	uint16_t getArea(){return my_id & 3;}   // Erase all but the area bits : 0000 0000 0000 00xx
};

#endif

