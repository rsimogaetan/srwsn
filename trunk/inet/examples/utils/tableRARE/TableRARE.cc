#include <iostream>
using namespace std;
#include <string.h>
#include "TableRARE.h"

TableRARE::TableRARE(const char *name) {
	my_name = strdup(name);
}

TableRARE::~TableRARE() {
	delete my_name;
}

void TableRARE::toString(){
	cout << "TableRARE: Hello, my name is : " << my_name << endl;
}

