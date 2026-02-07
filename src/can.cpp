#include "Arduino.h"

#include "nextion.h"
#include "neopixel.h"
#include <cmath>
#include<chrono>
#include <cstdint>

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> CanInterface::Can0; //Declare Object CanInterface

CanInterface::CanInterface(){
    // deprecated function
}
double CanInterface::longitude = 0.0;
double CanInterface::latitude = 0.0;
double CanInterface::startLongitude = 0.0;
double CanInterface::startLatitude = 0.0;

bool CanInterface::lapStarted = false;
uint64_t CanInterface::lapStartTime = 0;

bool CanInterface::isInStartZone = false;
bool CanInterface::wasInZone   = false;
static const double MIN_LAP_MS = 10000.0;
static const double RADIUS_METERS = 10.0;
double CanInterface::lapTimeSeconds = 0;
double CanInterface::lapTimeEnd = 0;
uint16_t CanInterface::fastLapTime = 0;
int16_t CanInterface::delta = 0;
bool firstLap = false;
int CanInterface::count = 0;
int CanInterface::lastTime = 0;

float CanInterface::brakeTempFL = 0;
float CanInterface::brakeTempFR = 0;
float CanInterface::brakeTempRL = 0;
float CanInterface::brakeTempRR = 0;
bool CanInterface::lapstart = false;

CAN_message_t CanInterface::shift_msg; //Receives message from teensy
bool CanInterface::canActive = false;

bool CanInterface::init(){ // Init Can Interface Probaly dont change lol
    pinMode(32,OUTPUT); digitalWrite(32,HIGH); 
    pinMode(33,OUTPUT); digitalWrite(33,HIGH);

    Can0.begin();
    Can0.setBaudRate(1000000); //needs to be million to talk with CAN
    Can0.setMaxMB(16);

    // Can0.setMBFilter(REJECT_ALL);  

    // // Can0.setMBFilter(MB0, 0x640, 0x7F0);
    // // Can0.setMBFilter(MB0,1600);
    // Can0.setMBFilter(MB1,1613);

    Can0.enableFIFO();
    Can0.enableFIFOInterrupt();
    Can0.onReceive(receive_can_updates);
    lapStarted = false;
    
    return 1;
}
//Declares cases for each of the following i.e. Overrun sets up an overflow flags
void CanInterface::print_can_sniff(const CAN_message_t &msg){
    Serial.print("MB "); Serial.print(msg.mb);
    Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
    Serial.print("  LEN: "); Serial.print(msg.len);
    Serial.print(" EXT: "); Serial.print(msg.flags.extended);
    Serial.print(" TS: "); Serial.print(msg.timestamp);
    Serial.print(" ID: "); Serial.print(msg.id, DEC);
    Serial.print(" Buffer: ");
    //Prints this in Decimal
    for ( uint8_t i = 0; i < msg.len; i++ ) {
        Serial.print(msg.buf[i], DEC); Serial.print(" ");
    } 
    Serial.println();
}
//Reads the and sets the values for all ideal places baced on box
void CanInterface::receive_can_updates(const CAN_message_t &msg) {
    canActive = true;

    if(millis() - lastTime >= 1000){
        Serial.print("Msg Amount: ");
        Serial.println(count);
        lastTime = millis();
        count = 0;
    }


    count++;
    // Serial.println("AHHH");
    switch (msg.id) {
   
        
        // 1600: RPM

         case 1613: {
            Serial.printf("Gear: %d", millis());
            Serial.print(" ");
            Serial.println(msg.buf[6] & 15);
            NextionInterface::setGear(msg.buf[6] & 15);

            break;
        }

        case 1600: {
            uint16_t rpm = (msg.buf[1] | (msg.buf[0] << 8));
            // Serial.printf("RPM: %d\n", millis());
            // Serial.println(rpm);
           
            //uint16_t speed = (msg.buf[2]);
            NextionInterface::setRPM(rpm);
            // NextionInterface::setBrakeTemp(rpm,"");
            // // NextionInterface::setSpeed(speed);
            RevLights::updateLights(rpm);
            break;
        }

        // 1613: Gear

        // 1609: Temps & Voltage
        case 1609: {
            NextionInterface::setWaterTemp(msg.buf[0] - 40);
            NextionInterface::setOilTemp(msg.buf[1] - 40);
            NextionInterface::setVoltage(msg.buf[5] * 0.1f);
            break;
        }
           
        

        // 1612: Warning flags
        case 1612: {
            // Bit masks (byte 5)
            // constexpr uint8_t coolantMask     = 0b10000000; // bit 0
            // constexpr uint8_t oilTempMask     = 0b00010000; // bit 3
            // constexpr uint8_t oilPressureMask = 0b00001000; // bit 4
            // constexpr uint8_t fuelPressureMask= 0b00000001; // bit 6
            // bool coolantTempWarning = msg.buf[5] & coolantMask;
            // bool oilTempWarning = msg.buf[5] & oilTempMask;
            // bool oilPressureWarning = msg.buf[5] & oilPressureMask;
            // bool fuelPressureWarning = msg.buf[5] & fuelPressureMask;            

            // if (coolantTempWarning ) {
            //     NextionInterface::switchToWarning("Coolant Temp Warning");
            // } else if(oilTempWarning){
            //     NextionInterface::switchToWarning("Oil Temp Warning");
            // } else if(oilPressureWarning){
            //     NextionInterface::switchToWarning("Oil Pressure Warning");
            // } else if(fuelPressureWarning){
            //     NextionInterface::switchToWarning("Fuel Pressure Warning");
            // }
            //     else {
            //     NextionInterface::switchToDriver();
            // }
            break;
        }
          

        // 1284: WaterPump, FuelPump, Fan (fill in as needed)
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
                    Serial.println("Fan Error");
            }
             break;
        }
        
  // keep break inside the case block
        // 1604: Oil Pressure
        case 1604: {
            // OilPressure is carried in bytes 6..7; header expects two uint8_t args
            NextionInterface::setOilPressure(msg.buf[6], msg.buf[7]);
            // TODO: machine light indicator (MLI)
             break;
        }
        

        // 1617: Lambda
        case 1617: 
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
        Serial.println("Lap Started");
        startLongitude = longitude;
        startLatitude = latitude;
        lapStarted = true;
        lapStartTime = millis();
        Serial.println(longitude);
        lapstart = true;
    }
    double distance = haversine(latitude, longitude, startLatitude, startLongitude) * 1000;
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