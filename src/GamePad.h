/**
 * For communicating with the Controller mode in the BitBus App.
 *
 * The name GamePad is for compatibility with the Dabble library.
 */
#ifndef GamePad_h
#define GamePad_h

#include "Arduino.h"

// Action Button Bit Reference
// For member action_button_value
#define START_BIT 0
#define SELECT_BIT 1
#define TRIANGLE_BIT 2
#define CIRCLE_BIT 3
#define CROSS_BIT 4
#define SQUARE_BIT 5

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
  //  float getJoystickData(uint8_t b);
  //  float getx_axis();
  //  float gety_axis();
	
  //  float getXaxisData();
  //  float getYaxisData();

  // This method is only meant to be called by the BitBus module
  void _processInput();

 private:
  uint8_t action_buttons;
  uint8_t pos_left;
  uint8_t pos_right;
  uint8_t pos_front;
  uint8_t pos_back;

};

extern GamePadModule GamePad;
#endif
