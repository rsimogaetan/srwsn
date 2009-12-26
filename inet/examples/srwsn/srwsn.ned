package inet.examples.srwsn;

// This is an implementation example of our Semantic Routing 
// application of Wireless Sensor Network platform 

import inet.srwsn.nodes.srwsnHost;
import inet.world.ChannelControl;

network srwsn
{
    parameters:
        int numHosts;
        double playgroundSizeX;
        double playgroundSizeY;
    submodules:
        host[numHosts]: srwsnHost {
            parameters:
                @display("r=,,#707070");
        }
        channelcontrol: ChannelControl {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                @display("p=60,50");
        }
}
