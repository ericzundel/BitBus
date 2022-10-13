/*
 * GamePadModule: Implements parsing of message from the BitBus Controller in Analog Mode
 */
#include "BitBus.h"
#include "BitBusUtil.h"
#include "GamePad.h"
#include "MessageBuffer.h"

#include "Arduino.h"
#include "SoftwareSerial.h"

#define DEBUG 0

// Action Button Bit Reference
// For member action_button_value
#define START_BIT 0
#define SELECT_BIT 1

#define BUTTON_A_BIT 2
#define BUTTON_B_BIT 3
#define BUTTON_X_BIT 4
#define BUTTON_Y_BIT 5

// For compatibility with Dabble
#define TRIANGLE_BIT BUTTON_B_BIT
#define CIRCLE_BIT BUTTON_Y_BIT
#define CROSS_BIT BUTTON_X_BIT
#define SQUARE_BIT BUTTON_A_BIT

#define UP_BIT 0
#define DOWN_BIT 1
#define LEFT_BIT 2
#define RIGHT_BIT 3


/**
 * Defines one entry in the state table.
 *
 * The enum values are stored as uint8_t to save space in the table
 * and free up some RAM.
 *
 */
struct state_entry {
  uint8_t currentState;  // enum _INPUT_STATE: Must match current state
  uint8_t inputChar;     // Must match current input character. Special values are DIGIT regex ([0-9][A-F]) and ANY_CHAR regex (.)
  int (_MessageBuffer::*state_func)(int, enum _INPUT_STATE *); // Optional function to call to process this state
  uint8_t nextState;     // enum _INPUT_STATE: Next state transition
  uint8_t messageType;   // enum _MESSAGE_TYPE: If anything other than MT_UNKNOWN, sets the message type
};

// Sentinel values to represent multiple characters
#define ANY_CHAR (uint8_t)0xFF
#define DIGIT    (uint8_t)0xFE

