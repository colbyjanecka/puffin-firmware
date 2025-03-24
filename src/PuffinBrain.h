
/* __/\\\\\\\\\\\\\________________________/\\\\\________/\\\\\_____________________         */
/*  _\/\\\/////////\\\____________________/\\\///_______/\\\///______________________        */
/*  _\/\\\_______\/\\\___________________/\\\__________/\\\_______/\\\_______________        */
/*  _\/\\\\\\\\\\\\\/___/\\\____/\\\__/\\\\\\\\\____/\\\\\\\\\___\///___/\\/\\\\\\___        */
/*   _\/\\\/////////____\/\\\___\/\\\_\////\\\//____\////\\\//_____/\\\_\/\\\////\\\__       */
/*    _\/\\\_____________\/\\\___\/\\\____\/\\\_________\/\\\______\/\\\_\/\\\__\//\\\_      */
/*     _\/\\\_____________\/\\\___\/\\\____\/\\\_________\/\\\______\/\\\_\/\\\___\/\\\_     */
/*      _\/\\\_____________\//\\\\\\\\\_____\/\\\_________\/\\\______\/\\\_\/\\\___\/\\\_    */
/*       _\///_______________\/////////______\///__________\///_______\///__\///____\///__   */

//  Puffin Keyboard/MIDI Sequencer
//  Designed by Colby Janecka in Sunny California


//TODO: set up to use NIMBLE to save system resources
//#define USE_NIMBLE // Decreases system resource usage
#include <BleKeyboard.h>
#include <EEPROM.h>
#include <FastLED.h>
#include <UMS3.h>
#define Selectable SelectableForLED

// BOARD SETUP

UMS3 ums3;
int batteryLevel;
int macAddress;

void boardStart(void){
    // Initialize all board peripherals, call this first
  ums3.begin();
  //ums3.setLDO2Power(0);

  // Brightness is 0-255. We set it to 1/3 brightness here
  //ums3.setPixelBrightness(255 / 3);
  // Enable the power to the RGB LED.
  // Off by default so it doesn't use current when the LED is not required.
  ums3.setPixelPower(false);
}

void onboardLED(int color) {
    // colorWheel cycles red, orange, ..., back to red at 256
  ums3.setPixelColor(UMS3::colorWheel(color));
}

//Setup memory for LEDs
CRGB leds[NumLeds];

// KEYBOARD
BleKeyboard Kbd("Puffin","ManiaSynths",100);

bool isConnectedBT(){
  return Kbd.isConnected();
}

uint8_t gHue = 0; //Keep track of the colour wanted

//  Code to "store" devices, so that the keyboard can switch connections on the fly ////////////////////////////////////////////////////////////////////////
//Adapted from : https://github.com/Cemu0/ESP32_USBHOST_TO_BLE_KEYBOARD/blob/main/src/USBHIDBootBLEKbd.cpp
//Primarily stores the selected MAC address in EEPROM storage
uint8_t MACAddress[][6] = 
{
  {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED},
  {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE},
  {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF},
  {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF0},
  {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xDD},
  {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xDE},
  {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xDF},
  {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xD0},
  {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF5},
  
};

// Debug function for printing current mac address to serial output
void printMACAddress(uint8_t newMAC[6]){
  // Variable to store the MAC address
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_BT);
  Serial.print("Bluetooth MAC: ");
  for (int i = 0; i < 5; i++) {
    Serial.printf("%02X:", baseMac[i]);
  }
  Serial.printf("%02X\n", baseMac[5]);

  // TEST PRINT DESIRED MAC
  Serial.print("DESIRED MAC: ");
  for (int i = 0; i < 5; i++) {
    Serial.printf("%02X:", newMAC[i]);
  }
  Serial.printf("%02X\n", newMAC[5]);
}

