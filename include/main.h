#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <nextion.h>
#include <neopixel.h>
#include <can.h>

extern CanInterface can;
extern NextionInterface screen;
extern RevLights revLights;
void buttonsCallback();
void lapTimeButton();

#endif //MAIN_H 