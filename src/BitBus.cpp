/*
 * BitBusClass: Main module for handling input from the BitBus App
 */
#include <Stream.h>
#include <SoftwareSerial.h>

#include "BitBus.h"
#include "GamePad.h"

#if(!defined(__AVR__))
// This may work on other architectures, I just don't have one to try it
#error "Only Arduino AVR currently supported"
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

/**
 * Process incoming input from the serial port
 *  Note that the GamePadaction buttons are cleared before processing any new input.
 * The up/down/left/right buttons are persistent because we are emulating them
 * using Analog Mode.
 */
void BitBusClass::processInput()
{

  GamePad._clearActionButtons();

  while (bbSerial->available()) {
  /* Just punt processing over the the GamePad module because that
   * is all the library suports right now.
   */
    GamePad._processInput(bbSerial->read());
  }
}
