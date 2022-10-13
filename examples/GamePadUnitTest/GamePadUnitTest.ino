#include <BitBusUtil.h>
#include <GamePad.h>
#include <MessageBuffer.h>

/*
 * Unit tests for 
   Interface Based on: https://thestempedia.com/docs/dabble/game-pad-module/
*/
void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);      // make sure your Serial Monitor is also set at this baud rate.
  BitBus.begin(9600);      //Enter baudrate of your bluetooth.Connect bluetooth on Bluetooth port present on evive.
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

  Serial.println(" Test MT_TRIANGLE");
  mb.clear();
  ASSERTV(mb.messageType == MT_UNKNOWN, "expected MT_UNKNOWN", mb.messageType);
  ASSERTV(mb.inputState == IS_START, "expected IS_START", mb.inputState);
  mb.processInput('B');
  ASSERTV(mb.messageType == MT_TRIANGLE, "expected MT_TRIANGLE", mb.messageType);
  ASSERTV(mb.inputState == IS_MESSAGE_READY, "expected IS_MESSAGE_READY", mb.inputState);

  Serial.println(" Test MT_CIRCLE");
  mb.clear();
  mb.processInput('Y');
  ASSERTV(mb.messageType == MT_CIRCLE, "expected MT_CIRCLE", mb.messageType);
  ASSERTV(mb.inputState == IS_MESSAGE_READY, "expected IS_MESSAGE_READY", mb.inputState);

  Serial.println(" Test MT_CROSS");
  mb.clear();
  mb.processInput('X');
  ASSERTV(mb.messageType == MT_CROSS, "expected MT_CROSS", mb.messageType);
  ASSERTV(mb.inputState == IS_MESSAGE_READY, "expected IS_MESSAGE_READY", mb.inputState);

  Serial.println(" Test MT_SQUARE");
  mb.clear();
  mb.processInput('A');
  ASSERTV(mb.messageType == MT_SQUARE, "expected MT_SQUARE", mb.messageType);
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
  ASSERT(!GamePad.isSquarePressed(), "unexpected SQUARE");
  ASSERT(!GamePad.isCirclePressed(), "unexpected CIRCLE");
  ASSERT(!GamePad.isCrossPressed(), "unexpected CROSS");
  ASSERT(!GamePad.isTrianglePressed(), "unexpected TRIANGLE");
  ASSERT(!GamePad.isStartPressed(), "unexpected START");
  ASSERT(!GamePad.isSelectPressed(), "unexpected SELECT");
    Serial.println(" Testing _clear");

  Serial.println(" Testing Start button");
  sendToGamePadProcessInput("S");
  ASSERT(!GamePad.isUpPressed(), "unexpected UP");
  ASSERT(!GamePad.isDownPressed(), "unexpected DOWN");
  ASSERT(!GamePad.isLeftPressed(), "unexpected LEFT");
  ASSERT(!GamePad.isRightPressed(), "unexpected RIGHT");
  ASSERT(!GamePad.isSquarePressed(), "unexpected SQUARE");
  ASSERT(!GamePad.isCirclePressed(), "unexpected CIRCLE");
  ASSERT(!GamePad.isCrossPressed(), "unexpected CROSS");
  ASSERT(!GamePad.isTrianglePressed(), "unexpected TRIANGLE");
  ASSERT(GamePad.isStartPressed(), "expected START");
  ASSERT(!GamePad.isSelectPressed(), "unexpected SELECT");

  // Nothing should be set
  GamePad._clear();
  ASSERT(!GamePad.isUpPressed(), "unexpected UP");
  ASSERT(!GamePad.isDownPressed(), "unexpected DOWN");
  ASSERT(!GamePad.isLeftPressed(), "unexpected LEFT");
  ASSERT(!GamePad.isRightPressed(), "unexpected RIGHT");
  ASSERT(!GamePad.isSquarePressed(), "unexpected SQUARE");
  ASSERT(!GamePad.isCirclePressed(), "unexpected CIRCLE");
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
