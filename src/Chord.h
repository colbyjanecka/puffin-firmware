#include <iostream>
#include <string.h>
using namespace std;

/// Font data stored PER GLYPH
typedef struct {
    uint16_t chordmapOffset; ///< Pointer into GFXfont->bitmap
    String name;         ///< Bitmap dimensions in pixels
    uint8_t size;        ///< Bitmap dimensions in pixels
  } ChordData;

const ChordData Info[]  = {
  //  offset, name,           size
      {0,     "Major",        3},     // 00 Major
      {8,     "Minor",        3},     // 01 Minor
      {16,    "Augmented",    3},     // 02 Aug
  };

const uint8_t ChordIntervals[][8]  = {
    {0, 4, 7, 0, 0, 0, 0},
    {1, 3, 7, 0, 0, 0, 0},
    {0, 4, 7, 0, 0, 0, 0},
    {0, 4, 7, 0, 0, 0, 0},
};

/// Data stored for CHORD AS A WHOLE
typedef struct {
    uint8_t   *intervals;    ///< Chord Intervals, concatenated
    ChordData *data;         ///< Chord Data
  } ChordBank;



const ChordBank MyChords  = { (uint8_t *)ChordIntervals,
                            (ChordData *)Info };

// Approx. 1752 bytes

#define CHORD_MAJOR         00
#define CHORD_MINOR         01
#define CHORD_AUGMENTED     02


class MyChord {
  
    // Access specifier
public:

    // Construct Chord
    void constructAtRoot(int root, int type) {
        Serial.println("Playing " + MyChords.data[type].name);
        Serial.println("Size is " + String(MyChords.data[type].size));
        int size = MyChords.data[type].size;
        uint8_t * currentInterval = &MyChords.intervals[type];
        int out[size];
        //int intervals[8] = *MyChords.interval[type];
        for (int i = 0; i<size; i++){
            int note = currentInterval[i];
            Serial.println(String(note));
            //Serial.println(String((MyChords.intervals)));
            out[i] = root + (MyChords.intervals[type]);

        }
    }

private:

};