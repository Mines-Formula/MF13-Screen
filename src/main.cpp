#include "main.h"
#include "neopixel.h"
#include "Arduino.h"



void setup() {

  Serial.begin(9600);
  //Starts Nextion
  delay(5000);
  Serial.println("Starting nextion interface");
  NextionInterface::init(); // Creates Serial Port to Display
  Serial.println("Nextion interface initialized.");

  
  CanInterface::init();

  //Starts Rev Lights
  RevLights::begin(75, true, 9600);
  //Switches to the driver screen
  NextionInterface::switchToDriver();
  ;
}

void loop() {
  //Updates Can
  // Serial.println("AHH");
  CanInterface::task();
}
