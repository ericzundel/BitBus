#include "BitBus.h"
#include "GamePad.h"

#include "Arduino.h"
#include "SoftwareSerial.h"

// Action Button Bit Reference
// For member action_button_value
#define START_BIT 0
#define SELECT_BIT 1
#define TRIANGLE_BIT 2
#define CIRCLE_BIT 3
#define CROSS_BIT 4
#define SQUARE_BIT 5

enum _INPUT_STATE {
  IS_START = 0,
  IS_ERROR,
  IS_WAITING_FOR_CONSTANT,
  IS_L_CONSTANT,
  IS_WAITING_FOR_L_DIGIT_1,
  IS_WAITING_FOR_L_DIGIT_2,
  IS_WAITING_FOR_L_DIGIT_3_OR_R,
  IS_WAITING_FOR_R_DIGIT_1,
  IS_WAITING_FOR_R_DIGIT_2,
  IS_WAITING_FOR_R_DIGIT_3_OR_F,
  IS_WAITING_FOR_F_DIGIT_1,
  IS_WAITING_FOR_F_DIGIT_2,
  IS_WAITING_FOR_F_DIGIT_3_OR_B,
  IS_WAITING_FOR_B_DIGIT_1,
  IS_WAITING_FOR_B_DIGIT_2,
  IS_WAITING_FOR_B_DIGIT_3,
  IS_MESSAGE_READY,
};


enum _MESSAGE_TYPE {
  MT_UNKNOWN = 0,
  MT_START,
  MT_SELECT,
  MT_TRIANGLE,
  MT_CIRCLE,
  MT_CROSS,
  MT_SQUARE,
  MT_UP,
  MT_DOWN,
  MT_LEFT,
  MT_RIGHT,
  MT_ANALOG_POSITION,
};


// Internal data structure to read analog joystick position.
// Data is accumulated here, and then when complete can be
// copied into the GamePad instance.
class _MessageBuffer
{
public:
  _MessageBuffer();
  processInput(int inputChar);

  int parseDigit1(int inputChar);
  int parseDigit2(int inputChar);  
  int parseLDigits(int inputChar);
  int parseRDigits(int inputChar);
  int parseBDigits(int inputChar);
  int parseFDigits(int inputChar);
  
  enum _MESSAGE_TYPE messageType;
  uint8_t leftValue;
  uint8_t rightValue;
  uint8_t frontValue;
  uint8_t backValue;
  
private:
  int _processStateEntry(struct state_entry *entry, int inputChar);
  uint8_t _parseDigits(int inputChar, uint8_t *valuePtr);  
  static bool _isDecDigit(int inputChar);
  static bool _isHexDigit(int inputChar);
  static uint8_t _asciiToInt(int inputChar);
  enum _INPUT_STATE inputState;
  char digitBuf[2];
  boolean isHex;
};

/**
 * Defines one entry in the state table
 */
struct state_entry {
  enum _INPUT_STATE currentState;  // Must match current state
  char inputChar; // Must match current input character. Special values are DIGIT regex ([0-9][A-F]) and ANY_CHAR regex (.)
  int (_MessageBuffer::*state_func)(int); // Optional function to call to process this state
  enum _INPUT_STATE nextState; // Next state transition
  enum _MESSAGE_TYPE messageType; // If anything other than MT_UNKNOWN, sets the message type
};

#define ANY_CHAR (char)0xFF
#define DIGIT    (char)0xFE

