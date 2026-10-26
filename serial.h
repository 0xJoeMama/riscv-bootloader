#ifndef SERIAL_H
#define SERIAL_H

#include "print.h"

// TODO: this whole file currently assumes the location of a serial interface
// which is clearly the wrong thing
extern sprinter serial;
// BYPASS CONCURRENCY SAFETY FOR THE SAKE OF EMERGENCY PRINTING
// MEANT TO BE USED TO REPORT HIGH PRIORITY FAILURES LIKE PANIC
extern sprinter emergency_serial;

#endif // SERIAL_H
