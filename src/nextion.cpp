#include "nextion.h"
//Inits Nextion as Loading Screen
page NextionInterface::current_page = page::LOADING;
//Sets initial value
uint16_t NextionInterface::engineRPM = 0;
uint8_t NextionInterface::waterTemp = -1;
uint8_t NextionInterface::oilTemp = -1;
uint16_t NextionInterface::oilPressure = -1;
float NextionInterface::batteryVoltage = -1;
float NextionInterface::lambda = -1;
int8_t NextionInterface::gear = -1;
int8_t NextionInterface::launchState = 0;
uint16_t NextionInterface::delta = 0;
uint16_t NextionInterface::prevmph = -1;  
uint16_t NextionInterface::currentMessage = 0;
double NextionInterface::prevLapTime = -1;
uint8_t NextionInterface::brakeTempPrev = 0;
String NextionInterface::warningMessage="";
bool NextionInterface::waterPumpBool = false;
bool NextionInterface::oilTempBool = false;
bool NextionInterface::oilPumpBool = false;
bool NextionInterface::voltageBool = false;
bool NextionInterface::fanBool = false;
bool NextionInterface::fuelPumpBool = false;
bool NextionInterface::MLIBool = false;
bool NextionInterface::messageBool = false;
bool NextionInterface::warningBool = false;

int8_t NextionInterface::Driver=-1;
String NextionInterface::Drivers[6] = {"Austin", "Sammy","Jimmy","Schimmy","Noah","Sammy"};
NextionInterface::NextionInterface() {}

