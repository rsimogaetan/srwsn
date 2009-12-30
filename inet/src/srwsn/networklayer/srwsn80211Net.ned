package inet.srwsn.networklayer;

// This is the file where we get to really implement our plateform
// We use a link layer wich already exists in INET

import inet.srwsn.networklayer.SR;

module srwsn80211Net
{
    parameters:
        bool proxyARP = default(true);
        @display("i=block/fork");
    gates:
        input ifIn[];
        output ifOut[];
    submodules:
        sr: SR {
            parameters:
                @display("p=85,95;q=queue");
            gates:
                queueIn[sizeof(ifIn)];
        }
    connections allowunconnected:

        // L2 interfaces to IP and from ARP
        for i=0..sizeof(ifOut)-1 {
            ifIn[i] --> { @display("m=s"); } --> sr.queueIn[i];
        }
}
