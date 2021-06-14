#include "cpu.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include "opcode.h"
#include "mapper.h"
#include "examine.h"

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

uint8_t cpuReadByte(CpuState *cpu, const PrgRom *prgRom, Operation op,
                 uint16_t pc) {
  uint8_t addr = 0x55;
  switch (op.addrMode) {
    case A_IMM:
      return readByte(&cpu->memory, prgRom, pc+1);
    default:
      printf("unsupported addressing mode %s\n", addrModeName(op.addrMode));
      return 0x55;
  }
}

void cpuWriteByte(CpuState *cpu, const PrgRom *prgRom, Operation op,
                  uint16_t pc, uint8_t byte) {
  switch (op.addrMode) {
    case A_ZPG:
      uint16_t addr = readByte(&cpu->memory, prgRom, pc+1);
      writeByte(&cpu->memory, prgRom, addr, byte);
      break;
    default:
      printf("unsupported addressing mode %s\n", addrModeName(op.addrMode));
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

  uint8_t b1 = readByte(&cpu->memory, prgRom, cpu->pc);
  Operation op = opcodes[b1];

  if (op.op == UND) {
    printf("invalid operation %x\n", b1);
    return 0;
  }

  printState(cpu, prgRom, cpu->pc);

  uint16_t nextPc = cpu->pc + opLen(op.addrMode);

  switch (op.op) {
    case JMP:
      nextPc = readAddr(&cpu->memory, prgRom, op, cpu->pc + 1);
      break;
    case LDX:
      cpu->x = cpuReadByte(cpu, prgRom, op, cpu->pc);
      break;
    case STX:
      cpuWriteByte(cpu, prgRom, op, cpu->pc, cpu->x);
      break;
    default:
      printf("unsupported op %s\n", opName(op.op));
      return 0;
  }

  cpu->pc = nextPc;

  return 1;
}
