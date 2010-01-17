package inet.srwsn.networklayer;

simple TableRARE
{
    parameters:
        bool ready;
        int maxEntry = default(512); // The max number of entry
        int queryRelaxationMaxRetry = default(3); // The number max to expand the request
        int maxQueries = default(6); // Temperature, Pression, Son, Lumiere, Humidite, Puit
        int maxTypes = default(4); // Relation math, Feu, Intru, Pluie 
        int maxPeers = default(5); 
        @display("i=block/table");
}
