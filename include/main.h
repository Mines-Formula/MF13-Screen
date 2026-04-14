#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <nextion.h>
#include <neopixel.h>
#include <can.h>
#include <can_debug.h>

extern CanInterface can;
extern NextionInterface screen;
extern RevLights revLights;

#endif //MAIN_H 