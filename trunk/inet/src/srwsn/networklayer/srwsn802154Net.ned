package inet.srwsn.networklayer;

simple srwsn802154Net
{
    parameters:
        // debug switch
        bool debug; 				// debug switch
        bool isPANCoor;
    gates:
        input uppergateIn; 			// from the upper layer
        output uppergateOut; 		// to the upper layer
        input lowergateIn; 			// from the physical layer
        output lowergateOut; 			// to the physical layer
}