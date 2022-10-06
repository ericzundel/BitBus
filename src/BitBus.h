/**
 * This library is for communication with the BitBus app.
 *
 * Author: Eric Z. Ayers <ericzundel@gmail.com>
 * Date: October 5, 2022
 *
 * Currently only SoftwareSerial is supported on pins 2 and 3.
 * 
 * The software interface is meant to be very similar to the Dabble
 * software interface for easy portability, even though the protocols
 * between the App and the microcontroller seem to be very different.
 *
 * This is licensed under GNU GPL V3 [http://www.gnu.org/licenses/gpl.txt].
 * See ../LICENSE
 */

#ifndef BitBus_h
#define BitBus_h

#include "Arduino.h"
#include "Stream.h"

class BitBusClass
{
public:
  BitBusClass();
  
  // Library Initialization
  void begin(unsigned long baudRate=9600, int rx=2, int tx=3);
  // Processing Incomming Frames
  void processInput();
  
private:
  void init();
  static bool isInit;
  Stream * bbSerial;
};

// Extern Object
extern BitBusClass BitBus;
#endif
