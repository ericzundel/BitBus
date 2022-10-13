#include <BitBus.h>
#include <GamePad.h>
#include <MessageBuffer.h>

// Testing and Debugging Routines
#include <BitBusUtil.h>

/*
 * Unit tests for the BitBus Controller Module.
 *
 * On success, it should print "All tests passed." at the end of the run.
 *
 * Interface Based on: https://thestempedia.com/docs/dabble/game-pad-module/
 */
void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);    // Make sure your Serial Monitor is also set at this baud rate.
  BitBus.begin(9600);     // Enter baudrate of your bluetooth.Connect bluetooth on Bluetooth port present on your Arduino.
}

_MessageBuffer mb;

void printTest(char *msg) {
  Serial.println("--------------------------------------------------------");
  Serial.print("Starting Test ");
  Serial.println(msg);
}

void testMessageBufferInternals() {
  printTest("MessageBufferInternals");

  Serial.println(" State Table:");
  mb._printStateTable();
  Serial.println();
  
  Serial.println(" Test isDecDigit");

  ASSERT(mb._isDecDigit('0'), "expected 0 to be Dec Digit");
  ASSERT(mb._isDecDigit('0'), "expected 9 to be Dec Digit");
  ASSERT(!mb._isDecDigit('A'), "expected A to not be Dec Digit");
  ASSERT(!mb._isDecDigit('0'-1), "expected non Dec Digit");

  Serial.println(" Test isHexDigit");
  ASSERT(mb._isHexDigit('0'), "expected 0 to be hex digit");
  ASSERT(mb._isHexDigit('1'), "expected 1 to be hex digit");
  ASSERT(mb._isHexDigit('A'), "expected A to be hex digit");
  ASSERT(mb._isHexDigit('F'), "expected F to be hex digit");
  ASSERT(!mb._isHexDigit('G'), "expected G not to be hex digit");
  ASSERT(!mb._isDecDigit('0'-1), "expected non Hex Digit");
}

void testMessageBufferActionButtons() {
  printTest("MessageBufferActions");

  Serial.println(" Test MT_BUTTON_B");
  mb.clear();
  ASSERTV(mb.messageType == MT_UNKNOWN, "expected MT_UNKNOWN", mb.messageType);
  ASSERTV(mb.inputState == IS_START, "expected IS_START", mb.inputState);
  mb.processInput('B');
  ASSERTV(mb.messageType == MT_BUTTON_B, "expected MT_BUTTON_B", mb.messageType);
  ASSERTV(mb.inputState == IS_MESSAGE_READY, "expected IS_MESSAGE_READY", mb.inputState);

  Serial.println(" Test MT_BUTTON_Y");
  mb.clear();
  mb.processInput('Y');
  ASSERTV(mb.messageType == MT_BUTTON_Y, "expected MT_BUTTON_Y", mb.messageType);
  ASSERTV(mb.inputState == IS_MESSAGE_READY, "expected IS_MESSAGE_READY", mb.inputState);

  Serial.println(" Test MT_BUTTON_X");
  mb.clear();
  mb.processInput('X');
  ASSERTV(mb.messageType == MT_BUTTON_X, "expected MT_BUTTON_X", mb.messageType);
  ASSERTV(mb.inputState == IS_MESSAGE_READY, "expected IS_MESSAGE_READY", mb.inputState);

  Serial.println(" Test MT_BUTTON_A");
  mb.clear();
  mb.processInput('A');
  ASSERTV(mb.messageType == MT_BUTTON_A, "expected MT_BUTTON_A", mb.messageType);
  ASSERTV(mb.inputState == IS_MESSAGE_READY, "expected IS_MESSAGE_READY", mb.inputState);

  Serial.println(" Test MT_SELECT");
  mb.clear();
  mb.processInput('C');
  ASSERTV(mb.messageType == MT_SELECT, "expected MT_SELECT", mb.messageType);
  ASSERTV(mb.inputState == IS_MESSAGE_READY, "expected IS_MESSAGE_READY", mb.inputState);

  Serial.println(" Test MT_START");
  mb.clear();
  mb.processInput('S');
  ASSERTV(mb.messageType == MT_START_BUTTON, "expected MT_START_BUTTON", mb.messageType);
  ASSERTV(mb.inputState == IS_MESSAGE_READY, "expected IS_MESSAGE_READY", mb.inputState); 
}

