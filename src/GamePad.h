/**
 * For communicating with the Controller mode in the BitBus App.
 *
 * The name GamePad is for compatibility with the Dabble library.
 */
#ifndef GamePad_h
#define GamePad_h

#include "Arduino.h"

enum GAMEPAD_ERROR {
  GP_OK = 0,
  GP_ERROR_EXPECTED_CONSTANT = 10,
  GP_ERROR_UNHANDLED_MESSAGE_TYPE = 11,
};

class GamePadModule
{
 public:
  //Constructor
  GamePadModule();
  
  //Checker Functions
  bool isStartPressed();
  bool isSelectPressed();
  bool isTrianglePressed();
  bool isCirclePressed();
  bool isCrossPressed();
  bool isSquarePressed();

  bool isUpPressed();
  bool isDownPressed();
  bool isLeftPressed();
  bool isRightPressed();

  // TODO(ericzundel): These methods I'd like to support eventually
  
  //  uint16_t getAngle();
  //  uint8_t getRadius();
  //  bool isPressed(uint8_t a);
  
  // TODO(ericzundel) Should I use floating point? The Dabble 
  // class uses floating point, but the BitBus is all integer math.
  //
  //  float getJoystickData(uint8_t b);
  //  float getx_axis();
  //  float gety_axis();
	
  //  float getXaxisData();
  //  float getYaxisData();

  // This method is only meant to be called by the BitBus module
  int _processInput(int inputChar);
  void _clearActionButtons();
  void _clear();

  uint8_t getLeftPosition();
  uint8_t getRightPosition();
  uint8_t getUpPosition();
  uint8_t getDownPosition();
  
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
