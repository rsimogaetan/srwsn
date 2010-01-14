package inet.srwsn.nodes;

// This file describes a sensor (node) in our sensor network

import inet.srwsn.linklayer.srwsn80211Nic;
import inet.srwsn.networklayer.srwsn80211Net;

import inet.base.NotificationBoard;
import inet.linklayer.ieee80211.Ieee80211NicAdhoc;
import inet.mobility.BasicMobility;
import inet.networklayer.common.InterfaceTable;

import inet.srwsn.networklayer.TableRARE;
import inet.srwsn.networklayer.BloomTable;
module srwsn80211Host
{
    parameters:
        @node();
        string mobilityType = default("NullMobility");
        @display("i=device/pocketpc_s");
    gates:
        input radioIn @directIn;
    submodules:
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=60,70");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=60,150");
        }
        networkLayer: srwsn80211Net {
            parameters:
                @display("p=248,247;q=queue");
            gates:
                ifIn[1];
                ifOut[1];
        }
        wlan: Ieee80211NicAdhoc {
            parameters:
                @display("p=248,349;q=queue");
        }
        mobility: <mobilityType> like BasicMobility {
            parameters:
                @display("p=149,307");
        }
        tableRARE: TableRARE {
            @display("p=60,388");
        }
        bloomTable: BloomTable {
            @display("p=60,307");
        }
    connections allowunconnected:
        // connections to network outside
        radioIn --> wlan.radioIn;
        wlan.uppergateOut --> networkLayer.ifIn[0];
        wlan.uppergateIn <-- networkLayer.ifOut[0];
}

