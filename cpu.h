#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#include "memory.h"
#include "mapper.h"

enum StatusFlags {
  F_CARRY = 0x01,
  F_ZERO = 0x02,
  F_INTDIS = 0x04,
  F_DECIMAL = 0x08,
  // Unused = 0x10,
  // Unused = 0x20,
  F_OVERFLOW = 0x40,
  F_NEGATIVE = 0x80,
};

typedef struct {
  uint8_t a;
  uint8_t x;
  uint8_t y;
  uint16_t pc;
  uint8_t s;
  uint8_t status;

  Memory memory;
} CpuState;

void initCpu(CpuState *cpu);

int step(CpuState *cpu, const PrgRom *prgRom);

#endif  // CPU_H
