//
// Author: Thomas Reschka
//

package inet.examples.emulation.extserver;

import inet.nodes.inet.ExtRouter;
import inet.nodes.inet.StandardHost;
import ned.DatarateChannel;

network extServer {
    parameters:
		@display("bgb=400,200");
    submodules:
        router1: ExtRouter {
            parameters:
                numExtInterfaces = 1;
                @display("p=200,100;i=abstract/router");
            gates:
                pppg[2];
        }
        client1: StandardHost {
            parameters:
                @display("p=50,100");
            gates:
                pppg[1];
        }
        server1: StandardHost {
            parameters:
                @display("p=350,100;i=device/server");
            gates:
                pppg[1];
        }
    connections allowunconnected:
        router1.pppg[0] <--> NormalPath <--> client1.pppg[0];
        router1.pppg[1] <--> NormalPath <--> server1.pppg[0];
}

channel NormalPath extends DatarateChannel {
    parameters:
        datarate = 100Mbps;
        delay = 0.0565us; // 10m
		per = 0;
        ber = 0;
}
