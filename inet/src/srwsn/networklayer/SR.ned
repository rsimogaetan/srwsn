package inet.srwsn.networklayer;

simple SR
{
    parameters:
        double procDelay @unit("s") = default(0s);
        bool isSink = default(false);
        @display("i=block/routing");
    gates:
        input queueIn[];
        output queueOut;
}
