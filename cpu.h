#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#include "memory.h"
#include "mapper.h"

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