void NextionInterface::init() {
    Serial2.begin(9600);
    delay(200);
    DEBUG_PRINT("Nextion Setup");
  


 
    
    
    
}
//Converts the given value from Celsius to Farenheight
short NextionInterface::ctof(short celsius) {
    return (celsius * 9 / 5) + 32;
}
// Convert Kilometer per Hour to Miles per Hour
short NextionInterface::kmhtomph(short kmh){
    return (kmh / 1.6);
}
//Sends Message to message
void NextionInterface::sendNextionMessage(String message) {
    // Serial.println(message);
    Serial2.print(message);
    Serial2.write(255);
    Serial2.write(255);
    Serial2.write(255);
}
void NextionInterface::switchScreenUpdate(){
    if(launchState==0){
    setButtonImage("LaunchVar", 0);//red
    }else{
    setButtonImage("LaunchVar", 1);//green
    }
    String instruction = "waterTempVar.txt=\"" + static_cast<String>(ctof(waterTemp)) + "F\"";
    sendNextionMessage(instruction);
    instruction = "oilTempVar.txt=\"" + static_cast<String>(ctof(oilTemp))+"F\"";
    sendNextionMessage(instruction);
    instruction = "oilPressureVar.txt=\"" + static_cast<String>(oilPressure) + " PSI\"";
    sendNextionMessage(instruction);
    instruction = "voltageVar.txt=\"" + String(batteryVoltage, 1) + " V\"";
    sendNextionMessage(instruction);
    if(currentMessage != 0){
    instruction = "messageDriver.txt=\"" + String(currentMessage) + "\"";
    sendNextionMessage(instruction);
    }
    if(current_page == page::DIAGNOSTICS){
        instruction = "rpmVar.txt=\"" + String(round(engineRPM*1.4), 0) + "\"";
    }else{
        instruction = "rpm.txt=\"" + String(engineRPM, DEC) + "\"";
    }
    sendNextionMessage(instruction);
    if (gear == 0){
            instruction = "gearShiftVar.txt=\"" + String('N') + '\"';
            
    }else if(gear==-1){
        instruction = "gearShiftVar.txt=\"" + String('Q') + '\"';

    }else{
            instruction = "gearShiftVar.txt=\"" + String(gear) + '\"';
            
        }
    sendNextionMessage(instruction);
    instruction = "lambda.txt=\"" + String(lambda, 3) + " LA\"";
    sendNextionMessage(instruction);
    //no buttons or pumps yet will add if needed
}
//Sets the Water Temp on Screen
void NextionInterface::setWaterTemp(int value) {
    if(value != waterTemp){
        waterTemp = value;

        String instruction = "waterTempVar.txt=\"" + static_cast<String>(ctof(value)) + "F\"";
        sendNextionMessage(instruction);
    }
}
void NextionInterface::setLaunchIndicator(uint8_t value){

    if(launchState != value){
    launchState=value;
    if(launchState==0){
    setButtonImage("LaunchVar", 0);//red
    }else{
    setButtonImage("LaunchVar", 1);//green
    }
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

        String instruction = "voltageVar.txt=\"" + String(value, 1) + " V\"";
        sendNextionMessage(instruction);
    }
}
void NextionInterface::setDriver(int value){
    String instruction;
    if(Driver==-1){
        Driver=value;
        return;
    }
    
    if(value!=Driver){
        
        Driver=value;
        if (Driver==0){
            switchToDiagnostic();
            instruction = "driverVar.txt=\"Dnostic\"";
    
        }else if(Driver==1 /*Austin*/|| Driver==2 /*Sammy*/ || Driver==6 /*Sammy*/ || Driver==4 /*Shimmy*/){
            switchToDriverAust();
  
            instruction = "driverVar.txt=\"" + String(Drivers[value-1]) + "\"";
            
        }else if(Driver==5 /*Noah*/){
            switchToDriverNoah();
            instruction = "driverVar.txt=\"" + String(Drivers[value-1]) + "\"";

        }else{
            switchToDriver();
        //send driver name
        if(value-1<driverCount){
            instruction = "driverVar.txt=\"" + String(Drivers[value-1]) + "\"";
        }else{
            instruction = "driverVar.txt=\"No Driver Set\"";
        }
        }
        
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
        String instruction;
        if(current_page == page::DIAGNOSTICS){
        instruction = "rpmVar.txt=\"" + String(round(engineRPM*1.4), 0) + "\"";
        }else{
        instruction = "rpm.txt=\"" + String(engineRPM, DEC) + "\"";
        }
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
    value*=0.01;
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
    if(value != fuelPumpBool){
        fuelPumpBool = value;

        setButtonImage("fuelPumpVar", fuelPumpBool);
    }
}

void NextionInterface::setFanBool(bool value) {
    if(value != fanBool){
        fanBool = value;

        setButtonImage("fanVar", fanBool);
    }
}

void NextionInterface::setWaterPumpBool(bool value) {
    if(value != waterPumpBool){
        waterPumpBool = value;

        setButtonImage("waterPumpVar", waterPumpBool);
    }
}

void NextionInterface::setMLIBool(bool value) {
    if(value != MLIBool){
        MLIBool = value;

        setButtonImage("MLIVar", MLIBool);
    }
}

void NextionInterface::setMessageBool(bool value) {
    if(value != messageBool){
        messageBool = value;

        setButtonImage("MessageVar", messageBool);
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


void NextionInterface::switchToDiagnostic(){
    if(current_page != page::DIAGNOSTICS){
        sendNextionMessage("page DIAGNOSTICS");
        switchScreenUpdate();
        current_page = page::DIAGNOSTICS;
    }
}
void NextionInterface::switchToDriver(){
    if(current_page != page::DRIVER){
        sendNextionMessage("page DRIVER");
        switchScreenUpdate();
        current_page = page::DRIVER;
        }
        
}
void NextionInterface::switchToDriverNoah(){
    if(current_page != page::DRIVERnoah){
        sendNextionMessage("page DRIVERnoah");
        switchScreenUpdate();
        current_page = page::DRIVERnoah;
        }
        
}
void NextionInterface::switchToDriverAust(){
    if(current_page != page::DRIVERaust){
        sendNextionMessage("page DRIVERaust");
        switchScreenUpdate();
        current_page = page::DRIVERaust;
        }
        
}


void NextionInterface::switchToYippee() {
    
}
void NextionInterface::setScreenColor(int color){
    String instruction;
    switch (current_page) {
        case page::DIAGNOSTICS:{
        instruction="DIAGNOSTICS";
        break;
        }

        case page::DRIVER:{
        instruction="DRIVER";
        break;}


        case page::DRIVERaust:{
        instruction="DRIVERaust";
        break;}


        case page::DRIVERnoah:{
        instruction="DRIVERnoah";
        break;}

    }
    instruction+=".bco="+ String(color, DEC);
    sendNextionMessage(instruction);


}
void NextionInterface::showWarning(const String WARNING) {
    warningBool=true;
    if(warningMessage != WARNING){
        warningMessage=WARNING;
        if(current_page == page::DRIVERaust){
            setScreenColor(NextionWarningLightRed);

        }else{
        setScreenColor(NextionWarningDarkRed);
        }
 
        
   
        
        
        sendNextionMessage("vis warningLabel,1");
        //set the text
        String instruction = String("warningLabel.txt=\"") + WARNING + "\"";
        //current_page = page::WARNING;
        sendNextionMessage(instruction);

        

    }
}


page NextionInterface::getCurrentPage() {
    return current_page;
}