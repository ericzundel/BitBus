/*
 * GamePadDemo: Demonstration of the BitBus App using the Controller in Analog Mode.
 *
 * Implements the interface based on Dabble: https://thestempedia.com/docs/dabble/game-pad-module/
 *
 * To run this demo:
 *   - Connect your Bluetooth Module to digital pins 2 & 3 on the Arduino.
 *   - Install the BitBlue BitBus app on your phone.
 *   - Press "Scan" to connect to the Bluetooth device
 *   - Choose "Controller" when the device is recognized
 *   - Click the "Mode" button in the app to switch into analog joystick mode.
 */
#include <BitBus.h>
#include <GamePad.h>


void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);     // Make sure your Serial Monitor is also set at this baud rate.
  BitBus.begin(9600);      // Enter baudrate of your bluetooth.
}

void loop() {
  delay(100);
  BitBus.processInput();             // This function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.

  // After you process input, you have one chance to read the action buttons.
  // The up/down/left/right buttons are sticky and will keep reading until you let go of the joystick

  // Probe the GamePad interface to see what was pressed
  Serial.print("KeyPressed: ");
  if (GamePad.isUpPressed())
  {
    Serial.print("UP ");
  }

  if (GamePad.isDownPressed())
  {
    Serial.print("DOWN ");
  }

  if (GamePad.isLeftPressed())
  {
    Serial.print("LEFT ");
  }

  if (GamePad.isRightPressed())
  {
    Serial.print("RIGHT ");
  }

  if (GamePad.isAPressed()) {
    Serial.print("A ");
  }

  if (GamePad.isBPressed()) {
    Serial.print("B ");
  }

  if (GamePad.isXPressed()) {
    Serial.print("X ");
  }

  if (GamePad.isYPressed()) {
    Serial.print("Y ");
  }

  // Dabble Compatibility mode functions
  if (GamePad.isSquarePressed())
  {
    Serial.print("Square ");
  }

  if (GamePad.isCirclePressed())
  {
    Serial.print("Circle ");
  }

  if (GamePad.isCrossPressed())
  {
    Serial.print("Cross ");
  }

  if (GamePad.isTrianglePressed())
  {
    Serial.print("Triangle ");
  }

  if (GamePad.isStartPressed())
  {
    Serial.print("Start");
  }

  if (GamePad.isSelectPressed())
  {
    Serial.print("Select");
  }
  Serial.print('\t');

  // Unimplemented Dabble compatibility mode functions
#if 0
  int a = GamePad.getAngle();
  Serial.print("Angle: ");
  Serial.print(a);
  Serial.print('\t');
  int b = GamePad.getRadius();
  Serial.print("Radius: ");
  Serial.print(b);
  Serial.print('\t');
  float c = GamePad.getXaxisData();
  Serial.print("x_axis: ");
  Serial.print(c);
  Serial.print('\t');
  float d = GamePad.getYaxisData();
  Serial.print("y_axis: ");
  Serial.print(d);
#endif

  Serial.print(" Left: ");
  Serial.print(GamePad.getLeftPosition());
  Serial.print(" Right: ");
  Serial.print(GamePad.getRightPosition());
  Serial.print(" Up: ");
  Serial.print(GamePad.getUpPosition());
  Serial.print(" Down: ");
  Serial.print(GamePad.getDownPosition());
  Serial.println();

}
