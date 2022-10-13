/**
 * MessageBuffer.h - An internal class that helps GamePad parse a message from the App
 *
 * Not intended for use outside of the library or unit testing
 */
#ifndef MESSAGE_BUFFER_H
#define MESSAGE_BUFFER_H

// Defines the state machine states for parsing input
enum _INPUT_STATE {
  IS_START = 0,
  IS_ERROR = 2,
  IS_WAITING_FOR_L = 10,
  IS_WAITING_FOR_L_DIGIT_1 = 11,
  IS_WAITING_FOR_L_DIGIT_2 = 12,
  IS_WAITING_FOR_L_DIGIT_3_OR_R = 13,
  IS_WAITING_FOR_R = 20,
  IS_WAITING_FOR_R_DIGIT_1 = 21,
  IS_WAITING_FOR_R_DIGIT_2 = 22,
  IS_WAITING_FOR_R_DIGIT_3_OR_F = 23,
  IS_WAITING_FOR_F = 30,
  IS_WAITING_FOR_F_DIGIT_1 = 31,
  IS_WAITING_FOR_F_DIGIT_2 = 32,
  IS_WAITING_FOR_F_DIGIT_3_OR_B = 33,
  IS_WAITING_FOR_B = 40,
  IS_WAITING_FOR_B_DIGIT_1 = 41,
  IS_WAITING_FOR_B_DIGIT_2 = 42,
  IS_WAITING_FOR_B_DIGIT_3 = 43,
  IS_MESSAGE_READY = 50,
};


enum _MESSAGE_TYPE {
  MT_UNKNOWN = 0,
  MT_START_BUTTON,
  MT_SELECT,
  MT_BUTTON_A,
  MT_BUTTON_B,
  MT_BUTTON_X,
  MT_BUTTON_Y,
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

  int parseDigit1(int inputChar, enum _INPUT_STATE *nextStatePtr);
  int parseDigit2(int inputChar, enum _INPUT_STATE *nextStatePtr);
  int parseDigit2B(int inputchar, enum _INPUT_STATE *nextStatePtr);
  int parseLDecDigits(int inputChar, enum _INPUT_STATE *nextStatePtr);
  int parseLHexDigits(int inputChar, enum _INPUT_STATE *nextStatePtr);
  int parseRDigits(int inputChar, enum _INPUT_STATE *nextStatePtr);
  int parseBDigits(int inputChar, enum _INPUT_STATE *nextStatePtr);
  int parseFDigits(int inputChar, enum _INPUT_STATE *nextStatePtr);
  void clear();

  enum _MESSAGE_TYPE messageType;
  uint8_t leftValue;
  uint8_t rightValue;
  uint8_t upValue;
  uint8_t downValue;

  /* private: */ // NB(ericzundel): I didn't make these private so I could test them.
  static bool _isDecDigit(int inputChar);
  static bool _isHexDigit(int inputChar);
  static uint8_t _asciiToInt(int inputChar);
  int _printStateTable();
  enum _INPUT_STATE inputState;

private:
  int _parseDigits(int inputChar, uint8_t *valuePtr);
  int _processStateEntry(struct state_entry *entry, int inputChar);


  char digitBuf[2];
  /* isHex: hexadecmial mode
   * true if we are parsing 2 digit hex values
   * false if we are parsing 3 digit decimal values
   *
   * NB(ericzundel): we could do away with this by adding lots more states,
   *   but that seems like a lot of work when hand writing a parser.
   */
  boolean isHex;
};

#endif
