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
    cpu->memory.memory[i] = 0xff;
  }
}

uint16_t readAddr(Memory *memory, const PrgRom *prgRom, Operation op,
                  uint16_t at) {
  uint16_t addr = 0xF00D;
  switch (op.addrMode) {
    case A_ABS:
      return readWord(memory, prgRom, at);
    case A_IND:
      at = readWord(memory, prgRom, at);
      return readWord(memory, prgRom, at);
    default:
      printf("unsupported addressing mode %s\n", addrModeName(op.addrMode));
      return 0xF00D;
  }
}

int step(CpuState *cpu, const PrgRom *prgRom) {
  assert(cpu);
  assert(prgRom);

  uint8_t b1 = readMem(&cpu->memory, prgRom, cpu->pc);
  Operation op = opcodes[b1];

  if (op.op == UND) {
    printf("invalid operation %x\n", b1);
    return 0;
  }

  switch (op.op) {
    case JMP:
      cpu->pc = readAddr(&cpu->memory, prgRom, op, cpu->pc + 1);
      printf("pc set to %x\n", cpu->pc);
      break;
    default:
      printf("unsupported op %s\n", opName(op.op));
      return 0;
  }

  return 1;
}
