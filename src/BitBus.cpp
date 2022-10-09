
#include <Stream.h>
#include <SoftwareSerial.h>

#include "BitBus.h"
#include "GamePad.h"

#if(!defined(__AVR_ATmega328P__))
#error "Only Arduino UNO and Arduino Nano currently supported"
#endif

// Singleton to communicate with BitBus app
BitBusClass BitBus;

// Class Constructor
BitBusClass::BitBusClass()
{
  bbSerial = NULL;
}
 
// First call to the library
void BitBusClass::begin(unsigned long baudRate, int rx, int tx)
{
  // Check to see if we are re-starting the same instance
  if (bbSerial) {
    free(bbSerial);
  }
  // TODO(ericzundel) Get rid of dynamic allocation
  SoftwareSerial *newSerial = new SoftwareSerial(rx, tx);
  newSerial->begin(baudRate);
  bbSerial = newSerial;
}

// Process incoming input from the serial port
void BitBusClass::processInput()
{
  if (bbSerial->available()) {
  /* Just punt processing over the the GamePad module because that 
   * is all the library suports right now.
   */
    GamePad._processInput(bbSerial->read());
  }
}