//Allows the keyboard to connect to multiple devices, and "remembers" what device it was connected to
void readMacEEPROM(bool midiOn){  

  //Adapted from GitHub, alongside the MAC address code
  EEPROM.begin(4);                                    //Begin EEPROM, allow us to store
  macAddress = EEPROM.read(0);                         //Read selected address from storage
  if(midiOn) { macAddress+=4; }
  Serial.println("Connecting using MAC address for device " + String(macAddress));
  esp_base_mac_addr_set(&MACAddress[macAddress][0]);   //Set MAC address based on that stored value

  std::string newName = std::string("Puffin []").insert(8, std::string(1, ('A' + macAddress)));
  
  Kbd.setName(newName);
}

int getMacDevice(){
  return macAddress;
}

//Basically just change the selected ID and reset - MAC address can only be changed before the keyboard start, so write to store selection, until changed again
  //Take in device number, and set the EEPROM to the selected - selects what address to shift to, instead of iterating to that address
void storeMacEEPROM(int DevNum)
{
    int currentDevice = EEPROM.read(0);
    //Write and commit to storage, reset ESP 32
    EEPROM.write(0,DevNum);
    EEPROM.commit();
    esp_sleep_enable_timer_wakeup(1);
    esp_deep_sleep_start(); 
}

//LED Function, to be expanded upon to add more LED functionality later
void LedMode (int Select)
{
  switch(Select)
  {
    case 1:
      gHue++;
      fill_rainbow(leds, NumLeds, gHue, 7);
      break;
    case 2:
      fill_solid(leds, NumLeds, CRGB::Cyan);      
      break;
    default:
      break;
  }
  FastLED.show();
}


// BATTERY
int battPin = 10;
int battRead = 0;
float battMax = 4.05; // Set max battery voltage here (fully charged)
float battMin = 3;   // Set min battery voltage (should be 3.0 volts)

int updateBattLevel ()
{
  //return analogRead(battPin);
  float battVoltage = ums3.getBatteryVoltage();
  batteryLevel = (battVoltage - battMin)/(battMax-battMin) * 100;
  Serial.println("battLevel: " + String(batteryLevel));
  return batteryLevel;
}

int getBattLevel()
{
  return batteryLevel;
}

float getBattVolts ()
{
  //return analogRead(battPin);
  float battery = ums3.getBatteryVoltage();
  //Serial.println(battery);
  return battery;
}

bool getUSBStatus ()
{
  return ums3.getVbusPresent();
}

// TIMER USED TO CALCULATE WPM

const int maxReadings = 20;   // HOW MANY WORDS TO USE FOR AVG -  change for 

int startTime = 0, stopTime = 0, avgWPM = 0, readings = 1;
double avgWordTime = 0;
int totalWordTime = 0;
bool firstSpace = true;

int writePos = 0;

// calculates the time since the last function call 
int getLastWordTime() {
  readings = min((readings + 1), maxReadings);  // Increase num of readings to avg
  stopTime = millis();
  int elapsed = (stopTime - startTime);
  startTime = stopTime;
  if(readings != maxReadings){ 
    totalWordTime += elapsed; 
  } else {
    totalWordTime = totalWordTime-(totalWordTime/readings) + elapsed;
  }
  //double wpm = int(1/((double)elapsed/60000));
  Serial.println("Numb of readings averaged: " + String(readings));
  Serial.println("Total Time: " + String(totalWordTime));

  return elapsed;
}

void spaceHit(){
  if(firstSpace){ getLastWordTime(); }
}

int getWPM(){
  return avgWPM; 
}


// WPM - every word, store latest wpm in array.  every 5 seconds, average the last 5 readings and update avgWPM.
// bool spacePressed - whether or not space has been pressed
void updateWPM(int maxReadings){
  //append a larger number to keep decreasing 
  if((millis() - startTime)>4000){
    totalWordTime = 1;
    //if(avgWPM == 0){ totalWordTime = 0; }
    readings = max((readings - 1), 1);            // Decrease num of readings to avg
    avgWPM = 0;
  } else {

    avgWordTime = totalWordTime/readings;
  
    // convert average word time to WPM
    avgWPM = int(1/(avgWordTime/60000));
    

  }
}