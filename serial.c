#include "serial.h"
#include "assert.h"
#include "spinlock.h"
#include "std.h"

#define SERIAL_ADDR ((char *)0x10000000LU)

static void print_char_serial(char c) { *SERIAL_ADDR = c; }

// only one process may use the serial interface at a time
static Spinlock serial_lock = {0};

static void print_serial(const char *c) {
  assert_held(&serial_lock);
  while (*c) {
    *SERIAL_ADDR = *c++;
  }
}

static int serial_prepare(void *arg) {
  spinlock_lock(&serial_lock);
  return 0;
}

static int serial_flush(void *arg) {
  spinlock_unlock(&serial_lock);
  return 0;
}

sprinter serial = {0};

void serial_init() {
  // nothing
  serial.print = print_serial;
  serial.prepared = (atomic_u32){0};
  serial.print_c = print_char_serial;

  serial.prepare = serial_prepare;
  serial.flush = serial_flush;

  serial.arg = NULL;
}
