#include "Arduino.h"

#include "nextion.h"
#include "neopixel.h"
#include <cmath>
#include <chrono>
#include <cstdint>

// #define SCREEN_DEBUG // disable if not reading from serial 
#include "can_debug.h" 


FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> CanInterface::Can0; //Declare Object CanInterface 

double CanInterface::longitude = 0.0;
double CanInterface::latitude = 0.0;
double CanInterface::startLongitude = 0.0;
double CanInterface::startLatitude = 0.0;

bool CanInterface::lapStarted = false;
double CanInterface::lapStartTime = 0;

bool CanInterface::isInStartZone = false;
bool CanInterface::wasInZone   = false;
static const double MIN_LAP_MS = 10000.0;
static const double RADIUS_METERS = 10.0;
double CanInterface::lapTimeSeconds = 0;
double CanInterface::lapTimeEnd = 0;
double CanInterface::fastLapTime = 0;
double CanInterface::delta = 0;

bool firstLap = false;
int CanInterface::count = 0;
int CanInterface::lastTime = 0;

float CanInterface::brakeTempFL = 0;
float CanInterface::brakeTempFR = 0;
float CanInterface::brakeTempRL = 0;
float CanInterface::brakeTempRR = 0;
bool CanInterface::lapstart = false;
uint8_t numGear = 0;

CAN_message_t CanInterface::shift_msg; //Receives message from teensy
bool CanInterface::canActive = false;

//used to check timestamp when can sends a message
uint32_t lastCanMessageTimeStamp=0;

bool CanInterface::init(){ 
    pinMode(32,OUTPUT); digitalWrite(32,HIGH); 
    pinMode(33,OUTPUT); digitalWrite(33,HIGH);

    Can0.begin();
    Can0.setBaudRate(1000000); //needs to be million to talk with CAN
    Can0.setMaxMB(16);

    // Can0.setMBFilter(REJECT_ALL);  

    // Can0.setMBFilter(MB0, 0x640, 0x7F0);
    // Can0.setMBFilter(MB0,1600);
    // Can0.setMBFilter(MB1,1613);

    Can0.enableFIFO();
    Can0.enableFIFOInterrupt();
    Can0.onReceive(receive_can_updates);
    lapStarted = false;
    
    return 1;
}

