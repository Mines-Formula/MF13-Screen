#include "neopixel.h"
Adafruit_NeoPixel RevLights::pixels(RevLights::NUM_PIXELS, RevLights::LED_PINS, NEO_GRB + NEO_KHZ800);
RevLights RevLight;
void RevLights::begin(uint8_t brightness, bool initSerial, uint32_t serialBaud)
    {
        if(initSerial){
            Serial.begin(serialBaud);
            Serial.println("RevLights begin()");
        }

        // if(!ledRPMThresholds){
        //     ledRPMThresholds = new ledRPMThreshold[NUM_PIXELS];
        // }

        pixels.begin();
        pixels.setBrightness(brightness);
        pixels.clear();
        pixels.show();
        setLEDThreshold();

        delay(100);
        updateLights(0 , 0);
}

//f\left(g,l,i\right)=\frac{\left(g-l\right)}{11}\left(i\right)+l



//commented out old Will code
// for(int gear=0; gear<6;gear++){

//     for (int i = 8; i < 12; i++) {
//         ledRPMThresholds[i].thresholds[gear]= ((RPMShiftPoints[gear]-200-RPMShiftDownPoints[gear])/12)*(i+1)+RPMShiftDownPoints[gear]; // <- need to look at this 
//         ledRPMThresholds[i].color = LED_COLOR_BLUE;
//     }

//     for (int i = 7; i >= 0; i--) {
//         ledRPMThresholds[i].thresholds[gear]= ((RPMShiftPoints[gear]-200-RPMShiftDownPoints[gear])/12)*(i+1)+RPMShiftDownPoints[gear];
//         if (i >= 4) {
//             ledRPMThresholds[i].color = LED_COLOR_GREEN;
//         } else {
//             ledRPMThresholds[i].color = LED_COLOR_YELLOW;
//         }
//     }
// }

void RevLights::setLEDThreshold(){

    for (int i = 0; i < 12; i++) {
        for(int gear=0; gear<6;gear++){
            int upperShiftPoint=RPMShiftPoints[gear]-200;
            int lowerShiftPoint=RPMShiftDownPoints[gear]+150;
            ledRPMThresholds[i].thresholds[gear]= ((upperShiftPoint-lowerShiftPoint)/12)*(i)+lowerShiftPoint;

        }
        if(i<4){
            ledRPMThresholds[i].color = LED_COLOR_YELLOW;
        }else if(i<=7){
            ledRPMThresholds[i].color = LED_COLOR_GREEN;

        }else{
            ledRPMThresholds[i].color = LED_COLOR_BLUE;
        }

    }
}


void RevLights::updateLights(int rpm, uint8_t numGear) //DEV NOTE: If this class is failing, it likely means data types arent being initialized
    {      

    // Memory hasnt initialized yet   
    if (ledRPMThresholds[0].thresholds == nullptr) {
        return;
    }

    pixels.clear();
    
    if ((rpm >= RPMShiftPoints[numGear]+150)||(numGear==0&&rpm>8000)) { //ask because this should probably be basied on the gear
        // All red at/over redline
        pixels.setBrightness(255);
        for (int i = 0; i < NUM_PIXELS; ++i) {
            pixels.setPixelColor(i, LED_COLOR_RED);
        }
    } else if (rpm == 0) {
        pixels.setBrightness(75);
        // All green when engine is off
        for (int i = 0; i < NUM_PIXELS; ++i) {
            pixels.setPixelColor(i, LED_COLOR_GREEN);
        }
    } 
    else if(rpm>=RPMShiftPoints[numGear]-150){ //sets all lights to blue at shift point
        pixels.setBrightness(255);
        for (int i = 0; i < NUM_PIXELS; ++i) {
            if(i<2 || i<9){
            pixels.setPixelColor(i, LED_COLOR_RED);
            }else{
            pixels.setPixelColor(i, LED_COLOR_BLUE);
            }

            //flash blue(full brightness)bring there attetion
        }
    }
     else {
        pixels.setBrightness(75);
        // Fill according to thresholds
        for (int i = 0; i < NUM_PIXELS; ++i) {
            if (rpm >= ledRPMThresholds[i].thresholds[numGear]) {
                pixels.setPixelColor(i, ledRPMThresholds[i].color);
            } else {
                pixels.setPixelColor(i, LED_COLOR_OFF);
            }
        }
    }

    pixels.show();
    }