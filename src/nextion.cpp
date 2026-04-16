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
uint16_t NextionInterface::delta = 0;
uint16_t NextionInterface::prevmph = -1;  
uint16_t NextionInterface::currentMessage = 0;
double NextionInterface::prevLapTime = -1;
uint8_t NextionInterface::brakeTempPrev = 0;
bool NextionInterface::waterTemp = false;
bool NextionInterface::waterPump = false;
bool NextionInterface::oilTemp = false;
bool NextionInterface::oilPump = false;
bool NextionInterface::voltage = false;
bool NextionInterface::neutral = false;
bool NextionInterface::fan = false;
bool NextionInterface::fuelPump = false;
bool NextionInterface::MLI = false;
bool NextionInterface::message = false;
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
// Convert Kilometer per Hour to Miles per Hour
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
    // get one decimal of precision
    if(newOilPressure != oilPressure){
        oilPressure = newOilPressure;

        String instruction = "oilPressureVar.txt=\"" + static_cast<String>(oilPressure) + " PSI\"";
        sendNextionMessage(instruction);
    }
}
//Sets voltage for current screen would have to be multiplied by 100
void NextionInterface::setVoltage(float value) {
    if (value != batteryVoltage) {
        batteryVoltage = value;

        String instruction = "voltageVar.txt=\"" + String(value, DEC) + " V\"";
        sendNextionMessage(instruction);
    }
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
    value = (value / 50);
    value = value*50;
    if(value != engineRPM){
        engineRPM = value;

        String instruction = "rpm.txt=\"" + String(engineRPM, DEC) + "\"";
        sendNextionMessage(instruction);
    }
}
//Set Gear level
void NextionInterface::setGear(int value) {
    if(value!=gear){
        gear = value;
        if (gear == 0){
            String instruction = "gearShiftVar.txt=\"" + String('N') + '\"';
            sendNextionMessage(instruction);
        }else{
            String instruction = "gearShiftVar.txt=\"" + String(gear) + '\"';
            sendNextionMessage(instruction);
        }
    }
}

// Set Lambda
void NextionInterface::setLambda(float value) {
    if (value != lambda) {
        lambda = value;
        
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

// Sets the Delta (difference in lap time)
void NextionInterface::setDelta(double value){
    if(value != delta){
        delta = value;

        String instruction;
        if(delta > 0){
            instruction = "deltaVar.txt=\"" + String("+") + String(delta, 2) + "\"";
        } else if(delta < 0){
            instruction = "deltaVar.txt=\""  + String(delta) + "\"";
        }
        sendNextionMessage(instruction);
    }
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

// Set the image flag, TRUE-Green, FALSE-Red
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
    if(value != fuelPump){
        fuelPump = value;

        setButtonImage("fuelPumpVar", fuelPump);
    }
}

void NextionInterface::setFanBool(bool value) {
    if(value != fan){
        fan = value;

        setButtonImage("fanVar", fan);
    }
}

void NextionInterface::setWaterPumpBool(bool value) {
    if(value != waterPump){
        waterPump = value;

        setButtonImage("waterPumpVar", waterPump);
    }
}

void NextionInterface::setMLIBool(bool value) {
    if(value != MLI){
        MLI = value;

        setButtonImage("MLIVar", MLI);
    }
}

void NextionInterface::setMessageBool(bool value) {
    if(value != message){
        message = value;

        setButtonImage("MessageVar", message);
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