#include "serial.h"
#include "assert.h"
#include "spinlock.h"
#include "std.h"

#define SERIAL_ADDR ((char *)0x10000000LU)

// only one process may use the serial interface at a time
static Spinlock serial_lock = {0};

static void serial_print_c(char c) {
  assert_held(&serial_lock);
  *SERIAL_ADDR = c;
}

static void serial_printn(const char *s, usize n) {
  assert_held(&serial_lock);
  for (usize i = 0; i < n; i++)
    *SERIAL_ADDR = *s++;
}

static void serial_print(const char *c) {
  assert_held(&serial_lock);
  while (*c)
    *SERIAL_ADDR = *c++;
}

static int serial_prepare(void *arg) {
  spinlock_lock(&serial_lock);
  return 0;
}

static int serial_flush(void *arg) {
  spinlock_unlock(&serial_lock);
  return 0;
}

sprinter serial = {
    .print = serial_print,
    .print_c = serial_print_c,
    .printn = serial_printn,

    .prepare = serial_prepare,
    .flush = serial_flush,
};

static void eserial_print_c(char c) { *SERIAL_ADDR = c; }

static void eserial_printn(const char *s, usize n) {
  for (usize i = 0; i < n; i++)
    *SERIAL_ADDR = *s++;
}

static void eserial_print(const char *c) {
  while (*c)
    *SERIAL_ADDR = *c++;
}

sprinter emergency_serial = {
    .print = eserial_print,
    .print_c = eserial_print_c,
    .printn = eserial_printn,
};
