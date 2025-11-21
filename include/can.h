#include <FlexCAN_T4.h>

#ifndef CAN_H
#define CAN_H

class NextionInterface;

class CanInterface{
public:
    CanInterface();

    static bool canActive;
    static CAN_message_t shift_msg;
    static bool startLapTime;

    static FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can0;

    static bool init();

    static void print_can_sniff(const CAN_message_t &msg);

    static void receive_can_updates(const CAN_message_t &msg);

    static double haversine(double lat1, double lon1, double lat2, double lon2);


    static void lapTime(const bool button);

    static void task();

    static double longitude;
    static double latitude;
    static double startLongitude;
    static double startLatitude;
    static bool lapStarted;
    static uint64_t lapStartTime;
    
    static bool isInStartZone;

    static bool wasInZone;

    static double lapTimeSeconds;

    static double lapTimeEnd;

    static float brakeTempFL;
    static float brakeTempFR;
    static float brakeTempRL;
    static float brakeTempRR;
    
    

};

#endif //CAN_