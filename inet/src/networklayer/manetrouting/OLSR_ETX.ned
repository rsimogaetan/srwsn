package inet.networklayer.manetrouting;
import inet.networklayer.manetrouting.BaseRouting;
simple OLSR_ETX extends BaseRouting
{
    parameters:
        int Willingness = default(3); //ms
        int Hello_ival = default(2); // sec
        int Tc_ival = default(5); // sec
        int Mid_ival = default(5); // sec
        int use_mac = default(0);
        int Mpr_algorithm = default(1);
        int routing_algorithm = default(1);
        int Link_quality = default(2);
        bool Fish_eye = default(false);
        int Tc_redundancy = default(3);
        bool Link_delay = default(true);
        double C_alpha = default(0.2);
        bool UseIndex = default(false); // use the interface index instead the ip to identify the interface EXPERIMENTAL
        bool reduceFuncionality = default (false);
    gates:
        input from_ip;
        output to_ip;
}

