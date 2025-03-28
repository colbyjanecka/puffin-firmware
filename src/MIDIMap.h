#include <Connections.h>

int MIDIMap[NumLayers][RowCount][ColCount] = 
{
  //  Layer 0 TEST //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  {
    {'-',       '-',        '-',        CLOCK_START,        '2',       '3',       '4',          '5',       '6',          '-',       '-',        '-'},
    {'-',       'q',        'w',        'e',        'r',       't',       'y',          'u',       'i',          'o',       'p',        '-'},
    {'-',       PLAY_Gbl,   PLAY_Abl,   PLAY_Bbl,   'f',       PLAY_Db,    PLAY_Eb,     'j',        PLAY_Gb,     PLAY_Ab,   PLAY_Bb,    SEQ_CLEAR},
    {PLAY_Fl,   PLAY_Gl,    PLAY_Al,    PLAY_Bl,    PLAY_C,    PLAY_D,     PLAY_E,      PLAY_F,     PLAY_G,      PLAY_A,    PLAY_B,     PLAY_Ch},
    {PLAY_CHORD,       '-',        '-',       '-',         ' ',       '-',        ' ',         '-',        FUNCTION_SW, '-',       SW_OUTPUT,  '-'}
  },
  //  Layer 1 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  {
    {'-',              '-',        '-',       MIDI_MODE,     BPM_DEC,     CHANGE_ID0,    CHANGE_ID1,CHANGE_ID2,CHANGE_ID3, '-',        '-',       '-'},
    {'`',              '1',        '2',        '3',       '4',        '5',       '6',       '7',       '8',       '9',       '0',       '-'},
    {'-',    'a',        's',       '[',        '(',       '{',        '}',       ')',        ']',       '-',        '\'',       '-'},
    {'-',   'z',        'x',       'c',        'v',       'b',        'n',       'm',        ',',       '.',        '/',       '-'},
    {'-',  '-', '-','-',      ' ',       '-',        ' ',       '-', '-', '-', '-', '-'}
  },
  //  Layer 2 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  {
    {MIDI_PLAY,       'L',        'Z'},
    {':',       MIDI_TOGGLE,         'Q'}
  },
  //  Layer 3 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  {
    {MIDI_PLAY,       'L',        'Z'},
    {':',       MIDI_OFF,         'Q'}
  }
};