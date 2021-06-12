#include "cpu.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include "opcode.h"

void initCpu(CpuState *cpu) {
  assert(cpu);
  // http://wiki.nesdev.com/w/index.php/CPU_power_up_state
  cpu->a = 0;
  cpu->x = 0;
  cpu->y = 0;
  // TODO: What is this actually? Do we go through the reset vector?
  cpu->pc = 0xc000;
  cpu->s = 0xfd;
  cpu->status = 0x34;

  for (int i = 0; i < sizeof(cpu->memory); ++i) {
    cpu->memory[i] = 0xff;
  }
}

// TODO: This depends on mapper.
int pcToAddr(int pc) {
  return (pc - 0x8000) % 0x4000;
}

int step(CpuState *cpu, const uint8_t *prgRom, int prgLen) {
  assert(cpu);
  assert(prgRom);

  int prgAddr = pcToAddr(cpu->pc);
  assert(prgAddr < prgLen && prgAddr >= 0);
  uint8_t b1 = *(prgRom + prgAddr);
  Operation op = opcodes[b1];

  if (op.op == UND) {
    printf("invalid operation %x\n", b1);
    return 0;
  }
  uint16_t addr = 0xF00D;
  switch (op.addrMode) {
    case A_ABS:
      addr = prgRom[prgAddr + 2];
      addr <<= 8;
      addr += prgRom[prgAddr + 1];
      break;
    default:
      printf("unsupported addressing mode %s\n", addrModeName(op.addrMode));
      return 0;
  }

  switch (op.op) {
    case JMP:
      cpu->pc = addr;
      printf("pc set to %x\n", cpu->pc);
      break;
    default:
      printf("unsupported op %s\n", opName(op.op));
      return 0;
  }

  return 1;
}
