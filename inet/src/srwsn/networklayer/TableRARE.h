#ifndef __TABLERARE_H
#define __TABLERARE_H

#include <omnetpp.h>

class TableRARE :  public cSimpleModule
{
public:
	TableRARE();
	~TableRARE();
	void toString();
private:
	virtual void initialize();
	bool ready;
	int maxEntry;
};

#endif

