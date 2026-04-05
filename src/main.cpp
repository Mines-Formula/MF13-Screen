#include "main.h"
#include "neopixel.h"
#include "Arduino.h"

RevLights Revlight;//declares for all files because of extern

void setup() {
  Serial.begin(9600);
  //Starts Nextion
  delay(5000);
  
  Serial.println("Starting nextion interface");
  NextionInterface::init(); // Creates Serial Port to Display
  Serial.println("Nextion interface initialized.");

  //Starts Rev Lights
  RevLight.begin(75, true, 9600);
  
  CanInterface::init();

  //Switches to the driver screen
  NextionInterface::switchToDriver();
  
}

void loop() {
  //Updates Can
  // Serial.println("AHH");
  CanInterface::task();
}
