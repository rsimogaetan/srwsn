package inet.networklayer.manetrouting;

simple OLSR
{
    parameters:
        int Willingness = default(3); //ms
        int Hello_ival = default(2); // sec
        int Tc_ival = default(5); // sec
        int Mid_ival = default(5); // sec
        int use_mac = default(0);
        bool UseIndex = default(false); // use the interface index instead the ip to identify the interface EXPERIMENTAL
        string ExcludedInterfaces = default(""); // list of interfaces excluded by the manet routing protocol
        bool reduceFuncionality = default (false);
    gates:
        input from_ip;
        output to_ip;
}



