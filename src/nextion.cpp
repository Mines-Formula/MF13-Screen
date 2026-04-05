#include "nextion.h"
//Inits Nextion as Loading Screen
page NextionInterface::current_page = page::LOADING;
//Sets initial value
uint16_t NextionInterface::engineRPM = 999;
uint8_t NextionInterface::waterTemp = -1;
uint8_t NextionInterface::oilTemp = -1;
uint16_t NextionInterface::oilPressure = 999;
float NextionInterface::batteryVoltage = 999;
float NextionInterface::lambda = -1;
int8_t NextionInterface::gear = -1;
uint16_t NextionInterface::prevmph = -1;  
uint16_t NextionInterface::currentMessage = 0;
double NextionInterface::prevLapTime = -1;
uint8_t NextionInterface::brakeTempPrev = 0;
bool NextionInterface::startupWaterTemp = false;
bool NextionInterface::startupWaterPump = false;
bool NextionInterface::startupOilTemp = false;
bool NextionInterface::startupOilPump = false;
bool NextionInterface::startupVoltage = false;
bool NextionInterface::startupSpeed = false;
bool NextionInterface::startupRPM = false;
bool NextionInterface::startupGear = false;
bool NextionInterface::neutral = false;
bool NextionInterface::startupFan = false;
bool NextionInterface::startupFuelPump = false;
bool NextionInterface::startupMLI = false;
bool NextionInterface::startupMessage = false;
NextionInterface::NextionInterface() {}

void NextionInterface::init() {
    Serial2.begin(9600);
    delay(200);
    Serial.println("Nextion Setup");
    switchToLoading();
}
//Converts the given value from Celsius to Farenheight
short NextionInterface::ctof(short celsius) {
    return (celsius * 9 / 5) + 32;
}
// Convertt Kilometer per Hour to Miles per Hour
short NextionInterface::kmhtomph(short kmh){
    return (kmh /1.6);
}
//Sends Message to message
void NextionInterface::sendNextionMessage(String message) {
    // Serial.println(message);
    Serial2.print(message);
    Serial2.write(255);
    Serial2.write(255);
    Serial2.write(255);
}
//Sets the Water Temp on Screen
void NextionInterface::setWaterTemp(int value) {
    if(value != waterTemp){
        waterTemp = value;

        String instruction = "waterTempVar.txt=\"" + static_cast<String>(ctof(value)) + "F\"";
        sendNextionMessage(instruction);
    }

}
//Set Oil Temp
void NextionInterface::setOilTemp(uint8_t value) {

    if(value != oilTemp){
        oilTemp = value;

        String instruction = "oilTempVar.txt=\"" + static_cast<String>(ctof(value))+"F\"";
        sendNextionMessage(instruction);
    }

}
//Set Oil Pressure and send string
void NextionInterface::setOilPressure(uint8_t value, uint8_t value2) {
    uint16_t newOilPressure = (((static_cast<uint16_t>(value2)) | (static_cast<uint16_t>(value) << 8)) * 0.0145);
    // Serial.println(newOilPressure);
    // get one decimal of precision
    if(oilPressure != newOilPressure){
        // Serial.printf("value: 0x%X 0x%X\n", value, value2);
        oilPressure = newOilPressure;
        String instruction = "oilPressureVar.txt=\"" + static_cast<String>(oilPressure) + " PSI\"";
        sendNextionMessage(instruction);
    }
}
//Sets voltage for current screen would have to be multiplied by 100
void NextionInterface::setVoltage(float value) {
    // if (value != batteryVoltage) {
        batteryVoltage = value;

        String instruction = "voltageVar.txt=\"" + String(value, DEC) + " V\"";
        sendNextionMessage(instruction);
    // }
}
//Send a message to the driver
void NextionInterface::setDriverMessage(uint16_t value) {
    if(value != currentMessage) {
        currentMessage = value;

        String instruction = "messageDriver.txt=\"" + String(value) + "\"";
        sendNextionMessage(instruction);
    }
}
//Set the RPM on the screen
void NextionInterface::setRPM(uint16_t value) {
         int roundedValue = (value / 50);
         roundedValue = roundedValue*50;

        String instruction = "rpm.txt=\"" + String(roundedValue, DEC) + "\"";
        sendNextionMessage(instruction);

}
//Set Gear level
void NextionInterface::setGear(int numGear) {
        gear = numGear;

        if (numGear == 0){
            String instruction = "gearShiftVar.txt=\"" + String('N') + '\"';
            sendNextionMessage(instruction);
        }else{
            String instruction = "gearShiftVar.txt=\"" + String(numGear) + '\"';
            sendNextionMessage(instruction);
        }
}

