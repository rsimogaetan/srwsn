package inet.srwsn.networklayer;

simple BloomTable
{
    parameters:
        bool ready;
        int maxEntry = default(512); // The max number of entry 
        int tailleFiltre = default(100);
        @display("i=block/table");
}
