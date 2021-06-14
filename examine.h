#ifndef EXAMINE_H
#define EXAMINE_H

#include <stdint.h>

#include "cpu.h"

// Returns bytes read.
uint16_t printOp(const CpuState *cpu, const PrgRom *rom, uint16_t pc);

// Returns bytes read.
uint16_t printOps(const CpuState *cpu, const PrgRom *rom, uint16_t pc, int numOps);

uint16_t printState(const CpuState *cpu, const PrgRom *rom, uint16_t pc);

void examineRom(const CpuState *cpu, const PrgRom *rom, uint16_t pc);

#endif  // EXAMINE_H