//Declares cases for each of the following i.e. Overrun sets up an overflow flags
void CanInterface::print_can_sniff(const CAN_message_t &msg){
    DEBUG_PRINT("MB "); DEBUG_PRINT(msg.mb);
    DEBUG_PRINT("  OVERRUN: "); DEBUG_PRINT(msg.flags.overrun);
    DEBUG_PRINT("  LEN: "); DEBUG_PRINT(msg.len);
    DEBUG_PRINT(" EXT: "); DEBUG_PRINT(msg.flags.extended);
    DEBUG_PRINT(" TS: "); DEBUG_PRINT(msg.timestamp);
    DEBUG_PRINT(" ID: "); DEBUG_PRINT(msg.id, DEC);
    DEBUG_PRINT(" Buffer: ");
    //Prints this in Decimal
    for ( uint8_t i = 0; i < msg.len; i++ ) {
        DEBUG_PRINT_DEC((msg.buf[i])); 
    } 
    DEBUG_PRINT("\n");
}
//Reads the and sets the values for all ideal places baced on box
void CanInterface::receive_can_updates(const CAN_message_t &msg) {
    canActive = true;
    lastCanMessageTimeStamp=millis();//recoreds the time a can message is receved

    if(lastCanMessageTimeStamp - lastTime >= 1000){
        DEBUG_PRINT("Msg Amount: ");
        DEBUG_PRINT(count);
        DEBUG_PRINT("\n");
        lastTime = lastCanMessageTimeStamp;
        count = 0;
    }
    


    count++;
    // Serial.println("AHHH");
    switch (msg.id) {
   
        
     

        //if your case is missing a break it will break everything; just remember that there should be a break after every case!!!!!


         // 1613: Gear
         case 1613: {
            // Serial.printf("Gear: %d", millis());
            // Serial.print(" ");
            // Serial.println(msg.buf[6] & 15);
            numGear=msg.buf[6] & 15; 
            NextionInterface::setGear(numGear); //filter the byte
            
            break;
        }

           // 1600: rpm
        case 1600: {
            uint16_t rpm = (msg.buf[1] | (msg.buf[0] << 8));
            NextionInterface::setRPM(rpm);
            RevLight.updateLights(rpm, numGear);

            //uint16_t speed = (msg.buf[2]);
            // // NextionInterface::setSpeed(speed);
            break;
        }

       

        // 1609: Temps & Voltage
        case 1609: {
            // Problem Area
            NextionInterface::setWaterTemp(msg.buf[0] - 40);
            NextionInterface::setOilTemp(msg.buf[1] - 40);
            NextionInterface::setVoltage((int)((msg.buf[5] * 0.1f) * 10) / 10.0);
            break;
        }
        //driver switch 
        case 0x07F2: {
            // for (int i = 0; i <2; i++) {
            // Serial.print(msg.buf[i]);
            // //Serial.print("\t"); // Adds a tab space between bytes for alignment
            // }
  
            Serial.println((msg.buf[0] << 8 | msg.buf[1])/1000);
            NextionInterface::setDriver((msg.buf[0] << 8 | msg.buf[1])/1000);
            break;
            
        }

        // 1612: Warning flags
        case 1612: {
            // Bit masks (byte 5)
            constexpr uint8_t coolantMask     = 0b00000001; // bit 0
            constexpr uint8_t oilTempMask     = 0b00001000; // bit 3
            constexpr uint8_t oilPressureMask = 0b00010000; // bit 4
            constexpr uint8_t fuelPressureMask= 0b10000000; // bit 6
            bool coolantTempWarning = msg.buf[5] & coolantMask;
            bool oilTempWarning = msg.buf[5] & oilTempMask;
            bool oilPressureWarning = msg.buf[5] & oilPressureMask;
            bool fuelPressureWarning = msg.buf[5] & fuelPressureMask;

            // Serial.println("_________");
            // Serial.println(msg.timestamp);            
            // Serial.println(msg.buf[0]);
            // Serial.println(msg.buf[1]);
            // Serial.println(msg.buf[2]);
            // Serial.println(msg.buf[3]);
            // Serial.println(msg.buf[4]);
            // Serial.println(msg.buf[5]);
            // Serial.println(msg.buf[6]);
            // Serial.println(msg.buf[7]);
            // Serial.println("_________");

            if (coolantTempWarning ) {
                NextionInterface::showWarning("Coolant Temp Warning");
            } else if(oilTempWarning){
                NextionInterface::showWarning("Oil Temp Warning");
            } else if(oilPressureWarning){
                NextionInterface::showWarning("Oil Pressure Warning");
            } else if(fuelPressureWarning){
                NextionInterface::showWarning("Fuel Pressure Warning");
            }
                else {
                NextionInterface::showWarning("");
            }
            break;
        }
        

        case 1284: {

            if(msg.buf[0] == 0 || msg.buf[0] == 1){
                if(msg.buf[1] == 0 || msg.buf[1] == 1){
                    NextionInterface::setWaterPumpBool(msg.buf[1]);
                } 
                else
                    Serial.println("Water Pump Error");

                if(msg.buf[0] == 0 || msg.buf[0] == 1){
                    //NextionInterface::setFuelPumpValue(msg.buf[0]);
                    NextionInterface::setFuelPumpBool(msg.buf[0]);
                }   
                else
                    Serial.println("Fuel Pump Error");

                if(msg.buf[3] == 0 || msg.buf[3] == 1){

                    NextionInterface::setFanBool(msg.buf[3]);
                }
                else
                    DEBUG_PRINT("Fan Error");
            }
             break;
        }
        
  // keep break inside the case block
        // 1604: Oil Pressure
        case 1604: {
            // OilPressure is carried in bytes 6..7; header expects two uint8_t args
            //Look here for potential issue
            NextionInterface::setOilPressure(msg.buf[6], msg.buf[7]);
            // TODO: machine light indicator (MLI)
             break;
        }
        

        // 1617: Lambda
        case 1617: 
            //Look here
            NextionInterface::setLambda(msg.buf[0]);
            break;

        
        case 1608: {
            // int wheelSpeedFL = ((((static_cast<uint16_t>(msg.buf[0])) | (static_cast<uint16_t>(msg.buf[1]) << 8))*0.0277777777778)*15)*0.00094697;
            // int wheelSpeedFR =  ((((static_cast<uint16_t>(msg.buf[2])) | (static_cast<uint16_t>(msg.buf[3]) << 8))*0.0277777777778)*15)*0.00094697;
            // int speed = (wheelSpeedFL+wheelSpeedFR)/2; // gets the average between two wheels
        

            // // Serial.print(msg.buf[3]);
            // // Serial.print("  ");
            // // Serial.println(msg.buf[2]);
            // NextionInterface::setSpeed(speed);
              break;
        }


        
        // Longitude and Latitude
        case 1664:{

            // int32_t lat_raw =
            // ((int32_t)msg.buf[0] << 24) |
            // ((int32_t)msg.buf[1] << 16) |
            // ((int32_t)msg.buf[2] << 8)  |
            // (int32_t)msg.buf[3];

            // int32_t lon_raw =
            // ((int32_t)msg.buf[4] << 24) |
            // ((int32_t)msg.buf[5] << 16) |
            // ((int32_t)msg.buf[6] << 8)  |
            // (int32_t)msg.buf[7];

            // latitude  = (double)lat_raw * 1e-7;
            // longitude = (double)lon_raw * 1e-7;
            break;

        }
        case 6:{
            // int32_t brakeTempFLRaw = (msg.buf[0] | (msg.buf[1] << 8));
            // int32_t brakeTempFRRaw = (msg.buf[2] | (msg.buf[3] << 8));
            // brakeTempFL = brakeTempFLRaw * 0.1f;
            // brakeTempFR= brakeTempFRRaw * 0.1f;
            break;


        }
        case 7:{
            // int32_t brakeTempRLRaw = (msg.buf[0] | (msg.buf[1] << 8));
            // int32_t brakeTempRRRaw = (msg.buf[2] | (msg.buf[3] << 8));
            // brakeTempRL = brakeTempRLRaw * 0.1f;
            // brakeTempRR = brakeTempRRRaw * 0.1f;

            // if(brakeTempFL > brakeTempFR && brakeTempFL > brakeTempRL && brakeTempFL > brakeTempRR){
            //     NextionInterface::setBrakeTemp(brakeTempFL, "Front Left");
            // }
            // else if(brakeTempFR > brakeTempFL && brakeTempFR > brakeTempRL && brakeTempFR > brakeTempRR){
            //     NextionInterface::setBrakeTemp(brakeTempFR, "Front Right");
            // }
            // else if(brakeTempRL > brakeTempFL && brakeTempRL > brakeTempFR && brakeTempRL > brakeTempRR){
            //     NextionInterface::setBrakeTemp(brakeTempRL, "Back Left");
            // }
            // else NextionInterface::setBrakeTemp(brakeTempRR, "Back Right");
            break;

        }
        // 2047: “Any warnings present” message
    
        case 2047: 
            // if(msg.buf != 0){
                
            // }
            break;
        


        default: 
            break;
        

    }
}