// TODO(ericzundel): When I wrote this big table, I assumed it would be stored in flash, but now I'm not so sure.
const struct state_entry stateTable[] = {
  {IS_START,                      ANY_CHAR, NULL,                         IS_WAITING_FOR_CONSTANT,        MT_UNKNOWN},
  {IS_WAITING_FOR_CONSTANT,       'L',      NULL,                         IS_WAITING_FOR_L_DIGIT_1,       MT_ANALOG_POSITION},
  {IS_WAITING_FOR_CONSTANT,       'R',      NULL,                         IS_WAITING_FOR_R_DIGIT_1,       MT_ANALOG_POSITION},
  {IS_WAITING_FOR_CONSTANT,       'B',      NULL,                         IS_WAITING_FOR_B_DIGIT_1,       MT_ANALOG_POSITION},
  {IS_WAITING_FOR_CONSTANT,       'F',      NULL,                         IS_WAITING_FOR_F_DIGIT_1,       MT_ANALOG_POSITION},
  {IS_WAITING_FOR_CONSTANT,       '?',      NULL,                         IS_MESSAGE_READY,               MT_START},
  {IS_WAITING_FOR_CONSTANT,       '?',      NULL,                         IS_MESSAGE_READY,               MT_SELECT},
  {IS_WAITING_FOR_CONSTANT,       'T',      NULL,                         IS_MESSAGE_READY,               MT_TRIANGLE},
  {IS_WAITING_FOR_CONSTANT,       '?',      NULL,                         IS_MESSAGE_READY,               MT_CIRCLE,},
  {IS_WAITING_FOR_CONSTANT,       '?',      NULL,                         IS_MESSAGE_READY,               MT_CROSS},
  {IS_WAITING_FOR_CONSTANT,       '?',      NULL,                         IS_MESSAGE_READY,               MT_SQUARE},
  {IS_WAITING_FOR_L_DIGIT_1,      DIGIT,    &_MessageBuffer::parseDigit1,  IS_WAITING_FOR_L_DIGIT_2,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_L_DIGIT_2,      DIGIT,    &_MessageBuffer::parseDigit2,  IS_WAITING_FOR_L_DIGIT_3_OR_R, MT_ANALOG_POSITION},
  {IS_WAITING_FOR_L_DIGIT_3_OR_R, 'R',      &_MessageBuffer::parseLDigits, IS_WAITING_FOR_R_DIGIT_1,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_L_DIGIT_3_OR_R, DIGIT,    &_MessageBuffer::parseLDigits, IS_WAITING_FOR_CONSTANT,       MT_ANALOG_POSITION},  
  {IS_WAITING_FOR_R_DIGIT_1,      DIGIT,    &_MessageBuffer::parseDigit1,  IS_WAITING_FOR_L_DIGIT_2,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_R_DIGIT_2,      DIGIT,    &_MessageBuffer::parseDigit2,  IS_WAITING_FOR_L_DIGIT_3_OR_R, MT_ANALOG_POSITION},
  {IS_WAITING_FOR_R_DIGIT_3_OR_F, 'F',      &_MessageBuffer::parseRDigits, IS_WAITING_FOR_F_DIGIT_1,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_R_DIGIT_3_OR_F, DIGIT,    &_MessageBuffer::parseRDigits, IS_WAITING_FOR_CONSTANT,       MT_ANALOG_POSITION},
  {IS_WAITING_FOR_F_DIGIT_1,      DIGIT,    &_MessageBuffer::parseDigit1,  IS_WAITING_FOR_F_DIGIT_2,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_F_DIGIT_2,      DIGIT,    &_MessageBuffer::parseDigit2,  IS_WAITING_FOR_F_DIGIT_3_OR_B, MT_ANALOG_POSITION},
  {IS_WAITING_FOR_F_DIGIT_3_OR_B, 'B',      &_MessageBuffer::parseFDigits, IS_WAITING_FOR_B_DIGIT_1,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_F_DIGIT_3_OR_B, DIGIT,    &_MessageBuffer::parseFDigits, IS_WAITING_FOR_CONSTANT,       MT_ANALOG_POSITION},  
  {IS_WAITING_FOR_B_DIGIT_1,      DIGIT,    &_MessageBuffer::parseDigit1,  IS_WAITING_FOR_B_DIGIT_2,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_B_DIGIT_2,      DIGIT,    &_MessageBuffer::parseDigit2,  IS_WAITING_FOR_B_DIGIT_3,      MT_ANALOG_POSITION},
  // TRICKY SPOT - end of input for B
};


// Singleton for other libraries to access this module
GamePadModule GamePad;

// Static data for this module
static _MessageBuffer message;

_MessageBuffer::_MessageBuffer()
{
  messageType = MT_UNKNOWN;
  leftValue = rightValue = frontValue = backValue = 0;
  inputState = IS_START;
  digitBuf[0] = digitBuf[1] = 0;
  isHex = false;

}