// Set Lambda
void NextionInterface::setLambda(float value) {
    if (value != lambda) {
        lambda = value;
    //     Serial.println(lambda);
    
        String instruction = "lambda.txt=\"" + String(value, 3) + " LA\"";
        sendNextionMessage(instruction);
    }
}


// Set Speed
void NextionInterface::setSpeed(int value){
    if(prevmph != value){
        prevmph = value;
        String instruction = "speedVar.txt=\"" + String(value, DEC) + " MPH" + "\"";
        sendNextionMessage(instruction);
    }
}

// Set the Lap Time
void NextionInterface::setLapTime(double lapTime){
    if(prevLapTime != lapTime){
        prevLapTime = lapTime;
        String instruction = "lapTimeVar.txt=\"" + String(lapTime, 2) + " S" + "\"";
        sendNextionMessage(instruction);
    }
}

void NextionInterface::setDelta(double delta){
    String instruction;
    if(delta > 0){
        instruction = "deltaVar.txt=\"" + String("+") + String(delta, 2) + "\"";
    } else if(delta < 0){
        instruction = "deltaVar.txt=\""  + String(delta) + "\"";
    }
    sendNextionMessage(instruction);
}
// Set the brake temp of the highest brake temp and set the name of the brake temp
void NextionInterface::setBrakeTemp(float temp, String name){
    if(temp != brakeTempPrev){
        brakeTempPrev = temp;
        String instruction = "brakeTempVar.txt=\"" + String(temp) + " F " + "\"";
        sendNextionMessage(instruction);
        String instructionName = "brakeNum.txt=\"" + name + "\"";
        sendNextionMessage(instructionName);

    }
}

// SET THE IMAGE FLAGS
void NextionInterface::setButtonImage(String elementName, bool value) {

    String instruction = "";
    
    if (!value) {
        instruction = elementName + ".pic=" + String(RED_BUTTON_ID);

    } else{
        instruction = elementName + ".pic=" + String(GREEN_BUTTON_ID);
    }

    sendNextionMessage(instruction);
}

//SET FLAGS
void NextionInterface::setFuelPumpBool(bool value) {
    setButtonImage("fuelPumpVar", value);
    if(!startupFuelPump){
        setButtonImage("fuelPumpVar", value);
        startupFuelPump = true;
    }
}

void NextionInterface::setFanBool(bool value) {
    setButtonImage("fanVar", value);
    if(!startupFan){
        setButtonImage("fanVar", value);
        startupFan = true;
    }
}

void NextionInterface::setWaterPumpBool(bool value) {
    setButtonImage("waterPumpVar", value);
    if(!startupWaterPump){
        setButtonImage("waterPumpVar", value);
        startupWaterPump = true;
    }
}

void NextionInterface::setMLIBool(bool value) {
    setButtonImage("MLIVar", value);
    if(!startupMLI){
        setButtonImage("MLIVar", value);
        startupMLI = true;
    }
}

void NextionInterface::setMessageBool(bool value) {
    setButtonImage("MessageVar", value);
    if(!startupMessage){
        setButtonImage("MessageVar", value);
        startupMessage = true;
    }
}



// SCREEN PAGE FUNCTIONS

void NextionInterface::switchToLoading() {
    if(current_page != page::LOADING){
        sendNextionMessage("page LOADING");
        current_page = page::LOADING;
    }  
}

void NextionInterface::switchToStartUp() {
    if(current_page != page::STARTUP){
        sendNextionMessage("page STARTUP");
        current_page = page::STARTUP;
    }
}

void NextionInterface::switchToDriver() {
    if(current_page != page::DRIVER){
        sendNextionMessage("page DRIVER");
        current_page = page::DRIVER;
    }
}

void NextionInterface::switchToDiagnostic(){
    if(current_page != page::DIAGNOSTICS){
        sendNextionMessage("page DIAGNOSTICS");
        current_page = page::DIAGNOSTICS;
    }
}

void NextionInterface::switchToYippee() {
    
}

void NextionInterface::switchToWarning(const String WARNING) {
    // if(current_page != page::WARNING){
    //     sendNextionMessage("page WARNING");
    //     String instructionName = String("warningLabel.txt=\"") + WARNING + "\"";
    //     current_page = page::WARNING;
    // }
}


page NextionInterface::getCurrentPage() {
    return current_page;
}