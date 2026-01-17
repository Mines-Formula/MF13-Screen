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
    /**
     * @brief Initialize Can
     * 
     * @return true 
     * @return false 
     */
    static bool init();

    /**
     * @brief Print CAN
     * 
     * @param msg 
     */
    static void print_can_sniff(const CAN_message_t &msg);

    /**
     * @brief Recieve any Updates
     * 
     * @param msg 
     */
    static void receive_can_updates(const CAN_message_t &msg);

    /**
     * @brief Calculate distance from target
     * 
     * @param lat1 
     * @param lon1 
     * @param lat2 
     * @param lon2 
     * @return double 
     */
    static double haversine(double lat1, double lon1, double lat2, double lon2);

    /**
     * @brief Start Laptime and update laptime
     * 
     * @param button 
     */
    static void lapTime(const bool button);
    /**
     * @brief CAN Events
     * 
     */
    static void task();

    static double longitude;
    static double latitude;
    static double startLongitude;
    static double startLatitude;
    static bool lapStarted;
    static uint64_t lapStartTime;
    static u_int16_t fastLapTime;
    static int16_t delta;
    
    static bool isInStartZone;

    static bool wasInZone;

    static double lapTimeSeconds;

    static double lapTimeEnd;

    static float brakeTempFL;
    static float brakeTempFR;
    static float brakeTempRL;
    static float brakeTempRR;
    static bool lapstart;
    

};

#endif //CAN_H