/**
 * Returns: 0 on success, 0xFF on failure
 */
int _MessageBuffer::parseDigit1(int inputChar) {
  this->digitBuf[0] = inputChar;
}

int _MessageBuffer::parseDigit2(int inputChar) {
  this->digitBuf[1] = inputChar;
}

static bool _MessageBuffer::_isDecDigit(int inputChar) {
  return inputChar <= '9' && inputChar >= '0';
}

static bool _MessageBuffer::_isHexDigit(int inputChar) {
  return _isDecDigit(inputChar) || inputChar <= 'F' && inputChar >= 'A';
}

/**
 * Returns 0-15 on success, 0xFF on failure
 */
static uint8_t _MessageBuffer::_asciiToInt(int nybble) {
  if (nybble <= '9' && nybble >= '0') {
    return nybble - '0';
  }
  if (nybble <= 'F' && nybble >= 'A') {
    return 10 + nybble - 'A';
  }
  return 0xFF;
}

/**
 * Parse the digits in the digits[] buffer plus possibly the current character.
 *
 * If two digits are encounted followed by a non digit, assume app is in HEX mode.
 * If three decimal digits are encountered, assume app is in DEC mode.
 *
 * inputChar: current character being processed
 * valuePtr: set to the parsed 2 or 3 digit value on success.
 *
 * Returns: 0 on success, non zero on failure
 */
uint8_t _MessageBuffer::_parseDigits(int inputChar, uint8_t *valuePtr) {
  if (_isDecDigit(inputChar)) {
    if (true == this->isHex) {
#if DEBUG
      Serial.print("ERROR: Got third decimal digit in hex mode");
#endif
      return GP_ERROR_EXPECTED_CONSTANT;
    }
    *valuePtr = (_asciiToInt(this->digitBuf[0]) * 100) + (_asciiToInt(this->digitBuf[1]) * 10) + _asciiToInt(inputChar);
  } else {
    this->isHex = true;
    *valuePtr = (_asciiToInt(this->digitBuf[0]) << 4) + _asciiToInt(this->digitBuf[1]);
  }
  return 0;
}

int _MessageBuffer::parseLDigits(int inputChar) {
  uint8_t value = 0;
  uint8_t result = _parseDigits(inputChar, &value);
  this->leftValue = value;
  return result;
}

int _MessageBuffer::parseRDigits(int inputChar) {
  uint8_t value = 0;
  uint8_t result = _parseDigits(inputChar, &value);
  this->rightValue = value;
  return result;
}

int _MessageBuffer::parseFDigits(int inputChar) {
  uint8_t value = 0;
  uint8_t result = _parseDigits(inputChar, &value);
  this->frontValue = value;
  return result;
}

int _MessageBuffer::parseBDigits(int inputChar) {
  uint8_t value;
  uint8_t result = _parseDigits(inputChar, &value);
  this->backValue = value;
  return result;
}

/**
 * Returns: 0 on success
 */
int _MessageBuffer::_processStateEntry(struct state_entry *entry, int inputChar) {
  if (entry->state_func) {
    int result = (this->*entry->state_func)(inputChar);
    if (!result) {
      return 0xFF;
    }
  }
  if (MT_UNKNOWN != entry->messageType) {
    this->messageType = entry->messageType;
  }
  this->inputState = entry->nextState;
  return 0;
}

/**
 * Process an input character.
 *
 * Data is accumulated until the input state is IS_MESSAGE_READY
 * 
 * Returns: 0 when a complete message is received.
 *          1 when still waiting on a complete message
 *          any other value is an error
 */
int _MessageBuffer::processInput(int inputChar) {
  bool isDigit = _isHexDigit(inputChar);
  
  // Go through the state table to find a matching state

  for (int i = 0; i < sizeof(stateTable)/sizeof(struct state_entry); i++) {
    bool found = false;
    if (this->inputState == stateTable[i].currentState) {
      if ((inputChar == stateTable[i].inputChar)
	   || (isDigit && DIGIT == stateTable[i].inputChar)
	  || (ANY_CHAR == stateTable[i].inputChar)) {
	_processStateEntry(&stateTable[i], inputChar);
      }
    }
  }
  
  if (this->inputState = IS_MESSAGE_READY) {
    return 0;
  }
  
  return 1;
}

