/**
 * GamePad: For communicating with the Controller mode in the BitBus App.
 *
 * General Use:
 * Call BitBus.processInput(), then check the getters to see what has changed.
 *
 * The name GamePad is for compatibility with the Dabble library.
 */
#ifndef GamePad_h
#define GamePad_h

#include "Arduino.h"

enum GAMEPAD_ERROR {
  GP_OK = 0,
  GP_ERROR_UNHANDLED_MESSAGE_TYPE = 100,
  GP_ERROR_NO_STATE_ENTRY = 101,
  GP_ERROR_INVALID_DEC_DIGIT = 102,
  GP_ERROR_UNEXPECTED_DEC_DIGIT = 103,
};

class GamePadModule
{
 public:
  GamePadModule();

  // Getter Functions
  bool isStartPressed();
  bool isSelectPressed();


  bool isAPressed();
  bool isBPressed();
  bool isYPressed();
  bool isXPressed();

  bool isUpPressed();
  bool isDownPressed();
  bool isLeftPressed();
  bool isRightPressed();

  uint8_t getLeftPosition();
  uint8_t getRightPosition();
  uint8_t getUpPosition();
  uint8_t getDownPosition();

  // Dabble Compatibility functions
  bool isTrianglePressed(); // Same as Button B
  bool isCirclePressed();   // Same as Button Y
  bool isCrossPressed();    // Same as Button X
  bool isSquarePressed();   // Same as Button A


  // TODO(ericzundel): Unimplemented Dabble compatibility methods
  // NB(zundel): I'm not very keen on adding in floating point

  //  uint16_t getAngle();
  //  uint8_t getRadius();
  //  bool isPressed(uint8_t a);

  //  float getJoystickData(uint8_t b);
  //  float getx_axis();
  //  float gety_axis();

  //  float getXaxisData();
  //  float getYaxisData();

  // Process an input character. Only meant to be called by tests and the BitBus module.
  int _processInput(int inputChar);
  // Clear the state of the action buttons. Only meant to be called by tests and the BitBus module.
  void _clearActionButtons();
  // Clear the state of the entire object. Only meant to be called by tests and the BitBus module.
  void _clear();


 private:
  uint8_t actionButtons;
  uint8_t positionButtons;
  uint8_t posLeft;
  uint8_t posRight;
  uint8_t posUp;
  uint8_t posDown;
};

extern GamePadModule GamePad;
#endif
