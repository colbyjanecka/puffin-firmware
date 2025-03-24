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

// MIDI.h
//  This file defines information and functions for the MIDI interface of the keyboard


//#define CS_USE_NIMBLE
#include <Seq.h>
#include <Control_Surface.h>
#include <MIDI_Interfaces/BluetoothMIDI_Interface.hpp>


// Instantiate a MIDI over BLE interface
BluetoothMIDI_Interface midi_ble;
// Instantiate a 5-pin DIN MIDI interface (on the TX and RX pins of Serial1)
HardwareSerialMIDI_Interface midi_ser {Serial0, MIDI_BAUD};
// Instantiate the pipe to connect the two interfaces
BidirectionalMIDI_Pipe pipes;

MIDI_Interface* midi_out = &midi_ble;

//MIDI TEST NOTE SENDING
// MIDI address of the note to send
const MIDIAddress noteAddress {MIDI_Notes::C[4], Channel_1};
// The velocity of the note events
const uint8_t velocity = 0x7F;

bool midiMode;
bool bridgeBLEtoSER = true;

bool activeNotes[12] = {false};

Clock clk;

Sequencer seq;

void setupSequencer(){
  seq.setDevice(midi_out);
  seq.setStep(0,PLAY_C,0x6F);
}

void setupBleSerPipe(){

}

void setMidiMode(bool on){
  EEPROM.begin(4);                                      //Begin EEPROM, allow us to store
  EEPROM.write(1,on);
  EEPROM.commit();
  esp_sleep_enable_timer_wakeup(1);
  esp_deep_sleep_start(); 
}

bool readMIDIModeEEPROM(){
  EEPROM.begin(4);                                      //Begin EEPROM, allow us to store
  midiMode = EEPROM.read(1);                     //Read selected address from storage
  return midiMode;
}
bool getMIDIMode(){
  return midiMode;
}
bool bleMIDIConnected(){
  return midi_ble.isConnected();
}

void playNote(int note, int vel){
  MIDIAddress noteAddress {note, Channel_1};
  //Serial.println("Playing note " + String(note));
  seq.midi_out->sendNoteOn(noteAddress, vel);
}

void stopNote(int note){
  MIDIAddress noteAddress {note, Channel_1};
  // The velocity of the note events
  const uint8_t velocity = 0x5F;
  //Serial.println("Stopping note " + String(note));
  seq.midi_out->sendNoteOff(noteAddress, velocity);
}

// Process the correct midi function triggered from MIDIMap.h
void processMIDIPress(int function){

  //Serial.println("MIDI FUNCTION: " + String(function));
  int currentBPM = clk.getBPM();

  // if function is between 300 and 320, play that respective note
  if(293 < function && function < 314){
    const uint8_t velocity = 0x5F;
    int noteToPlay = function-241;
    activeNotes[noteToPlay%12] = true;
    playNote(noteToPlay, velocity);
    if(seq.play){ seq.setStep(-1,function,velocity); }
  }
  switch (function){
    case SW_OUTPUT:
      if(seq.serialOutput){
        seq.switchOutput(&midi_ser);
        midi_out = &midi_ser;
      } else {
        seq.switchOutput(&midi_ble);
        midi_out = &midi_ble;
      }
      break;
    case CLOCK_START:
      //implement clock start
      break;
    case CLOCK_STOP:
      break;
    case BPM_INC:
      clk.setBPM(currentBPM + 1);
      break;
    case BPM_DEC:
      clk.setBPM(currentBPM - 1);
      break;
    case MIDI_MODE:
      setMidiMode(false);
      break;
    case PLAY_CHORD:
      playChord();
      break;
    case SEQ_CLEAR:
      seq.clear();
      //chord.constructAtRoot()
  }
}

void processMIDIRelease(int function){

  // if function is between 300 and 320, play that respective note
  if(293 < function && function < 314){
    const uint8_t velocity = 0x7F;
    int noteToPlay = function-241;
    activeNotes[noteToPlay%12] = false;
    stopNote(noteToPlay);
  }
}

int ticks = 0;
int lastClock = 0;
int currentClock = 0;
uint16_t now = 0;

void resetClock(){
  ticks = 0;
  lastClock = 0;
  currentClock = 0;
}

void handleClockFunction() {
  clk.ext = true;

  if(ticks < 24){  // per the MIDI specification, there are 24 ticks for every quarter note
      if(ticks == 0){
          lastClock = now;
      }
      ticks++;
  }
  else{
      ticks = 0;
      currentClock = now;
      if(lastClock > currentClock){ lastClock -= 8192; }
      float temp = currentClock - lastClock;
      float _BPM = 1001/temp*60;
      clk.setBPM((int)_BPM);
  }
}

// Custom MIDI callback that prints incoming messages.
struct MyMIDI_Callbacks : MIDI_Callbacks {
 
  // Callback for channel messages (notes, control change, pitch bend, etc.).
  void onChannelMessage(MIDI_Interface &, ChannelMessage cm) override {
    // Print the message
    Serial << F("Channel message: ")                                        //
           << hex << cm.header << ' ' << cm.data1 << ' ' << cm.data2 << dec //
           << F(" on cable ") << cm.cable.getOneBased() << endl;
  }

  // Callback for real-time messages
  void onRealTimeMessage(MIDI_Interface &, RealTimeMessage rt) override {
    // Print the message
    switch(rt.getMessageType()){
      case MIDIMessageType::TimingClock:
        // play next step in sequence
        if(seq.play){ seq.step(); }
        now = midi_ble.getTimestamp();
        handleClockFunction();
        break;
      case MIDIMessageType::Start:
        seq.start();
        resetClock();
        break;
      case MIDIMessageType::Continue:
        seq.play = true;
        break;
      case MIDIMessageType::Stop:
        seq.play = false;
        resetClock();
        seq.reset();
        break;
    }
  }
 
} callback;