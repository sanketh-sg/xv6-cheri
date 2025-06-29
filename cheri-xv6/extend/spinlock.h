#include <stdint.h>

struct spinlock {
  uint32_t locked;         // Is the lock held?

  // For debugging:
  char * __capability name;    // Name of lock (capability pointer)
  struct cpu * __capability cpu; // The cpu holding the lock (capability pointer)
};