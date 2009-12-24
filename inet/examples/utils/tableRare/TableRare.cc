#include <iostream>
using namespace std;
#include <string.h>
#include "TableRare.h"

TableRare::TableRare(const char *name) {
	my_name = strdup(name);
}

TableRare::~TableRare() {
	delete my_name;
}

void TableRare::toString(){
	cout << "TableRare: Hello, my name is : " << my_name << endl;
}
