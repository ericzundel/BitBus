
#include <Stream.h>
#include <SoftwareSerial.h>

#include "BitBus.h"
#include "GamePad.h"

#if(!defined(__AVR_ATmega328P__))
#error "Only Arduino UNO and Arduino Nano currently supported"
#endif

SoftwareSerial BBSerial;

// Class Constructor
BitBusClass::BitBusClass()
{
  isInit = false;
}
 
// First call to the library
void BitBusClass::begin(unsigned long baudRate, int rx, int tx)
{
  BBSerial.configure(rx, tx);
  BBSerial.begin(baudRate);
}

// Process incoming input from the serial port
void BitBusClass::processInput()
{
  /* This method maintains the interface of the Dabble library, but
   * doesn't work internally like it with a set of module subclasses.
   * Just punt processing over the the GamePad module because that 
   * is all the library suports right now.
   */
  GamePad._processInput();
}

// Private one time initialization
void BitBusClass::init()
{
  if (isInit) {
    return;
  }
  isInit = true;
}
