#ifndef SERIAL_H
#define SERIAL_H

#include "print.h"

extern sprinter serial;
// BYPASS CONCURRENCY SAFETY FOR THE SAKE OF EMERGENCY PRINTING
// MEANT TO BE USED TO REPORT HIGH PRIORITY FAILURES LIKE PANIC
extern sprinter emergency_serial;

#endif // SERIAL_H