void testMessageBufferAnalogPositionDec() {
  printTest("MessageBufferAnalogPositionDec");

  mb.clear();

  ASSERTV(mb.messageType == MT_UNKNOWN, "expected MT_UNKNOWN", mb.messageType);
  ASSERTV(mb.inputState == IS_START, "expected IS_START", mb.inputState);

  mb.processInput('L');
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "L expected MT_ANALOG_POSITION", mb.messageType);
  ASSERTV(mb.inputState == IS_WAITING_FOR_L_DIGIT_1, "expected IS_WAITING_FOR_L_DIGIT_1", mb.inputState);

  mb.processInput('0');
  ASSERTV(mb.inputState == IS_WAITING_FOR_L_DIGIT_2, "expected IS_WAITING_FOR_L_DIGIT_2", mb.inputState);

  mb.processInput('0');
  ASSERTV(mb.inputState == IS_WAITING_FOR_L_DIGIT_3_OR_R, "expected IS_WAITING_FOR_L_DIGIT_3_OR_F", mb.inputState);

  mb.processInput('1');
  ASSERTV(mb.inputState == IS_WAITING_FOR_CONSTANT, "expected IS_WAITING_FOR_CONSTANT", mb.inputState);

  mb.processInput('R');
  ASSERTV(mb.inputState == IS_WAITING_FOR_R_DIGIT_1, "expected IS_WAITING_FOR_R_DIGIT_1", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('2');
  ASSERTV(mb.inputState == IS_WAITING_FOR_R_DIGIT_2, "expected IS_WAITING_FOR_R_DIGIT_2", mb.inputState);

  mb.processInput('0');
  ASSERTV(mb.inputState == IS_WAITING_FOR_R_DIGIT_3_OR_F, "expected IS_WAITING_FOR_R_DIGIT_3_OR_F", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('0');
  ASSERTV(mb.inputState == IS_WAITING_FOR_CONSTANT, "expected IS_WAITING_FOR_CONSTANT", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('F');
  ASSERTV(mb.inputState == IS_WAITING_FOR_F_DIGIT_1, "expected IS_WAITING_FOR_F_DIGIT_1", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('0');
  ASSERTV(mb.inputState == IS_WAITING_FOR_F_DIGIT_2, "expected IS_WAITING_FOR_F_DIGIT_2", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('3');
  ASSERTV(mb.inputState == IS_WAITING_FOR_F_DIGIT_3_OR_B, "expected IS_WAITING_FOR_F_DIGIT_3_OR_B", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('0');
  ASSERTV(mb.inputState == IS_WAITING_FOR_CONSTANT, "expected IS_WAITING_FOR_CONSTANT", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('B');
  ASSERTV(mb.inputState == IS_WAITING_FOR_B_DIGIT_1, "expected IS_WAITING_FOR_B_DIGIT_1", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('0');
  ASSERTV(mb.inputState == IS_WAITING_FOR_B_DIGIT_2, "expected IS_WAITING_FOR_B_DIGIT_2", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('0');
  ASSERTV(mb.inputState == IS_WAITING_FOR_B_DIGIT_3, "expected IS_WAITING_FOR_F_DIGIT_3", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('2');
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);
  ASSERTV(mb.inputState == IS_MESSAGE_READY, "expected IS_MESSAGE_READY", mb.inputState);

  ASSERTV(mb.leftValue == 1, "expected left == 1", mb.leftValue);
  ASSERTV(mb.rightValue == 200, "expected right == 200", mb.rightValue);
  ASSERTV(mb.upValue == 30, "expected up == 30", mb.upValue);
  ASSERTV(mb.downValue == 2, "expected down == 2", mb.downValue);
}

void testMessageBufferAnalogPositionHex() {
  printTest("MessageBufferAnalogPositionHex");

  mb.clear();
  
  ASSERTV(mb.messageType == MT_UNKNOWN, "expected MT_UNKNOWN", mb.messageType);
  ASSERTV(mb.inputState == IS_START, "expected IS_START", mb.inputState);

  mb.processInput('L');
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "L expected MT_ANALOG_POSITION", mb.messageType);
  ASSERTV(mb.inputState == IS_WAITING_FOR_L_DIGIT_1, "expected IS_WAITING_FOR_L_DIGIT_1", mb.inputState);

  mb.processInput('0');
  ASSERTV(mb.inputState == IS_WAITING_FOR_L_DIGIT_2, "expected IS_WAITING_FOR_L_DIGIT_2", mb.inputState);

  mb.processInput('1');
  ASSERTV(mb.inputState == IS_WAITING_FOR_L_DIGIT_3_OR_R, "expected IS_WAITING_FOR_L_DIGIT_3_OR_F", mb.inputState);

  mb.processInput('R');
  ASSERTV(mb.inputState == IS_WAITING_FOR_R_DIGIT_1, "expected IS_WAITING_FOR_R_DIGIT_1", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('2');
  ASSERTV(mb.inputState == IS_WAITING_FOR_R_DIGIT_2, "expected IS_WAITING_FOR_R_DIGIT_2", mb.inputState);

  mb.processInput('0');
  ASSERTV(mb.inputState == IS_WAITING_FOR_R_DIGIT_3_OR_F, "expected IS_WAITING_FOR_R_DIGIT_3_OR_F", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('F');
  ASSERTV(mb.inputState == IS_WAITING_FOR_F_DIGIT_1, "expected IS_WAITING_FOR_F_DIGIT_1", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('3');
  ASSERTV(mb.inputState == IS_WAITING_FOR_F_DIGIT_2, "expected IS_WAITING_FOR_F_DIGIT_2", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('F');
  ASSERTV(mb.inputState == IS_WAITING_FOR_F_DIGIT_3_OR_B, "expected IS_WAITING_FOR_F_DIGIT_3_OR_B", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('B');
  ASSERTV(mb.inputState == IS_WAITING_FOR_B_DIGIT_1, "expected IS_WAITING_FOR_B_DIGIT_1", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('0');
  ASSERTV(mb.inputState == IS_WAITING_FOR_B_DIGIT_2, "expected IS_WAITING_FOR_B_DIGIT_2", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  mb.processInput('A');
  ASSERTV(mb.inputState == IS_MESSAGE_READY, "expected IS_MESSAGE_READY", mb.inputState);
  ASSERTV(mb.messageType == MT_ANALOG_POSITION, "expected MT_ANALOG_POSITION", mb.messageType);

  ASSERTV(mb.leftValue == 0x01, "expected left == 0x01", mb.leftValue);
  ASSERTV(mb.rightValue == 0x20, "expected right == 0x20", mb.rightValue);
  ASSERTV(mb.upValue == 0x3F, "expected up == 0x3F", mb.upValue);
  ASSERTV(mb.downValue == 0x0A, "expected down == 0x0A", mb.downValue);
}

/**
 * Send characters to GamePad._processInput()
 */
void sendToGamePadProcessInput(char *inputStr) {
  for (; *inputStr != 0; inputStr++) {
    GamePad._processInput(*inputStr);
  }
}

void testGamePadInternal() {
  printTest("GamePadInternal");
  GamePad._clear();
  
  // Nothing should be set
  ASSERT(!GamePad.isUpPressed(), "unexpected UP");
  ASSERT(!GamePad.isDownPressed(), "unexpected DOWN");
  ASSERT(!GamePad.isLeftPressed(), "unexpected LEFT");
  ASSERT(!GamePad.isRightPressed(), "unexpected RIGHT");
  ASSERT(!GamePad.isAPressed(), "unexpected A");
  ASSERT(!GamePad.isBPressed(), "unexpected B");
  ASSERT(!GamePad.isXPressed(), "unexpected X");
  ASSERT(!GamePad.isYPressed(), "unexpected Y");
  ASSERT(!GamePad.isSquarePressed(), "unexpected SQUARE");
  ASSERT(!GamePad.isCirclePressed(), "unexpected CIRCLE");
  ASSERT(!GamePad.isCrossPressed(), "unexpected CROSS");
  ASSERT(!GamePad.isTrianglePressed(), "unexpected TRIANGLE");
  ASSERT(!GamePad.isStartPressed(), "unexpected START");
  ASSERT(!GamePad.isSelectPressed(), "unexpected SELECT");

  Serial.println(" Testing Start button");
  sendToGamePadProcessInput("S");
  ASSERT(!GamePad.isUpPressed(), "unexpected UP");
  ASSERT(!GamePad.isDownPressed(), "unexpected DOWN");
  ASSERT(!GamePad.isLeftPressed(), "unexpected LEFT");
  ASSERT(!GamePad.isRightPressed(), "unexpected RIGHT");
  ASSERT(!GamePad.isAPressed(), "unexpected A");
  ASSERT(!GamePad.isBPressed(), "unexpected B");
  ASSERT(!GamePad.isXPressed(), "unexpected X");
  ASSERT(!GamePad.isYPressed(), "unexpected Y");
  ASSERT(!GamePad.isSquarePressed(), "unexpected SQUARE");
  ASSERT(!GamePad.isCirclePressed(), "unexpected CIRCLE");
  ASSERT(!GamePad.isCrossPressed(), "unexpected CROSS");
  ASSERT(!GamePad.isTrianglePressed(), "unexpected TRIANGLE");
  ASSERT(GamePad.isStartPressed(), "expected START");
  ASSERT(!GamePad.isSelectPressed(), "unexpected SELECT");

  Serial.println(" Testing _clear");
  // Nothing should be set
  GamePad._clear();
  ASSERT(!GamePad.isUpPressed(), "unexpected UP");
  ASSERT(!GamePad.isDownPressed(), "unexpected DOWN");
  ASSERT(!GamePad.isLeftPressed(), "unexpected LEFT");
  ASSERT(!GamePad.isRightPressed(), "unexpected RIGHT");
  ASSERT(!GamePad.isAPressed(), "unexpected A");
  ASSERT(!GamePad.isBPressed(), "unexpected B");
  ASSERT(!GamePad.isXPressed(), "unexpected X");
  ASSERT(!GamePad.isYPressed(), "unexpected Y");
  ASSERT(!GamePad.isSquarePressed(), "unexpected SQUARE");
  ASSERT(!GamePad.isCirclePressed(), "unexpected CIRCLE");
  ASSERT(!GamePad.isCrossPressed(), "unexpected CROSS");
  ASSERT(!GamePad.isTrianglePressed(), "unexpected TRIANGLE");
  ASSERT(!GamePad.isStartPressed(), "unexpected START");
  ASSERT(!GamePad.isSelectPressed(), "unexpected SELECT");

  
  Serial.println(" Testing Select button");
  GamePad._clear();
  sendToGamePadProcessInput("C");
  ASSERT(!GamePad.isUpPressed(), "unexpected UP");
  ASSERT(!GamePad.isDownPressed(), "unexpected DOWN");
  ASSERT(!GamePad.isLeftPressed(), "unexpected LEFT");
  ASSERT(!GamePad.isRightPressed(), "unexpected RIGHT");
  ASSERT(!GamePad.isAPressed(), "unexpected A");
  ASSERT(!GamePad.isBPressed(), "unexpected B");
  ASSERT(!GamePad.isXPressed(), "unexpected X");
  ASSERT(!GamePad.isYPressed(), "unexpected Y");
  ASSERT(!GamePad.isSquarePressed(), "unexpected SQUARE");
  ASSERT(!GamePad.isCirclePressed(), "unexpected CIRCLE");
  ASSERT(!GamePad.isCrossPressed(), "unexpected CROSS");
  ASSERT(!GamePad.isTrianglePressed(), "unexpected TRIANGLE");
  ASSERT(!GamePad.isStartPressed(), "unexpected START");
  ASSERT(GamePad.isSelectPressed(), "expected SELECT");

  Serial.println(" Testing A/Square button");
  GamePad._clear();
  sendToGamePadProcessInput("A");
  ASSERT(!GamePad.isUpPressed(), "unexpected UP");
  ASSERT(!GamePad.isDownPressed(), "unexpected DOWN");
  ASSERT(!GamePad.isLeftPressed(), "unexpected LEFT");
  ASSERT(!GamePad.isRightPressed(), "unexpected RIGHT");
  ASSERT(GamePad.isAPressed(), "expected A");
  ASSERT(!GamePad.isBPressed(), "unexpected B");
  ASSERT(!GamePad.isXPressed(), "unexpected X");
  ASSERT(!GamePad.isYPressed(), "unexpected Y");
  ASSERT(GamePad.isSquarePressed(), "expected SQUARE");
  ASSERT(!GamePad.isCirclePressed(), "unexpected CIRCLE");
  ASSERT(!GamePad.isCrossPressed(), "unexpected CROSS");
  ASSERT(!GamePad.isTrianglePressed(), "unexpected TRIANGLE");
  ASSERT(!GamePad.isStartPressed(), "unexpected START");
  ASSERT(!GamePad.isSelectPressed(), "unexpected SELECT");

  Serial.println(" Testing B/Triangle button");
  GamePad._clear();
  sendToGamePadProcessInput("B");
  ASSERT(!GamePad.isUpPressed(), "unexpected UP");
  ASSERT(!GamePad.isDownPressed(), "unexpected DOWN");
  ASSERT(!GamePad.isLeftPressed(), "unexpected LEFT");
  ASSERT(!GamePad.isRightPressed(), "unexpected RIGHT");
  ASSERT(!GamePad.isAPressed(), "unexpected A");
  ASSERT(GamePad.isBPressed(), "expected B");
  ASSERT(!GamePad.isXPressed(), "unexpected X");
  ASSERT(!GamePad.isYPressed(), "unexpected Y");
  ASSERT(!GamePad.isSquarePressed(), "unexpected SQUARE");
  ASSERT(!GamePad.isCirclePressed(), "unexpected CIRCLE");
  ASSERT(!GamePad.isCrossPressed(), "unexpected CROSS");
  ASSERT(GamePad.isTrianglePressed(), "expected TRIANGLE");
  ASSERT(!GamePad.isStartPressed(), "unexpected START");
  ASSERT(!GamePad.isSelectPressed(), "unexpected SELECT");

  Serial.println(" Testing X/Cross button");
  GamePad._clear();
  sendToGamePadProcessInput("X");
  ASSERT(!GamePad.isUpPressed(), "unexpected UP");
  ASSERT(!GamePad.isDownPressed(), "unexpected DOWN");
  ASSERT(!GamePad.isLeftPressed(), "unexpected LEFT");
  ASSERT(!GamePad.isRightPressed(), "unexpected RIGHT");
  ASSERT(!GamePad.isAPressed(), "unexpected A");
  ASSERT(!GamePad.isBPressed(), "unexpected B");
  ASSERT(GamePad.isXPressed(), "expected X");
  ASSERT(!GamePad.isYPressed(), "unexpected Y");
  ASSERT(!GamePad.isSquarePressed(), "unexpected SQUARE");
  ASSERT(!GamePad.isCirclePressed(), "unexpected CIRCLE");
  ASSERT(GamePad.isCrossPressed(), "expected CROSS");
  ASSERT(!GamePad.isTrianglePressed(), "unexpected TRIANGLE");
  ASSERT(!GamePad.isStartPressed(), "unexpected START");
  ASSERT(!GamePad.isSelectPressed(), "unexpected SELECT");

  Serial.println(" Testing Y/Circle button");
  GamePad._clear();
  sendToGamePadProcessInput("Y");
  ASSERT(!GamePad.isUpPressed(), "unexpected UP");
  ASSERT(!GamePad.isDownPressed(), "unexpected DOWN");
  ASSERT(!GamePad.isLeftPressed(), "unexpected LEFT");
  ASSERT(!GamePad.isRightPressed(), "unexpected RIGHT");
  ASSERT(!GamePad.isAPressed(), "unexpected A");
  ASSERT(!GamePad.isBPressed(), "unexpected B");
  ASSERT(!GamePad.isXPressed(), "unexpected X");
  ASSERT(GamePad.isYPressed(), "expected Y");
  ASSERT(!GamePad.isSquarePressed(), "unexpected SQUARE");
  ASSERT(GamePad.isCirclePressed(), "expected CIRCLE");
  ASSERT(!GamePad.isCrossPressed(), "unexpected CROSS");
  ASSERT(!GamePad.isTrianglePressed(), "unexpected TRIANGLE");
  ASSERT(!GamePad.isStartPressed(), "unexpected START");
  ASSERT(!GamePad.isSelectPressed(), "unexpected SELECT");

  Serial.println(" Test Up");
  GamePad._clear();
  sendToGamePadProcessInput("L000R100F255B020");
  ASSERT(GamePad.isUpPressed(), "expected UP");
  ASSERT(!GamePad.isDownPressed(), "unexpected DOWN");
  ASSERT(!GamePad.isLeftPressed(), "unexpected LEFT");
  ASSERT(!GamePad.isRightPressed(), "unexpected RIGHT");
  ASSERT(!GamePad.isAPressed(), "unexpected A");
  ASSERT(!GamePad.isBPressed(), "unexpected B");
  ASSERT(!GamePad.isXPressed(), "unexpected X");
  ASSERT(!GamePad.isYPressed(), "unexpected Y");
  ASSERT(!GamePad.isSquarePressed(), "unexpected SQUARE");
  ASSERT(!GamePad.isCirclePressed(), "unexpected CIRCLE");
  ASSERT(!GamePad.isCrossPressed(), "unexpected CROSS");
  ASSERT(!GamePad.isTrianglePressed(), "unexpected TRIANGLE");
  ASSERT(!GamePad.isStartPressed(), "unexpected START");
  ASSERT(!GamePad.isSelectPressed(), "unexpected SELECT");
  ASSERTV(GamePad.getLeftPosition() == 0, "expected 0", GamePad.getLeftPosition());
  ASSERTV(GamePad.getRightPosition() == 100, "expected 100", GamePad.getRightPosition());
  ASSERTV(GamePad.getUpPosition() == 255, "expected 255", GamePad.getUpPosition());
  ASSERTV(GamePad.getDownPosition() == 20, "expected 20", GamePad.getDownPosition());

  Serial.println(" Test Left");
  GamePad._clear();
  sendToGamePadProcessInput("LFFR01F25BF0");
  ASSERT(!GamePad.isUpPressed(), "unexpected UP");
  ASSERT(!GamePad.isDownPressed(), "unexpected DOWN");
  ASSERT(GamePad.isLeftPressed(), "expected LEFT");
  ASSERT(!GamePad.isRightPressed(), "unexpected RIGHT");
  ASSERT(!GamePad.isAPressed(), "unexpected A");
  ASSERT(!GamePad.isBPressed(), "unexpected B");
  ASSERT(!GamePad.isXPressed(), "unexpected X");
  ASSERT(!GamePad.isYPressed(), "unexpected Y");
  ASSERT(!GamePad.isSquarePressed(), "unexpected SQUARE");
  ASSERT(!GamePad.isCirclePressed(), "unexpected CIRCLE");
  ASSERT(!GamePad.isCrossPressed(), "unexpected CROSS");
  ASSERT(!GamePad.isTrianglePressed(), "unexpected TRIANGLE");
  ASSERT(!GamePad.isStartPressed(), "unexpected START");
  ASSERT(!GamePad.isSelectPressed(), "unexpected SELECT");
  ASSERTV(GamePad.getLeftPosition() == 0xFF, "expected 0xFF", GamePad.getLeftPosition());
  ASSERTV(GamePad.getRightPosition() == 0x01, "expected 0x01", GamePad.getRightPosition());
  ASSERTV(GamePad.getUpPosition() == 0x25, "expected 0x25", GamePad.getUpPosition());
  ASSERTV(GamePad.getDownPosition() == 0xF0, "expected 0xF0", GamePad.getDownPosition());

  Serial.println(" Test Down");
  GamePad._clear();
  sendToGamePadProcessInput("L0FR52F02BF1");
  ASSERT(!GamePad.isUpPressed(), "unexpected UP");
  ASSERT(GamePad.isDownPressed(), "expected DOWN");
  ASSERT(!GamePad.isLeftPressed(), "unexpected LEFT");
  ASSERT(!GamePad.isRightPressed(), "unexpected RIGHT");

  Serial.println(" Test Right");
  GamePad._clear();
  sendToGamePadProcessInput("L00R01F00B00");
  ASSERT(!GamePad.isUpPressed(), "unexpected UP");
  ASSERT(!GamePad.isDownPressed(), "unexpected DOWN");
  ASSERT(!GamePad.isLeftPressed(), "unexpected LEFT");
  ASSERT(GamePad.isRightPressed(), "expected RIGHT");

  Serial.println(" Test Stop");
  GamePad._clear();
  sendToGamePadProcessInput("L00R00F00B00");
  ASSERT(!GamePad.isUpPressed(), "unexpected UP");
  ASSERT(!GamePad.isDownPressed(), "unexpected DOWN");
  ASSERT(!GamePad.isLeftPressed(), "unexpected LEFT");
  ASSERT(!GamePad.isRightPressed(), "unexpected RIGHT");
}

void unitTest() {
  Serial.println("************* START OF UNIT TEST RUN ******************");

  assertionFailures = 0;
  testMessageBufferInternals();
  testMessageBufferActionButtons();
  testMessageBufferAnalogPositionDec();\
  testMessageBufferAnalogPositionHex();
  testGamePadInternal();

  if(assertionFailures) {
    Serial.print("FAIL: ");
    Serial.print(assertionFailures);
    Serial.println(" tests failed.");
  } else {
    Serial.println("All tests passed.");
  }
  Serial.println();
  Serial.println("************* END OF UNIT TEST RUN ******************");
}

void loop() {
  // Give the tester time to clear the serial monitor or wait for next iteration.
  delay(5000);
  unitTest();
}