// Class Constructor
GamePadModule::GamePadModule() {
  posLeft = posRight = posFront = posBack = 0;
  actionButtons = 0;
}

bool GamePadModule::isUpPressed()
{
  return false;
}

bool GamePadModule::isDownPressed()
{
  return false;
}

bool GamePadModule::isLeftPressed()
{
  return false;
}

bool GamePadModule::isRightPressed()
{
  return false;
}

//Orange Button Checker
bool GamePadModule::isStartPressed()
{
  return !!(this->actionButtons & (1<<START_BIT));
}

//Red Button Checker 
bool GamePadModule::isSelectPressed()
{	
  return !!(this->actionButtons & (1<<SELECT_BIT));
}

//Green Button Checker 
bool GamePadModule::isTrianglePressed()
{
  return !!(this->actionButtons & (1<<TRIANGLE_BIT));
}

//Blue Button Checker
bool GamePadModule::isCirclePressed()
{	
  return !!(this->actionButtons & (1<<CIRCLE_BIT));
}

bool GamePadModule::isCrossPressed()
{
  return !!(this->actionButtons & (1<<CROSS_BIT));
}

bool GamePadModule::isSquarePressed()
{
  return !!(this->actionButtons & (1<<SQUARE_BIT));
}

/*-------------------------------------------------------------------*/
#if USE_SYNCRONOUS_READ

uint8_t asciiToInt(uint8_t nybble) {
  if (nybble <= '9' && nybble >= '0') {
    return nybble - '0';
  }
  if (nybble <= 'F' && nybble >= 'A') {
    return 10 + nybble - 'A';
  }
  return 0xFF;
}

char readByte() {
  // Block and wait for the next character
  while (btSerial.available() <= 0) {
    // do nothing
  }
  return btSerial.read();
}

uint8_t readThreeDigitAscii() {
  uint8_t hundreds = asciiToInt(readByte());
  uint8_t tens = asciiToInt(readByte());
  uint8_t ones = asciiToInt(readByte());
  return (hundreds * 100) + (tens * 10) + ones;
}

/**
   A synchronous version of reading an analog joystick message. 

   This was test code, just here for reference.

   Read the 12 character message from the bitbus app when
   the controller is set to analog mode with decimal input.
   LXXXRXXXFXXXBXXX
   Where L,R,F,B are the ascii values for L,R,F,B and
   X is the ascii value for '0'-'F'
   Return: 0 on success, nonzero on failure
*/
int8_t readAnalogMessageSyncronous() {
  const byte messageLength = 12;
  char c = readByte();
  while (c != 'L') {
    c = readByte();
  }
  
  // Next two characters define the left value
  analogMessage.leftValue = readThreeDigitAscii();

  c = readByte();
  if (c != 'R') {
    Serial.print("ERROR: Expected 'R' Got: ");
    Serial.println(c);
    return -1;
  }
  analogMessage.rightValue = readThreeDigitAscii();

  c = readByte();
  if (c != 'F') {
    Serial.println("ERROR: Expected 'F' Got: ");
    Serial.println(c);
    return -1;
  }

  analogMessage.frontValue = readThreeDigitAscii();

  c = readByte();
  if (c != 'B') {
    Serial.println("ERROR: Expected 'B' Got: ");
    Serial.println(c);
    return -1;
  }
  analogMessage.backValue = readThreeDigitAscii();

  return 0;
}
#endif // USE_SYNCHRONOUS_READ
/*-------------------------------------------------------------------*/


// Handle the work of processing GamePad specific input.
int GamePadModule::_processInput(int inputChar)
{
#if DEBUG
  Serial.print("GPM Input: '");
  Serial.print(inputChar);
  Serial.println("'");
#endif
  
  if (!message.processInput(inputChar)) {
    // Got a new message
    switch (message.messageType) {
    case MT_UNKNOWN:
    default:
#if DEBUG
      Serial.print("Error: Unhandled Message Type ");
      Serial.println(message.messageType);
#endif
      return GP_ERROR_UNHANDLED_MESSAGE_TYPE;
      break;
    }
  }
  
}
