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
    cpu->memory.memory[i] = 0x00;
  }
}

uint8_t getByteOp(CpuState *cpu, const PrgRom *prgRom, Operation op,
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

uint16_t getAddrOp(const Memory *memory, const PrgRom *prgRom, Operation op,
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

void cpuStackPushWord(CpuState *cpu, uint16_t word) {
  cpuStackPush(cpu, word & 0xff);
  cpuStackPush(cpu, word >> 8);
}

uint8_t cpuStackPop(CpuState *cpu) {
  cpu->s += 1;
  return cpu->memory.memory[0x100 + cpu->s];
}

uint16_t cpuStackPopWord(CpuState *cpu) {
  uint16_t word = cpuStackPop(cpu);
  word <<= 8;
  word |= cpuStackPop(cpu);
  return word;
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

  uint8_t byteOp;

  switch (op.op) {
    case NOP:
      break;

    case JMP:
      nextPc = getAddrOp(&cpu->memory, prgRom, op, cpu->pc);
      break;
    case JSR:
      // TODO: nextPc might not be correct, seems like it might be nextPc-1.
      cpuStackPushWord(cpu, nextPc);
      nextPc = getAddrOp(&cpu->memory, prgRom, op, cpu->pc);
      break;
    case RTS:
      nextPc = cpuStackPopWord(cpu);
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
      cpu->a = getByteOp(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->a);
      break;
    case STA:
      cpuWriteByte(cpu, prgRom, op, cpu->pc, cpu->a);
      break;
    case LDX:
      cpu->x = getByteOp(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->x);
      break;
    case STX:
      cpuWriteByte(cpu, prgRom, op, cpu->pc, cpu->x);
      break;
    case LDY:
      cpu->y = getByteOp(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->y);
      break;
    case STY:
      cpuWriteByte(cpu, prgRom, op, cpu->pc, cpu->y);
      break;

    case PHA:
      cpuStackPush(cpu, cpu->a);
      break;
    case PHP:
      // Bit 5 and 4 are set to 1.
      cpuStackPush(cpu, cpu->status | 0x30);
      break;
    case PLA:
      cpu->a = cpuStackPop(cpu);
      cpuSetNZ(cpu, cpu->a);
      break;
    case PLP:
      // Don't change bit 5 and 4.
      cpu->status = (cpu->status & 0x30) | (cpuStackPop(cpu) & ~0x30);
      break;

    case BIT:
      byteOp = getByteOp(cpu, prgRom, op, cpu->pc);
      uint8_t z = byteOp & cpu->a ? 0 : F_ZERO;
      // Copy N and V to status flags.
      cpu->status = (byteOp & (F_OVERFLOW | F_NEGATIVE)) | z |
                    (cpu->status & ~(F_OVERFLOW | F_NEGATIVE | F_ZERO));
      break;
    case CMP:
      byteOp = getByteOp(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->a - byteOp);
      cpu->status = (cpu->status & ~F_CARRY) | (cpu->a >= byteOp ? F_CARRY : 0);
      break;
    case CPX:
      byteOp = getByteOp(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->x - byteOp);
      cpu->status = (cpu->status & ~F_CARRY) | (cpu->x >= byteOp ? F_CARRY : 0);
      break;
    case CPY:
      byteOp = getByteOp(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->y - byteOp);
      cpu->status = (cpu->status & ~F_CARRY) | (cpu->y >= byteOp ? F_CARRY : 0);
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

    case AND:
      cpu->a &= getByteOp(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->a);
      break;
    case ORA:
      cpu->a |= getByteOp(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->a);
      break;
    case EOR:
      cpu->a ^= getByteOp(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->a);
      break;
    case ADC:
      byteOp = getByteOp(cpu, prgRom, op, cpu->pc);
      uint8_t result = cpu->a + byteOp + (cpu->status & F_CARRY ? 1 : 0);
      // Set overflow if the operands' MSBs are equal and the result's MSB is
      // not the same. Can this be simplified?
      cpu->status = (cpu->status & ~F_OVERFLOW) |
          (!((cpu->a ^ byteOp) & 0x80) &&
           ((result ^ byteOp) & 0x80) ? F_OVERFLOW : 0);
      cpu->a = result;
      cpuSetNZ(cpu, cpu->a);
      cpu->status = (cpu->status & ~F_CARRY) | (cpu->a < byteOp ? F_CARRY : 0);
      break;

    default:
      printf("unsupported op %s\n", opName(op.op));
      return 0;
  }

  cpu->pc = nextPc;

  return 1;
}
