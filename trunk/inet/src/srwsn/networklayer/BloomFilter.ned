package inet.srwsn.networklayer;

simple BloomFilter
{
    parameters:
        bool ready;
        int maxEntry = default(128); // The max number of entry 
        @display("i=block/table");
}
