package inet.mobility;

simple GaussMarkovMobility like BasicMobility
{
    parameters:
        double updateInterval @unit("s") = default(0.1s);
        double alpha = default(0.5); // [0;1], 0 - brown motion, 1 - linear motion
        double speed @unit("kph") = default(10kph);
        double angle @unit("deg") = default(0deg);
        double variance = default(40);
        int margin = default(30);
}
