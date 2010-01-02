#ifndef __INET_TABLE_RARE_ACCESS_H
#define __INET_TABLE_RARE_ACCESS_H

//  Cleanup and rewrite: Andras Varga, 2004

#include <omnetpp.h>
#include "ModuleAccess.h"
#include "TableRARE.h"


/**
 * Gives access to the TableRARE.
 */
class INET_API TableRAREAccess : public ModuleAccess<TableRARE>
{
    public:
    	TableRAREAccess() : ModuleAccess<TableRARE>("tableRARE") {}
};

#endif

