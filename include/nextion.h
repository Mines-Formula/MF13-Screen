#ifndef NEXTION_H
#define NEXTION_H

#include <string>
#include <Arduino.h>
#include "can.h"
#include "can_debug.h"

enum page
{
    LOADING,
    STARTUP,
    DRIVER,
    YIPPEE,
    WARNING,
    DIAGNOSTICS
};

class NextionInterface
{
private:
    static short ctof(short celsius);
    static short kmhtomph(short kmh);
    static page current_page;

    static void sendNextionMessage(String message);

    static int const RGB565_GREEN = 1472;
    static int const RGB565_ORANGE = 47936;
    static int const RGB565_RED = 45056;
    static int const RGB565_BLACK = 0;

    static int const GREEN_BUTTON_ID = 5;
    static int const RED_BUTTON_ID = 4;
    
    static bool waterPumpBool;
    static bool oilTempBool;
    static bool oilPumpBool;
    static bool voltageBool;
    static bool startupSpeedBool;
    static bool fuelPumpBool;
    static bool fanBool;
    static bool MLIBool;
    static bool messageBool;
    static int image;
    static bool neutral;
    static uint8_t waterTemp;
    static uint8_t oilTemp;
    static uint16_t oilPressure;
    static uint8_t brakeTempPrev;
    static float batteryVoltage;
    static uint16_t engineRPM;
    //static uint16_t displayRPM;
    static float lambda;
    static int8_t gear;
    static uint16_t prevmph;
    static uint16_t delta;
    static uint16_t currentMessage;
    static double prevLapTime;
    static int8_t Driver;
    static String Drivers[5];
public:
    NextionInterface();
    /**
     * @brief Initialize Screan
     * 
     */
    static void init();

    /**
     * @brief Convert Celsius to Farenheight
     * 
     * @param celsius 
     * @return short farenheight
     */


    /**
     * @brief Convert Kilometer to Miles Per Hour
     * 
     * @param kmh 
     * @return short 
     */


    /**
     * @brief Set the Water Temp 
     * 
     * @param value 
     */
    static void setWaterTemp(int value);

    /**
     * @brief Set the Oil Temp 
     * 
     * @param value 
     */
    static void setOilTemp(uint8_t value);

    /**
     * @brief Set the Oil Pressure
     * 
     * @param value 
     * @param value2 
     */
    static void setOilPressure(uint8_t value, uint8_t value2);
    /**
    * @brief Update new screens with the old values
     * 
     * 
     * 
     */
    static void switchScreenUpdate();
    /**
     * @brief Set the Voltage 
     * 
     * @param value 
     */
    static void setVoltage(float value);
    /**
     * @brief Set Drivers name
     * 
     * @param value 
     */
    static void setDriver(int value);
    /**
     * @brief Set the Driver Message 
     * 
     * @param value 
     */
    static void setDriverMessage(uint16_t value);

    /**
     * @brief Set the RPM
     * 
     * @param value 
     */
    static void setRPM(uint16_t value);

    /**
     * @brief Set the Gear 
     * 
     * @param gear 
     */
    static void setGear(int gear);
    
    /**
     * @brief Set the Button Image 
     * 
     * @param elementName 
     * @param value 
     */
    static void setButtonImage(String elementName, bool value);

    /**
     * @brief Set the Fuel Pump Bool
     * 
     * @param value 
     */
    static void setFuelPumpBool(bool value);

    /**
     * @brief Set the Fan Bool
     * 
     * @param value 
     */
    static void setFanBool(bool value);

    /**
     * @brief Set the Water Pump Bool 
     * 
     * @param value 
     */
    static void setWaterPumpBool(bool value);

    /**
     * @brief Set Machine Light Indicator Bool
     * 
     * @param value 
     */
    static void setMLIBool(bool value);

    /**
     * @brief Set the Message Bool 
     * 
     * @param value 
     */
    static void setMessageBool(bool value);

    /**
     * @brief Set the Speed
     * 
     * @param mph 
     */
    static void setSpeed(int mph);

    /**
     * @brief Set the Fuel Pump Value
     * 
     * @param value 
     */
    static void setFuelPumpValue(bool value);

    /**
     * @brief Set the Fan Value
     * 
     * @param value 
     */
    static void setFanValue(bool value);

    /**
     * @brief Set the Water Pump Value
     * 
     * @param value 
     */
    static void setWaterPumpValue(bool value);

    /**
     * @brief Set the Lambda
     * 
     * @param value 
     */
    static void setLambda(float value);

    /**
     * @brief Set the Lap Time 
     * 
     * @param lapTime 
     */
    static void setLapTime(double lapTime);
    
    /**
     * @brief Set the Delta
     * 
     * @param delta 
     */
    static void setDelta(double delta);

    /**
     * @brief Set the Brake Temp object
     * 
     * @param temp 
     * @param name 
     */
    static void setBrakeTemp(float temp, String name);

    /**
     * @brief Switch Screen to Loading
     * 
     */
    static void switchToLoading();
    
    /**
     * @brief Switch Screen to Start Up
     * 
     */
    static void switchToStartUp();

    /**
     * @brief Switch Screen to Driver
     * 
     */
    static void switchToDriver();

    /**
     * @brief Switch to Yippee DO NOT USE FOR FUN
     * 
     */
    static void switchToYippee();
    
    /**
     * @brief Switch Screen to Warning
     * 
     */
    static void switchToWarning(String WARNING);

    /**
     *@brief Switch Screen to Diagnostic  */ 
    static void switchToDiagnostic();

    /**
     * @brief Get the Current Page
     * 
     * @return page 
     */
    static page getCurrentPage();
};

#endif // NEXTION_H