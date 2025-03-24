#include <Chord.h>

class Clock {
    public:
    bool ext = false;

    long bpm = 120;
    long tempo = 1000/(bpm/60);

    long prevmillis = 0;
    long interval = tempo/24;

    void setBPM(long bpmNew){
        bpm = (long)bpmNew;
        tempo = 1000/(bpm/60);
    }

    int getBPM(){
        return bpm;
    }

    bool clockUpdate(){
        unsigned long currentMillis = millis();
        if(currentMillis - prevmillis > interval) {
            // save the last time.
            prevmillis = currentMillis;
            return true;
        }
        return false;
    }
};

class Sequencer {
    public:

    bool play = false;
    bool serialOutput = false;

    int length = 96;
    int notes[96];

    int s;
    MIDI_Interface* midi_out;

    void setDevice(MIDI_Interface* midi_ptr){
            midi_out = midi_ptr;
    }

    void send(int note, bool on){
        MIDIAddress noteAddress {note, Channel_1};
        if(on){
            midi_out->sendNoteOn(noteAddress, 0x5F); 
        } else {
            midi_out->sendNoteOff(noteAddress, 0x5F); 
        }
    }

    // function used to trigger step in sequence
    void step() {
        int note = notes[s];
        send(notes[s-1], false);
        if (note > 40 && play){
            send(note, true);
            Serial.println("Sending note " + String(note));
        }
        if(s < length-1) { s += 1; } else { s = 0; }
    }
     void setStep(int st, int data, int velocity){
        if (st == -1){ st = s; }
        velocity = 0x5F;
        notes[st] = data-241;
     }
     void clear(){
        notes[s] = 0;
     }
     void reset(){
        s = 0;
     }
     void start(){
        s = 0;
        play = true;
        step();
     }
     void switchOutput(MIDI_Interface* midi_ptr){
        serialOutput = not serialOutput;
        midi_out = midi_ptr;
    }
};

void playChord(){
    MyChord chord;
    chord.constructAtRoot(60, CHORD_MAJOR);
}