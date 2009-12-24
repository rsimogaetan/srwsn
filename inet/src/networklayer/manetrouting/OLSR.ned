package inet.networklayer.manetrouting;
import inet.networklayer.manetrouting.BaseRouting;
simple OLSR extends BaseRouting
{
    parameters:
        @class(OLSR);
        int Willingness = default(3); //ms
        int Hello_ival = default(2); // sec
        int Tc_ival = default(5); // sec
        int Mid_ival = default(5); // sec
        int use_mac = default(0);
        bool UseIndex = default(false); // use the interface index instead the ip to identify the interface EXPERIMENTAL
        bool reduceFuncionality = default (false);
    gates:
        input from_ip;
        output to_ip;
}



