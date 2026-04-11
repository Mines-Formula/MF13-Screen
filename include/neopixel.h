#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#ifndef NEOPIXEL_H
#define NEOPIXEL_H


class RevLights {
public:
    RevLights() {};
    constexpr static const int LED_PINS = 26;
    constexpr static const int NUM_PIXELS = 12;

    constexpr static const int REDLINE = 13000;
    constexpr static const int SHIFT_POINT = 12000;
    constexpr static const int RPM_DIFFERENCE = 500;
    
    // We're making an array of leds that each have an rpm threshold and a color
    struct ledRPMThreshold {
        int thresholds[6];
        uint32_t color;
    };

    static Adafruit_NeoPixel pixels;
    // in GRB
    constexpr static const int LED_COLOR_RED = 65280;       // 0x00FF00
    constexpr static const int LED_COLOR_GREEN = 16711680;  // 0xFF0000
    constexpr static const int LED_COLOR_BLUE = 255;        // 0x0000FF
    constexpr static const int LED_COLOR_YELLOW = 8388352;
    constexpr static const int LED_COLOR_OFF = 0;
    ledRPMThreshold ledRPMThresholds[NUM_PIXELS]; //makes a pointer to threshold and color 
    constexpr static const int RPMShiftPoints[7]={8000,12510,11643,11322,11017,10834,12000};
    constexpr static const int RPMShiftDownPoints[7]={0,8000,9100,9700,9800,9900,10000};


    /**
     * @brief Start REV Lights
     * 
     * @param brightness 
     * @param initSerial 
     * @param serialBaud 
     */
    void begin(uint8_t brightness = 75, bool initSerial = false, uint32_t serialBaud = 9600);

    /**
     * @brief Update REV Lights
     * 
     * @param rpm 
     */
    void updateLights(int rpm, uint8_t numGear);

    /**
     * @brief Set the threasholds for leds in ledRPMThresholds struct for setting the lights on the screen dynamicaly 
     * 
     * @param numGear 
     */
    void setLEDThreshold();
    
    /**
     * @brief startup light sequence
     */
    static void startupSequence();
};

extern RevLights RevLight;

#endif //NEOPIX_H