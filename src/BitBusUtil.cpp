#include <BitBusUtil.h>
#include "Arduino.h"

int assertionFailures = 0;

// Returns: 0 on success, non zero if assertion fails
int Assert(bool assertionValue, PGM_P errorMsg) {
  if (!assertionValue) {
    SerialPrint_P(errorMsg);
    assertionFailures++;
    return -1;
  }
  return 0;
}

// Returns: 0 on success, non zero if assertion fails
int Assert(bool assertionValue, PGM_P errorMsg, int value) {
  int result = Assert(assertionValue, errorMsg);
  if (result) {
    SerialPrint_P(PSTR(" Assertion Value: "));
    Serial.println(value);
  }
  return result;
}

//------------------------------------------------------------------------------
/**
 * %Print a string in flash memory to the serial port.
 *
 * \param[in] str Pointer to string stored in flash memory.
 */
void SerialPrint_P(PGM_P str) {
  for (uint8_t c; (c = pgm_read_byte(str)); str++)
    Serial.write(c);
}
//------------------------------------------------------------------------------
/**
 * %Print a string in flash memory followed by a CR/LF.
 *
 * \param[in] str Pointer to string stored in flash memory.
 */
void SerialPrintln_P(PGM_P str) {
  SerialPrint_P(str);
  Serial.println();
}
