//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef __BLOOMTABLE_H__
#define __BLOOMTABLE_H__

#include <omnetpp.h>
#include "BloomTableAccess.h"
#include "BloomFilter.h"
#include <map>

class BloomTable : public cSimpleModule
{
public:
	BloomTable();
	BloomTable(int Neighbors);
	~BloomTable();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void toString();

    int GetIDlocal (int IDnet);
    int GetIDnet (int IDlocal);
    int Get(int QueryId);
    void AddFilter(BloomFilter* BloomNeighbor, int IDnet);

    BloomFilter * BloomPerso;
    std::vector <BloomFilter*> NeighborsTable;
    std::vector <std::string> QueryTranslation;
    std::map <int,int> TableID;
    int IDlocalmax;                  // Taille dynamique de TableID


};

#endif
