package inet.examples.srwsn;

// This is an implementation example of our Semantic Routing 
// application of Wireless Sensor Network platform 


import inet.srwsn.nodes.srwsn802154Host;
import inet.world.ChannelControl;
import inet.underTest2.underTest.nodes.wpan.Ieee802154Node;

network srwsn802154
{
    parameters:
        double numHosts;
        double playgroundSizeX;
        double playgroundSizeY;
    submodules:
        host[numHosts]: srwsn802154Host {
            parameters:
                @display("r=,,#707070;p=120,41");
        }
        channelcontrol: ChannelControl {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                @display("p=34,22");
        }
}
