#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef struct {
  uint8_t a;
  uint8_t x;
  uint8_t y;
  uint16_t pc;
  uint8_t s;
  uint8_t status;

  uint8_t memory[0x800];
  // 0x0800-0x1FFF are mirrors of the 2k memory.
} CpuState;

void initCpu(CpuState *cpu);

int step(CpuState *cpu, const uint8_t *prgRom, int prgLen);

#endif  // CPU_H
