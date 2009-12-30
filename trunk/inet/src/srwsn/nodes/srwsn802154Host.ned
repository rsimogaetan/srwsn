package inet.srwsn.nodes;

import inet.srwsn.networklayer.srwsn802154Net;

import inet.base.NotificationBoard;
import inet.mobility.BasicMobility;
import inet.networklayer.common.InterfaceTable;

import inet.underTest2.underTest.linklayer.ieee802154.Ieee802154Nic;
import inet.srwsn.linklayer.srwsn802154Nic;

import inet.underTest2.underTest.wpan.battery.Ieee802154Battery;
import inet.underTest2.underTest.wpan.Isabel.Base.Display;

module srwsn802154Host
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
        net: srwsn802154Net {
            parameters:
                @display("p=248,247;q=queue");
        }
        nic: srwsn802154Nic {
            parameters:
                @display("p=248,349;q=queue");
        }
        mobility: <mobilityType> like BasicMobility {
            parameters:
                @display("p=149,307");
        }
    connections allowunconnected:
        // connections to network outside
        radioIn --> nic.radioIn;
        nic.uppergateOut --> net.lowergateIn;
        nic.uppergateIn <-- net.lowergateOut;

}