void CanInterface::task(){
    Can0.events();
    //SupeSerialReadingFunction();
    canRecieveFailure();
    
}
void CanInterface::SupeSerialReadingFunction(){
//this is supper cool and will do something at some point
NextionInterface::setDriver(3);
NextionInterface::setGear(0);
}


void CanInterface::canRecieveFailure(){
    //Serial.println(lastCanMessageTimeStamp);

    if (millis()- lastCanMessageTimeStamp > 1000){
        RevLight.noCanMessageWarning();
        
    }


}


 

double toRadians(double degree){
    return degree * M_PI / 180;
}

double CanInterface::haversine(double lat1, double lon1, double lat2, double lon2){
    const double R = 6371.0;
    lat1 = toRadians(lat1);
    lon1 = toRadians(lon1);
    lat2 = toRadians(lat2);
    lon2 = toRadians(lon2);

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) + std::cos(lat1) * std::cos(lat2) * std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    double distance = R * c;

    return distance;

}

void CanInterface::lapTime(const bool button){
    if(button){
        DEBUG_PRINT("Lap Started");
        DEBUG_PRINT("\n");
        startLongitude = longitude;
        startLatitude = latitude;
        lapStarted = true;
        lapStartTime = millis();
        DEBUG_PRINT(longitude);
        DEBUG_PRINT("\n");
        lapstart = true;
    }
    // double distance = haversine(latitude, longitude, startLatitude, startLongitude) * 1000; //potentially unused
    // Serial.println("Distance: " + String(distance));
    isInStartZone = (haversine(latitude, longitude, startLatitude, startLongitude) * 1000)< RADIUS_METERS;
    if(lapStarted && !wasInZone && isInStartZone && (millis() - lapStartTime > MIN_LAP_MS) && (startLongitude != 0) && (startLatitude != 0)){
        if(fastLapTime < lapTimeSeconds){
            fastLapTime = lapTimeSeconds;
        }
        lapTimeEnd = millis();
        if(fastLapTime != 0 && firstLap){
            delta = fastLapTime - lapTimeSeconds;
        }else if(firstLap == false){
            firstLap = true;
            delta = lapTimeSeconds;
        }
        NextionInterface::setDelta(delta);
        lapStartTime = lapTimeEnd;
        NextionInterface::setLapTime(0.0);
        lapTimeSeconds = 0;
        
    }
    else if ((startLongitude != 0) && (startLatitude != 0)){
        lapTimeSeconds = ((millis() - lapStartTime) / 1000.0);
        lapTimeSeconds = std::round(lapTimeSeconds * 100.f) / 100.f;
        NextionInterface::setLapTime(lapTimeSeconds); 
        
    }
    wasInZone = isInStartZone;


}


void CanInterface::send_shift(const bool up, const bool down,const bool button3){
    shift_msg.id = 2033;

    shift_msg.len = 6;

    if(up){
        shift_msg.buf[0] = 0x6F;
        shift_msg.buf[1] = 0x7F;
    } else {
        shift_msg.buf[0] = 0;
        shift_msg.buf[1] = 0;
    }
    
    if(down){
        shift_msg.buf[2] = 0x5F;
        shift_msg.buf[3] = 0x7F;
    } else {
        shift_msg.buf[2] = 0;
        shift_msg.buf[3] = 0;
    }

    if(button3){
        shift_msg.buf[4] = 0x5F;
        shift_msg.buf[5] = 0x7F;
    }else{
        shift_msg.buf[4] = 0;
        shift_msg.buf[5] = 0;
    }

    
    Can0.write(shift_msg);
}