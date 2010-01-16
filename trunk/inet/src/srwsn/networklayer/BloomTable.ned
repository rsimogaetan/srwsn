package inet.srwsn.networklayer;

simple BloomTable
{
    parameters:
        bool ready;
        int maxEntry = default(512); // The max number of entry 
        bool sensorTemperature = default(false);
        bool sensorPressure = default(false);
        bool sensorSon = default(false);
        bool sensorLigth = default(false);
        bool sensorHumidity = default(true);
        int tailleFiltre = default(100);
        @display("i=block/table");
}
