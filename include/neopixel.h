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
        int threshold = -1;
        uint32_t color;
    };

    static Adafruit_NeoPixel pixels;
    // in GRB
    constexpr static const int LED_COLOR_RED = 65280;       // 0x00FF00
    constexpr static const int LED_COLOR_GREEN = 16711680;  // 0xFF0000
    constexpr static const int LED_COLOR_BLUE = 255;        // 0x0000FF
    constexpr static const int LED_COLOR_YELLOW = 8388352;
    constexpr static const int LED_COLOR_OFF = 0;
    ledRPMThreshold ledRPMThresholds[NUM_PIXELS]; //makes a pointer to 


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
    void updateLights(int rpm);
    
    /**
     * @brief startup light sequence
     */
    static void startupSequence();
};

extern RevLights RevLight;

#endif //NEOPIX_H