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
  // TODO: This should actually be 0x34 but for the tests bit 0x10 is not set.
  cpu->status = 0x24;

  for (int i = 0; i < sizeof(cpu->memory); ++i) {
    cpu->memory.memory[i] = 0xff;
  }
}

uint8_t cpuReadByte(CpuState *cpu, const PrgRom *prgRom, Operation op,
                    uint16_t pc) {
  switch (op.addrMode) {
    case A_IMM:
      return readByte(&cpu->memory, prgRom, pc+1);
    case A_ZPG:
      return cpu->memory.memory[readByte(&cpu->memory, prgRom, pc+1)];
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

uint16_t getAddrOp(Memory *memory, const PrgRom *prgRom, Operation op,
                   uint16_t pc) {
  uint16_t addr = 0xF00D;
  switch (op.addrMode) {
    case A_ABS:
      return readWord(memory, prgRom, pc+1);
    case A_IND:
      pc = readWord(memory, prgRom, pc+1);
      return readWord(memory, prgRom, pc);
    case A_REL:
      return pc + 2 + (int8_t)readByte(memory, prgRom, pc+1);
    default:
      printf("unsupported addressing mode %s\n", addrModeName(op.addrMode));
      return 0xF00D;
  }
}

void cpuStackPush(CpuState *cpu, uint8_t byte) {
  cpu->memory.memory[0x100 + cpu->s] = byte;
  cpu->s -= 1;
}

void cpuSetNZ(CpuState *cpu, uint8_t result) {
  uint8_t n = result & 0x80 ? F_NEGATIVE : 0;
  uint8_t z = result ? 0 : F_ZERO;
  cpu->status = n | z | (cpu->status & ~(F_NEGATIVE | F_ZERO));
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
    case NOP:
      break;

    case JMP:
      nextPc = getAddrOp(&cpu->memory, prgRom, op, cpu->pc);
      break;
    case JSR:
      // TODO: nextPc might not be correct, seems like it might be nextPc-1.
      cpuStackPush(cpu, nextPc & 0xff);
      cpuStackPush(cpu, nextPc >> 8);
      nextPc = getAddrOp(&cpu->memory, prgRom, op, cpu->pc);
      break;

    case BCC:
      if (!(cpu->status & F_CARRY)) {
        nextPc = getAddrOp(&cpu->memory, prgRom, op, cpu->pc);
      }
      break;
    case BCS:
      if (cpu->status & F_CARRY) {
        nextPc = getAddrOp(&cpu->memory, prgRom, op, cpu->pc);
      }
      break;
    case BEQ:
      if (cpu->status & F_ZERO) {
        nextPc = getAddrOp(&cpu->memory, prgRom, op, cpu->pc);
      }
      break;
    case BNE:
      if (!(cpu->status & F_ZERO)) {
        nextPc = getAddrOp(&cpu->memory, prgRom, op, cpu->pc);
      }
      break;
    case BMI:
      if (cpu->status & F_NEGATIVE) {
        nextPc = getAddrOp(&cpu->memory, prgRom, op, cpu->pc);
      }
      break;
    case BPL:
      if (!(cpu->status & F_NEGATIVE)) {
        nextPc = getAddrOp(&cpu->memory, prgRom, op, cpu->pc);
      }
      break;
    case BVS:
      if (cpu->status & F_OVERFLOW) {
        nextPc = getAddrOp(&cpu->memory, prgRom, op, cpu->pc);
      }
      break;
    case BVC:
      if (!(cpu->status & F_OVERFLOW)) {
        nextPc = getAddrOp(&cpu->memory, prgRom, op, cpu->pc);
      }
      break;

    case LDA:
      cpu->a = cpuReadByte(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->a);
      break;
    case STA:
      cpuWriteByte(cpu, prgRom, op, cpu->pc, cpu->a);
      break;
    case LDX:
      cpu->x = cpuReadByte(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->x);
      break;
    case STX:
      cpuWriteByte(cpu, prgRom, op, cpu->pc, cpu->x);
      break;
    case LDY:
      cpu->y = cpuReadByte(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->y);
      break;
    case STY:
      cpuWriteByte(cpu, prgRom, op, cpu->pc, cpu->y);
      break;

    case BIT:
      uint8_t operand = cpuReadByte(cpu, prgRom, op, cpu->pc);
      uint8_t z = operand & cpu->a ? 0 : F_ZERO;
      // Copy N and V to status flags.
      cpu->status = (operand & (F_OVERFLOW | F_NEGATIVE)) | z |
                    (cpu->status & ~(F_OVERFLOW | F_NEGATIVE | F_ZERO));
      break;

    case SEC:
      cpu->status |= F_CARRY;
      break;
    case CLC:
      cpu->status &= ~F_CARRY;
      break;
    case SED:
      cpu->status |= F_DECIMAL;
      break;
    case CLD:
      cpu->status &= ~F_DECIMAL;
      break;
    case SEI:
      cpu->status |= F_INTDIS;
      break;
    case CLI:
      cpu->status &= ~F_INTDIS;
      break;
    case CLV:
      cpu->status &= ~F_OVERFLOW;
      break;

    default:
      printf("unsupported op %s\n", opName(op.op));
      return 0;
  }

  cpu->pc = nextPc;

  return 1;
}
