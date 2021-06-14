#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef struct {
  // 0x0800-0x1FFF are mirrors of the 2k memory.
  uint8_t memory[0x800];
} Memory;

#endif  // MEMORY_H
