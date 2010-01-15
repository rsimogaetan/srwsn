//
// Copyright (C) 2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//


class noncobject MACAddress;
class noncobject BloomFilter;
enum SensoType;

cplusplus {{
#include "MACAddress.h"
#include <stdint.h>  // Use [u]intN_t if you need exactly N bits.
#include "BloomFilter.h"

// ARP header length for IPv4 (4-byte addresses) and 802 LANs (6-byte MAC addrs)
#define SR_HEADER_BYTES   28
}}



//
// The different types of queris possible
//
enum SRQuery
{
    Q_REQUEST = 1;      // Request.
    Q_REPLY = 2;        // Reply.
}

//
// The message type
//
enum SRMsg{
    MSG_DISCOVERY = 1;    // Messages for discovery
    MSG_NORMAL = 2;       // Message exchanged on normal state
    MSG_ALERT = 3;        // Exchanged message when in alert
       
}

// Fields of the SR Packet
packet SRPacket
{
    int msgType enum(SRMsg);
    int queryType enum(SRQuery);
	int queryId enum(SensoType);
    MACAddress srcMACAddress;
    MACAddress destMACAddress;
    uint16_t Id;
    bool amIAlertGenerator;
    uint16_t alertTimeStamp;
    BloomFilter bloom;
}


