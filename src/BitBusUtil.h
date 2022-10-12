#ifndef BitBusUtil_h
#define BitBusUtil_h
#include <avr/pgmspace.h>

/** Compare values For unit testing */
#define ASSERT(p1, p2)      Assert((p1), PSTR(p2))
#define ASSERTV(p1, p2, p3)  Assert((p1), PSTR(p2), (p3))

/** Store and print a string in flash memory.*/
#define DebugPrint(x)       SerialPrint_P(PSTR(x))

/** Store and print a string in flash memory followed by a CR/LF.*/
#define DebugPrintln(x)     SerialPrintln_P(PSTR(x))


int Assert(bool assertionValue, PGM_P errorMsg);
int Assert(bool assertionValue, PGM_P errorMsg, int value);

void SerialPrint_P(PGM_P str);
void SerialPrintln_P(PGM_P str);

// TODO(ericzundel): ugly extern, wrap into Assertion class?
extern int assertionFailures;
#endif // WaveUtil_h
