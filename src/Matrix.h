#include <Connections.h>
#include <BleKeyboard.h>

/* Swapped
//Rows to scan, and Columns to read (definitions in SanctuaryHardware
short Rows[RowCount] = {Row1, Row2, Row3, Row4, Row5};
short Cols[ColCount] = {Col1, Col2, Col3, Col4, Col5, Col6, Col7, Col8, Col9, Col10, Col11, Col12};

//  Layer 1 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Layout[NumLayers][RowCount][ColCount] = 
{
  {
    {'K',       'S'},
    {'I',       '?'},
    {'S',       FUNCTION_SW}
  },
  //  Layer 2 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  {
    {'A',       'B'},
    {'C',       'D'},
    {'D',       'E'}
  }
};
*/

//Rows to scan, and Columns to read (definitions in SanctuaryHardware
short Rows[RowCount] = {Row1, Row2, Row3, Row4, Row5};
short Cols[ColCount] = {Col1, Col2, Col3, Col4, Col5, Col6, Col7, Col8, Col9, Col10, Col11, Col12};

int Layout[NumLayers][RowCount][ColCount] = 
{
  //  Layer 0 TEST //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  {
    {'-',              '-',           '-',          '1',        '2',       '3',        '4',       '5',        '6',          '-',            '-',            '-'},
    {KEY_TAB,          'q',           'w',          'e',        'r',       't',       'y',        'u',       'i',           'o',            'p',            KEY_BACKSPACE},
    {KEY_CAPS_LOCK,    'a',           's',          'd',        'f',       'g',        'h',       'j',        'k',          'l',            ';',            KEY_RETURN},
    {KEY_LEFT_SHIFT,   'z',           'x',          'c',        'v',       'b',        'n',       'm',        ',',          '.',            KEY_UP_ARROW,   '/'},
    {KEY_LEFT_CTRL,    KEY_LEFT_ALT,  KEY_LEFT_GUI, '-',        ' ',       '-',        ' ',       '-',        FUNCTION_SW,  KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW}
  },
  //  Layer 1 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  {
    {'-',               '-',          '-',          MIDI_MODE,  LED_B1,    CHANGE_ID0,  CHANGE_ID1, CHANGE_ID2, CHANGE_ID3,   '-',            '-',            '-'},
    {'`',               '1',          '2',          '3',        '4',       '5',         '6',        '7',        '8',          '9',            '0',            KEY_BACKSPACE},
    {KEY_CAPS_LOCK,     'a',          's',          '[',        '(',       '{',         '}',        ')',        ']',          '-',            '\'',           KEY_RETURN},
    {KEY_LEFT_SHIFT,    'z',          'x',          'c',        'v',       'b',         'n',        'm',        '-',          '=',            KEY_UP_ARROW,   '\\'},
    {KEY_LEFT_CTRL,     KEY_LEFT_ALT, KEY_LEFT_GUI, '-',        ' ',       '-',         ' ',        '-',        FUNCTION_SW,  KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW}
  }
};


//Array to check if a key is currently pressed - Initiate everything to 0
short PressedCheck[NumLayers][RowCount][ColCount] = { OFF };