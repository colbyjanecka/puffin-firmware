#include <Matrix.h>
#include <MIDIMap.h>
#include <PuffinBrain.h>
#include <Control_Surface.h>
//#include "Seq.h"
#include <MIDI.h>
#include <BleKeyboard.h>  
#include <EEPROM.h>
#include <MIDI_Interfaces/BluetoothMIDI_Interface.hpp>
#include "Graphics.h"
#include <Adafruit_NeoPixel.h>
#include <Wire.h>


// Define LED Array

Adafruit_NeoPixel pixels(51, 34, NEO_GRB + NEO_KHZ800);
int LED_DELAY;
bool ledEnabled = false;

bool prod = false;

// Screen refresh counter
int refresh = 0;
int screenSpeed = 10000;
int extClockCount = 0;

//Setup, onces once on boot
void setup() {

  readMacEEPROM(readMIDIModeEEPROM());
  
  Wire.begin();
  boardStart();
  clk.setBPM(120);

  // Start Serial Output for monitoring/debugging
  Serial.begin(115200);
  // Allow for native usb to kick in
  delay(200);

  // Display Setup
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_I2C_ADDR);
  display.setRotation(2);
  //startupAnimation();
  display.clearDisplay();
  mainScreenUpdate();
  Kbd.setBatteryLevel(updateBattLevel());

  // Keyboard Matrix Setup
  //Initialize col output pins and set default state
  for(int i = 0; i < ColCount; i++)
  {
    pinMode(Cols[i],OUTPUT);  
    digitalWrite(Cols[i],LOW);
  }
  //Initialize row input pins
  for(int i = 0; i < RowCount; i++)
  {
    pinMode(Rows[i],INPUT_PULLDOWN);
  }

  // Initalize LEDS
  if (ledEnabled){
    //LED Setup
    pixels.begin();
    pixels.show();
    pixels.setBrightness(3);
    LED_DELAY = 100;
  }

  // check and start MIDI mode if necessary
  if(midiMode){

    setupSequencer();
    // MIDI 
    // Change the name of the BLE device (must be done before initializing it)
    std::string newName = std::string("Puffin MIDI []").insert(13, std::string(1, ('A' + getMacDevice())));
    midi_ble.setName(newName.c_str()); //midi_ble.setName("Puffin MIDI []");
    
    // Manually route MIDI input from the serial interface to the BLE interface,
    // and the MIDI input from the BLE interface to the serial interface
    if(bridgeBLEtoSER) {midi_ser | pipes | midi_ble;}
    // Initialize the MIDI interfaces
    MIDI_Interface::beginAll();
    midi_ble.setCallbacks(callback); // Attach the custom callback
    //midi_ble.begin();
    Serial.println("Started midi_ble");

  // Start Keyboard
  } else {
    //Begin bluetooth keyboard, without keyboard will not appear or connect
    Kbd.begin();
  }

}

// basic test animation for LEDS
int NUM = 51;
void animation1()
{
  for(int i=0;i<NUM;i++)
  {
    pixels.setPixelColor(i, pixels.Color(255,0,0));
    pixels.show();
    delay(100);
  }
  for(int i=NUM;i>=0;i--)
  {
    pixels.setPixelColor(i, pixels.Color(0,255,0));
    pixels.show();
    delay(100);
  }
  for(int i=0;i<NUM;i++)
  {
    pixels.setPixelColor(i, pixels.Color(0,255,255));
    pixels.show();
    delay(100);
  }
  for(int i=NUM;i>=0;i--)
  {
    pixels.setPixelColor(i, pixels.Color(255,255,0));
    pixels.show();
    delay(100);
  }
}

int ColCnt = 0;  //Keep track of scanned row - needs to be outside of the void loop so it isn't reset
int LayerCnt = 0;
int color = 0;
int pixel = 0;
int LED = 0;

