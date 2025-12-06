#include "main.h"
#include "neopixel.h"
#include <IntervalTimer.h>


// Create an IntervalTimer object
IntervalTimer timer;

int const button = 6;

void setup() {
  //Inits buttons
  pinMode(button, INPUT_PULLUP);
  
  Serial.begin(9600);
  //Starts Nextion
  Serial.println("Starting nextion interface");
  NextionInterface::init(); // Creates Serial Port to Display
  Serial.println("Nextion interface initialized.");

  NextionInterface::switchToDriver();
  

  while(NextionInterface::getCurrentPage() != page::DRIVER){
      Serial.println("NOT BReaking");
  }
  //Start Can
  Serial.println("NOT BReaking");
  CanInterface::init();


  //Starts Rev Lights
  RevLights::begin(75, true, 9600);
  //Switches to the driver screen

  timer.begin(lapTimeButton, 1000);
}

void loop() {
  //Updates Can
  CanInterface::task();

}

void lapTimeButton(){
  if(digitalRead(button) == 0){
    CanInterface::lapTime(true);
  }
  else{ 

    CanInterface::lapTime(false);
  }
}