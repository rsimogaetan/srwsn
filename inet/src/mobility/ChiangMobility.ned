package inet.mobility;

simple ChiangMobility like BasicMobility
{
    parameters:
        double updateInterval @unit("s") = default(0.1s);
        double stateTransitionUpdateInterval @unit("s") = default(3s);
        double speed @unit("kph") = default(10kph);
}