//Loops to iterate through all functions
void loop() {

  //onboardLED(color);

  // Midi Loop Code
  if (getMIDIMode()){
    //if(bleMIDIConnected()){ getClock(); }
    // Continuously poll all interfaces and route the traffic between them
    MIDI_Interface::updateAll();
}

  //Check if the keyboard is connected, if so, scan the matrix
  if(true) //Kbd.isConnected()) // || midi_ble.isConnected())
  {
    //Initialize new Column to scan
    digitalWrite(Cols[ColCnt],HIGH);

    //Check rows
    int RowCnt = 0;

    //Repeat until all columns are scanned
    while(RowCnt <= (RowCount - 1))
    {
      //Check state of current position, and make sure it was previously off
      if(digitalRead( Rows[RowCnt] ) == HIGH && PressedCheck[LayerCnt][RowCnt][ColCnt] == OFF)
      {
        if(midiMode){
          processMIDIPress(MIDIMap[LayerCnt][RowCnt][ColCnt]);
        }
        //Switch based on the key pressed, allows for unique functions other than alphanumerics
        switch(Layout[LayerCnt][RowCnt][ColCnt])
        {
          case 0:
          case 1:
          case 2:
          case FUNCTION_SW:
            //PressedCheck.fill(OFF);
            Kbd.releaseAll();
            LayerCnt++;
            break;
          case FUNCTION_SW2:
            //PressedCheck.fill(OFF);
            Kbd.releaseAll();
            LayerCnt++;
            break;
          case NULL_CON:
            break;
          case NEXT:
            Kbd.press(KEY_MEDIA_NEXT_TRACK);
            Kbd.releaseAll();
            break;
          case PREV:
            Kbd.press(KEY_MEDIA_PREVIOUS_TRACK);
            break;
          case LED_B0:
            pixels.setBrightness(0);
            break;
          case LED_B1:
            pixels.setBrightness(5);
            LED_DELAY = 500;
            break;
          case LED_B2:
            LED_DELAY = 20;
            pixels.setBrightness(40);
            break;
          case CHANGE_ID0:
            // Change MAC Address to talk to different device
            storeMacEEPROM(0);
            break;
          case CHANGE_ID1:
            // Change MAC Address to talk to different device
            storeMacEEPROM(1);
            break;
          case CHANGE_ID2:
            // Change MAC Address to talk to different device
            storeMacEEPROM(2);
            break;
          case CHANGE_ID3:
            // Change MAC Address to talk to different device
            storeMacEEPROM(3);
            break;
          case MIDI_MODE:
            setMidiMode(true);
            break;
          case KEY_SS:
            Kbd.press(KEY_LEFT_GUI);
            Kbd.press(KEY_LEFT_SHIFT);
            Kbd.press('5');
            Kbd.releaseAll();

          //if spacebar was tapped, reset counter.
          case ' ':
            spaceHit();
            firstSpace = false;
            Kbd.press( Layout[LayerCnt][RowCnt][ColCnt] );
            break;
          default:
            firstSpace = true;
            Kbd.press( Layout[LayerCnt][RowCnt][ColCnt] );
        }
        //Assign the current key as ON, so it doesn't constantly press the button
        PressedCheck[LayerCnt][RowCnt][ColCnt] = ON;   
        Serial.println( Layout[LayerCnt][RowCnt][ColCnt] );
        //midi_ble.sendNoteOn(noteAddress, velocity);         // send a note on event
      }

      //Otherwise, check if the switch was released
      else if(digitalRead( Rows[RowCnt] ) == LOW && PressedCheck[LayerCnt][RowCnt][ColCnt] == ON)
      {
        //Switch based on the switch released
        switch(Layout[LayerCnt][RowCnt][ColCnt])
        {
          //Nothing for the tactile switch
          case 1:
          case 2:
          case 0:
            break;
          case 3:
            Kbd.release(KEY_MEDIA_PLAY_PAUSE);
            break;
          case FUNCTION_SW:
            PressedCheck[LayerCnt][RowCnt][ColCnt] = OFF;
            if(LayerCnt > 0)
            {
              LayerCnt--;
            }
            Kbd.release(KEY_MEDIA_PLAY_PAUSE);            
            Kbd.release(KEY_MEDIA_PREVIOUS_TRACK);
            Kbd.release(KEY_MEDIA_NEXT_TRACK);
            Kbd.releaseAll();
            break;
          case FUNCTION_SW2:
            PressedCheck[LayerCnt][RowCnt][ColCnt] = OFF;
            if(LayerCnt > 1)
            {
              LayerCnt--;
            }
            Kbd.releaseAll();
            break;
          case NULL_CON:
            PressedCheck[LayerCnt][RowCnt][ColCnt] = OFF;
            if(LayerCnt > 0)
            {
              LayerCnt--;
            }
            Kbd.release(KEY_MEDIA_PLAY_PAUSE);            
            Kbd.release(KEY_MEDIA_PREVIOUS_TRACK);
            Kbd.release(KEY_MEDIA_NEXT_TRACK);
            Kbd.releaseAll();
            break;
          case NEXT:
            Kbd.release(KEY_MEDIA_NEXT_TRACK);
            break;
          case PREV:
            Kbd.release(KEY_MEDIA_PREVIOUS_TRACK);
            break;
          case MIDI_TOGGLE:
            break;

          //Release all other keys on the keyboard
          default:
            Kbd.release( Layout[LayerCnt][RowCnt][ColCnt] );
          if(midiMode){
            processMIDIRelease(MIDIMap[LayerCnt][RowCnt][ColCnt]);
          }
        }
        //Let the keyboard know it's off, and to not constantly release keys that aren't released
        PressedCheck[LayerCnt][RowCnt][ColCnt] = OFF;

      }
      //Increase row to scan
      RowCnt++;
    }
    //Reset back to original column to scan
    digitalWrite(Cols[ColCnt],LOW);
    //Increase column outputted
    ColCnt++;
    //Loop back to original column if out of the number of columns
    if(ColCnt >= (ColCount))
    {
      ColCnt = 0;
    }
  }
  
  //Delay so it's not too fast
  if(not midiMode){ delay(1); screenSpeed = 10000; }

  EVERY_N_MILLIS(100){
    mainScreenUpdate();
  }

  // Update MIDI clock and output pulse if needed
  //if (clockUpdate()) { midi_ser.sendRealTime(clock); };

  // Update battery level every 30 seconds
  EVERY_N_SECONDS(30){
    Kbd.setBatteryLevel(updateBattLevel());
  }
  

  // Update WPM every 30 seconds
  if(not midiMode){
    EVERY_N_MILLISECONDS(200){
      updateWPM(10);
    }
  }



  if(ledEnabled){
    
    if(LED > LED_DELAY){
      color++;
      if (pixel >= 50){
        pixel = 0;
      } else {
        pixel += 1;
      }
      //pixels.setPixelColor(i, 255,0,0);
      pixels.setPixelColor(pixel, UMS3::colorWheel(color));
      pixels.show();
      LED = 0;
    } else {
      LED += 1;
    }
  }
}