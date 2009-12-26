#include <iostream>
using namespace std;
#include <string.h>
//#include <omnetpp.h>
#include "TableRare.h"

TableRare::TableRare(const char *name) {
	my_name = strdup(name);
}

TableRare::~TableRare() {
	delete my_name;
}

// The module class needs to be registered with OMNeT++
//Define_Module(TableRare);
//Register_Class(TableRare);

void TableRare::toString(){
	cout << "TableRare: Hello, my name is : " << my_name << endl;
}