// State Table stored in flash memory
// NB(ericzundel): You must copy an entry out with memcpy_P() to a local buffer
const struct state_entry stateTable[] PROGMEM = {
  {IS_START,                      'S',      NULL,                             IS_MESSAGE_READY,              MT_START_BUTTON},
  {IS_START,                      'C',      NULL,                             IS_MESSAGE_READY,              MT_SELECT},
  {IS_START,                      'A',      NULL,                             IS_MESSAGE_READY,              MT_BUTTON_A},
  {IS_START,                      'B',      NULL,                             IS_MESSAGE_READY,              MT_BUTTON_B},
  {IS_START,                      'X',      NULL,                             IS_MESSAGE_READY,              MT_BUTTON_X},
  {IS_START,                      'Y',      NULL,                             IS_MESSAGE_READY,              MT_BUTTON_Y},
  {IS_START,                      'L',      NULL,                             IS_WAITING_FOR_L_DIGIT_1,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_R,              'R',      NULL,                             IS_WAITING_FOR_R_DIGIT_1,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_B,              'B',      NULL,                             IS_WAITING_FOR_B_DIGIT_1,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_F,              'F',      NULL,                             IS_WAITING_FOR_F_DIGIT_1,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_L_DIGIT_1,      DIGIT,    &_MessageBuffer::parseDigit1,     IS_WAITING_FOR_L_DIGIT_2,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_L_DIGIT_2,      DIGIT,    &_MessageBuffer::parseDigit2,     IS_WAITING_FOR_L_DIGIT_3_OR_R, MT_ANALOG_POSITION},
  {IS_WAITING_FOR_L_DIGIT_3_OR_R, 'R',      &_MessageBuffer::parseLHexDigits, IS_WAITING_FOR_R_DIGIT_1,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_L_DIGIT_3_OR_R, DIGIT,    &_MessageBuffer::parseLDecDigits, IS_WAITING_FOR_R,              MT_ANALOG_POSITION},
  {IS_WAITING_FOR_R_DIGIT_1,      DIGIT,    &_MessageBuffer::parseDigit1,     IS_WAITING_FOR_R_DIGIT_2,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_R_DIGIT_2,      DIGIT,    &_MessageBuffer::parseDigit2,     IS_WAITING_FOR_R_DIGIT_3_OR_F, MT_ANALOG_POSITION},
  {IS_WAITING_FOR_R_DIGIT_3_OR_F, 'F',      &_MessageBuffer::parseRDigits,    IS_WAITING_FOR_F_DIGIT_1,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_R_DIGIT_3_OR_F, DIGIT,    &_MessageBuffer::parseRDigits,    IS_WAITING_FOR_F,              MT_ANALOG_POSITION},
  {IS_WAITING_FOR_F_DIGIT_1,      DIGIT,    &_MessageBuffer::parseDigit1,     IS_WAITING_FOR_F_DIGIT_2,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_F_DIGIT_2,      DIGIT,    &_MessageBuffer::parseDigit2,     IS_WAITING_FOR_F_DIGIT_3_OR_B, MT_ANALOG_POSITION},
  {IS_WAITING_FOR_F_DIGIT_3_OR_B, 'B',      &_MessageBuffer::parseFDigits,    IS_WAITING_FOR_B_DIGIT_1,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_F_DIGIT_3_OR_B, DIGIT,    &_MessageBuffer::parseFDigits,    IS_WAITING_FOR_B,              MT_ANALOG_POSITION},
  {IS_WAITING_FOR_B_DIGIT_1,      DIGIT,    &_MessageBuffer::parseDigit1,     IS_WAITING_FOR_B_DIGIT_2,      MT_ANALOG_POSITION},
  /*
   * NB(ericzundel) ParseDigit2B is different. It modifies the next state to be IS_MESSAGE_READY if in hex mode.
   * We could eliminate this snowflake by adding more states.
   */
  {IS_WAITING_FOR_B_DIGIT_2,      DIGIT,    &_MessageBuffer::parseDigit2B, IS_WAITING_FOR_B_DIGIT_3,      MT_ANALOG_POSITION},
  {IS_WAITING_FOR_B_DIGIT_3,      DIGIT,    &_MessageBuffer::parseBDigits, IS_MESSAGE_READY,              MT_ANALOG_POSITION},
};


// Singleton for other libraries to access this module
GamePadModule GamePad;

// Static data for this module
static _MessageBuffer message;

_MessageBuffer::_MessageBuffer()
{
  this->clear();
}

/**
 * Reset this buffer to the starting state.
 */
void _MessageBuffer::clear() {
  messageType = MT_UNKNOWN;
  leftValue = rightValue = upValue = downValue = 0;
  inputState = IS_START;
  digitBuf[0] = digitBuf[1] = 0;
  isHex = false;
}

/**
 * Store the first digit in the analog value.
 * Returns: 0 on success, 0xFF on failure
 */
int _MessageBuffer::parseDigit1(int inputChar, enum _INPUT_STATE *nextStatePtr) {
#if DEBUG
  DebugPrintln("In parseDigit1");
#endif
  this->digitBuf[0] = inputChar;
  return 0;
}

/**
 * Store the second digit in the analog value.
 * Returns: 0 on success, 0xFF on failure
 */

int _MessageBuffer::parseDigit2(int inputChar, enum _INPUT_STATE *nextStatePtr) {
#if DEBUG
  DebugPrintln("In parseDigit2");
#endif
  this->digitBuf[1] = inputChar;
  return 0;
}

/**
 * Store the second digit in the analog value when parsing the Back (Down) digits.
 *
 * Since this is the last digit in a hex mode version of the Analog Message,
 * it will force the message ready state to override the decl in the state table.
 *
 * Returns: 0 on success
 */

