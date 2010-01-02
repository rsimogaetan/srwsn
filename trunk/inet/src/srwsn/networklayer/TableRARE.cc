#include <omnetpp.h>
#include "TableRARE.h"

// The module class needs to be registered with OMNeT++
Define_Module(TableRARE);

TableRARE::TableRARE() {
}

TableRARE::~TableRARE() {
}

void TableRARE::initialize()
{
	ready = par("ready");
	maxEntry = par("maxEntry");
}

void TableRARE::toString(){
	EV << "[TableRARE] Hello, I am ready ? " << ready
	<<" ; max entry :" << maxEntry << endl;
}

