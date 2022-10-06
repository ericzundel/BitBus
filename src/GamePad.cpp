#include "BitBus.h"
#include "GamePad.h"

#include "Arduino.h"
#include "SoftwareSerial.h"

// Class Constructor
GamePadModule::GamePadModule() {
  pos_left = 0;
  pos_right = 0;
  pos_front = 0;
  pos_back = 0;
  action_buttons = 0;
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
	return !!(action_buttons & (1<<START_BIT));
}

//Red Button Checker 
bool GamePadModule::isSelectPressed()
{
	
	return !!(action_buttons & (1<<SELECT_BIT));
}

//Green Button Checker 
bool GamePadModule::isTrianglePressed()
{
	return !!(action_buttons & (1<<TRIANGLE_BIT));
}

//Blue Button Checker
bool GamePadModule::isCirclePressed()
{	
	return !!(action_buttons & (1<<CIRCLE_BIT));
}

bool GamePadModule::isCrossPressed()
{
	return !!(action_buttons & (1<<CROSS_BIT));
}

bool GamePadModule::isSquarePressed()
{
	return !!(action_buttons & (1<<SQUARE_BIT));
}

// Handle the work of processing GamePad specific input.
void GamePadModule::_processInput()
{
  
}