int _MessageBuffer::parseDigit2B(int inputChar, enum _INPUT_STATE *nextStatePtr) {
#if DEBUG
  DebugPrintln("In parseDigit2B");
#endif
  this->digitBuf[1] = inputChar;
  if(this->isHex) {
    // Override the input character to avoid error checking for 3rd decimal digit
    this->parseBDigits(0xFF, nextStatePtr);
    *nextStatePtr = IS_MESSAGE_READY;
  }
  return 0;
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
 * If in hex mode, assume there are only 2 digits and ignore the input character.
 * If not in hex mode, assume there are 3 digits and use the input character as the ones place.
 *
 * inputChar: current character being processed
 * valuePtr: set to the parsed 2 or 3 digit value on success.
 *
 * Returns: 0 on success, non zero on failure
 */
int _MessageBuffer::_parseDigits(int inputChar, uint8_t *valuePtr) {
  if (this->isHex) {
    if (this->_isDecDigit(inputChar)) {
#if DEBUG
      DebugPrintln("ERROR: Unexpected Digit in third place");
#endif
      return GP_ERROR_UNEXPECTED_DEC_DIGIT;
    }
    *valuePtr = (uint8_t) (_asciiToInt(this->digitBuf[0]) << 4) + _asciiToInt(this->digitBuf[1]);
  } else {
    uint8_t digit = _asciiToInt(this->digitBuf[0]);
    if (digit > 9) {
#if DEBUG
      DebugPrintln("ERROR: Invalid Decimal in hundreds");
#endif
      return GP_ERROR_INVALID_DEC_DIGIT;
    }
    *valuePtr = digit * 100;
    digit = _asciiToInt(this->digitBuf[1]);
    if (digit > 9) {
#if DEBUG
      DebugPrintln("ERROR: Invalid Decimal in tens");
#endif
      return GP_ERROR_INVALID_DEC_DIGIT;
    }
    *valuePtr = *valuePtr + (digit * 10);
    digit = _asciiToInt(inputChar);
    if (digit > 9) {
#if DEBUG
      DebugPrintln("ERROR: Invalid Decimal in ones");
#endif
      return GP_ERROR_INVALID_DEC_DIGIT;
    }
    *valuePtr = *valuePtr + digit;
  }
  return 0;
}

int _MessageBuffer::parseLDecDigits(int inputChar, enum _INPUT_STATE *nextStatePtr) {
#if DEBUG
  DebugPrintln("In parseLDecDigits");
#endif
  // Setting isHex is not necessary, but just to drive the point home we detected decimal mode
  this->isHex = false;

  uint8_t value = 0;
  int result = _parseDigits(inputChar, &value);
  this->leftValue = value;
  return result;
}

int _MessageBuffer::parseLHexDigits(int inputChar, enum _INPUT_STATE *nextStatePtr) {
#if DEBUG
  DebugPrintln("In parseLHexDigits");
#endif
  this->isHex = true;
  uint8_t value = 0;
  int result = _parseDigits(inputChar, &value);
  this->leftValue = value;
  return result;
}

int _MessageBuffer::parseRDigits(int inputChar, enum _INPUT_STATE *nextStatePtr) {
#if DEBUG
  DebugPrint("In parseRDigits isHex:");
  Serial.println(this->isHex);
#endif
  uint8_t value = 0;
  int result = _parseDigits(inputChar, &value);
  this->rightValue = value;
  return result;
}

int _MessageBuffer::parseFDigits(int inputChar, enum _INPUT_STATE *nextStatePtr) {
#if DEBUG
  DebugPrintln("In parseFDigits");
#endif
  uint8_t value = 0;
  int result = _parseDigits(inputChar, &value);
  this->upValue = value;
  return result;
}

int _MessageBuffer::parseBDigits(int inputChar, enum _INPUT_STATE *nextStatePtr) {
#if DEBUG
  DebugPrintln("In parseBDigits");
#endif
  uint8_t value;
  int result = _parseDigits(inputChar, &value);
  this->downValue = value;
  return result;
}

/**
 * Evaluate this entry in the state table using the optional function and proceed
 * to the next state.
 *
 * Returns: 0 on success, non-zero on failure
 */
int _MessageBuffer::_processStateEntry(struct state_entry *entry, int inputChar) {
  enum _INPUT_STATE nextState = entry->nextState;
  if (entry->state_func) {
    int result = (this->*entry->state_func)(inputChar, &nextState);
    if (result) {
#if DEBUG
      DebugPrint("ERROR: State Func returned: ");
      Serial.println(result);
#endif
      return result;
    }
  }
  if (MT_UNKNOWN != entry->messageType) {
    this->messageType = entry->messageType;
  }
  this->inputState = nextState;
  return 0;
}

static void printStateEntry(struct state_entry *entryPtr) {
  DebugPrint(" State: ");
  Serial.print(entryPtr->currentState);
  DebugPrint(" Input: ");
  if (entryPtr->inputChar < 200) {
    Serial.print((char)entryPtr->inputChar);
  } else {
    Serial.print(entryPtr->inputChar);
  }
  DebugPrint(" Next State: ");
  Serial.println(entryPtr->nextState);
}

/**
 * For debugging
 */
int _MessageBuffer::_printStateTable(){
  Serial.print("Table Size: ");
  Serial.print(sizeof(stateTable));
  Serial.println(" bytes");
  for (int i = 0; i < sizeof(stateTable)/sizeof(struct state_entry); i++) {
    Serial.print("Entry ");
    Serial.print(i);
    struct state_entry entry;
    memcpy_P(&entry, &stateTable[i], sizeof(struct state_entry));
    printStateEntry(&entry);
  }
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
  // Clean up after the last message
  if (IS_MESSAGE_READY == this->inputState || IS_ERROR == this->inputState) {
    this->clear();
  }

  bool isDigit = _isHexDigit(inputChar);

  // Go through the state table to find a matching state
  bool found = false;
  int result = 0;
  for (int i = 0; i < sizeof(stateTable)/sizeof(struct state_entry); i++) {
    struct state_entry entry;
    memcpy_P(&entry, &stateTable[i], sizeof(struct state_entry));

    if ((this->inputState == entry.currentState)
	&& ((inputChar == entry.inputChar)
	    || (isDigit && (DIGIT == entry.inputChar)))) {
#if DEBUG
      DebugPrint("Found ");
      Serial.print(i);
      printStateEntry(&entry);
#endif
      found = true;
      result = _processStateEntry(&entry, inputChar);
      break;
    }
  }

  if (!found) {
#if DEBUG
    DebugPrint("ERROR: No state entry found for ");
    Serial.print(this->inputState);
    DebugPrint(" Input: ");
    Serial.print((char)inputChar);
    DebugPrint(" isDigit: ");
    Serial.print(isDigit);
    Serial.println();
#endif
    // This is an error state, reset everything.
    this->clear();
    return GP_ERROR_NO_STATE_ENTRY;
  } else if (result) {
    // This is an error state, reset everything.
    this->clear();
    return result;
  } else if (IS_MESSAGE_READY == this->inputState) {
    return 0;
  }

  return 1;
}

// Class Constructor
GamePadModule::GamePadModule() {
  this->_clear();
}

/**
 * Reset the action button state.
 */
void GamePadModule::_clearActionButtons() {
  this->actionButtons = 0;
}

/**
 * Reset the module state.
 */
void GamePadModule::_clear() {
  this->actionButtons = this->positionButtons = 0;
  this->posLeft = this->posRight = this->posUp = this->posDown = 0;
}

/**
 * Returns: true if the up button is pressed.  Emulated in analog mode.
 */
bool GamePadModule::isUpPressed()
{
  return !!(this->positionButtons & (1<<UP_BIT));
}

/**
 * Returns: true if the down button is pressed.  Emulated in analog mode.
 */
bool GamePadModule::isDownPressed()
{
  return !!(this->positionButtons & (1<<DOWN_BIT));
}


/**
 * Returns: true if the left button is pressed.  Emulated in analog mode.
 */
bool GamePadModule::isLeftPressed()
{
  return !!(this->positionButtons & (1<<LEFT_BIT));
}

/**
 * Returns: true if the right button is pressed.  Emulated in analog mode.
 */
bool GamePadModule::isRightPressed()
{
  return !!(this->positionButtons & (1<<RIGHT_BIT));
}

// Orange Button Checker
bool GamePadModule::isStartPressed()
{
  return !!(this->actionButtons & (1<<START_BIT));
}

//Red Button Checker
bool GamePadModule::isSelectPressed()
{
  return !!(this->actionButtons & (1<<SELECT_BIT));
}

bool GamePadModule::isAPressed()
{
   return !!(this->actionButtons & (1<<BUTTON_A_BIT));
}

bool GamePadModule::isBPressed()
{
   return !!(this->actionButtons & (1<<BUTTON_B_BIT));
}

bool GamePadModule::isXPressed()
{
   return !!(this->actionButtons & (1<<BUTTON_X_BIT));
}

bool GamePadModule::isYPressed()
{
   return !!(this->actionButtons & (1<<BUTTON_Y_BIT));
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

uint8_t GamePadModule::getLeftPosition() {
  return this->posLeft;
}

uint8_t GamePadModule::getRightPosition() {
  return this->posRight;
}

uint8_t GamePadModule::getUpPosition() {
  return this->posUp;
}

uint8_t GamePadModule::getDownPosition() {
  return this->posDown;
}

/**
 * Handle the work of processing GamePad specific input.
 *
 */
int GamePadModule::_processInput(int inputChar)
{
#if DEBUG
  DebugPrint("GPM Input: ");
  if (inputChar < 128) {
    Serial.print("'");
    Serial.print((char)inputChar);
    Serial.print("'");
  } else {
    Serial.print(inputChar);
  }
  Serial.println();
#endif

  this->actionButtons = 0;

  if (!message.processInput(inputChar)) {
    // Got a new message
    switch (message.messageType) {
    case MT_START_BUTTON:
      this->actionButtons |= 1<<START_BIT;
      break;
    case MT_SELECT:
      this->actionButtons |= 1<<SELECT_BIT;
      break;
    case MT_BUTTON_A:
      this->actionButtons |= 1<<BUTTON_A_BIT;
      break;
    case MT_BUTTON_B:
      this->actionButtons |= 1<<BUTTON_B_BIT;
      break;
    case MT_BUTTON_X:
      this->actionButtons |= 1<<BUTTON_X_BIT;
      break;
    case MT_BUTTON_Y:
      this->actionButtons |= 1<<BUTTON_Y_BIT;
      break;
    case MT_ANALOG_POSITION:
      // Store the analog position
      this->posLeft = message.leftValue;
      this->posRight = message.rightValue;
      this->posUp = message.upValue;
      this->posDown = message.downValue;

      // Emulate the digital pushbuttons
      if (0 == this->posUp && 0 == this->posDown && 0 == this->posLeft && 0 == this->posRight) {
	// Stop position
	this->positionButtons = 0;
      } else {
	uint8_t largest = this->posUp;
	this->positionButtons = 1<<UP_BIT;
	if (this->posDown > largest) {
	  largest = this->posDown;
	  this->positionButtons = 1<<DOWN_BIT;
	}
	if (this->posLeft > largest) {
	  largest = this->posLeft;
	  this->positionButtons = 1<<LEFT_BIT;
	}
	if (this->posRight > largest) {
	  largest = this->posRight;
	  this->positionButtons = 1<<RIGHT_BIT;
	}
      }
      break;
    case MT_UNKNOWN:
    default:
      // Likely indicates an error in coding the state table
#if DEBUG
      Serial.print("Error: Unhandled Message Type ");
      Serial.println(message.messageType);
#endif
      return GP_ERROR_UNHANDLED_MESSAGE_TYPE;
      break;
    }

    // Clear out the message state for parsing the next message
    message.clear();
  }
}
