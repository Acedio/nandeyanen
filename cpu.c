#include "cpu.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include "opcode.h"
#include "mapper.h"
#include "examine.h"

#define CYCLES_PER_SCANLINE 341
#define SCANLINES_PER_VSYNC 261

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

  cpu->cycle = 0;
  // TODO: Why does this start at 241?
  cpu->scanline = 241;

  for (int i = 0; i < sizeof(cpu->memory); ++i) {
    cpu->memory.memory[i] = 0x00;
  }
}

uint16_t getAddrOp(const CpuState *cpu, const PrgRom *prgRom, Operation op,
                   uint16_t pc) {
  uint16_t addr = 0xF00D;
  switch (op.addrMode) {
    case A_IMM:
      return pc+1;
    case A_ABS:
      return readWord(&cpu->memory, prgRom, pc+1);
    case A_IND:
      addr = readWord(&cpu->memory, prgRom, pc+1);
      // Indirect addresses wrap when read on a page boundary.
      return readByte(&cpu->memory, prgRom, addr) |
          (readByte(&cpu->memory, prgRom,
                    (addr & 0xFF00) | (addr+1)&0xFF) << 8);
    case A_REL:
      return pc + 2 + (int8_t)readByte(&cpu->memory, prgRom, pc+1);
    case A_ZPG:
      return readByte(&cpu->memory, prgRom, pc+1);
    case A_X_IND:
      // ZPG address of address, no carry.
      addr = (readByte(&cpu->memory, prgRom, pc+1) + cpu->x) & 0xFF;
      return readByte(&cpu->memory, prgRom, addr) |
          (readByte(&cpu->memory, prgRom, (addr+1)&0xFF) << 8);
    case A_IND_Y:
      // ZPG address of address, no carry.
      addr = readByte(&cpu->memory, prgRom, pc+1);
      // Get address from ZPG...
      addr = readByte(&cpu->memory, prgRom, addr) |
          (readByte(&cpu->memory, prgRom, (addr+1)&0xFF) << 8);
      // ... increment by Y.
      return addr + cpu->y;
    default:
      fprintf(stderr, "unsupported addressing mode %s for getAddrOp\n",
              addrModeName(op.addrMode));
      return 0xF00D;
  }
}

uint8_t getByteOp(const CpuState *cpu, const PrgRom *prgRom, Operation op,
                  uint16_t pc) {
  uint16_t addr = getAddrOp(cpu, prgRom, op, pc);
  return readByte(&cpu->memory, prgRom, addr);
}

void cpuStackPush(CpuState *cpu, uint8_t byte) {
  cpu->memory.memory[0x100 + cpu->s] = byte;
  cpu->s -= 1;
}

void cpuStackPushWord(CpuState *cpu, uint16_t word) {
  cpuStackPush(cpu, word >> 8);
  cpuStackPush(cpu, word & 0xff);
}

uint8_t cpuStackPop(CpuState *cpu) {
  cpu->s += 1;
  return cpu->memory.memory[0x100 + cpu->s];
}

uint16_t cpuStackPopWord(CpuState *cpu) {
  uint16_t word = cpuStackPop(cpu);
  word |= cpuStackPop(cpu) << 8;
  return word;
}

void cpuSetNZ(CpuState *cpu, uint8_t result) {
  uint8_t n = result & 0x80 ? F_NEGATIVE : 0;
  uint8_t z = result ? 0 : F_ZERO;
  cpu->status = n | z | (cpu->status & ~(F_NEGATIVE | F_ZERO));
}

// Shift operations are weird in that they're the only ones that use the
// "accumulator" addressing mode. Seems to me like they should just use separate
// opcodes :P
void shiftOp(CpuState *cpu, const PrgRom *prgRom, Operation op, uint16_t pc) {
  uint16_t addr;
  uint8_t byte;

  if (op.addrMode == A_ACC) {
    byte = cpu->a;
  } else {
    addr = getAddrOp(cpu, prgRom, op, pc);
    byte = readByte(&cpu->memory, prgRom, addr);
  }

  uint8_t carry = cpu->status & F_CARRY;

  switch (op.op) {
    case ASL:
      cpu->status = (cpu->status & ~F_CARRY) | (byte & 0x80 ? F_CARRY : 0);
      byte <<= 1;
      break;
    case LSR:
      cpu->status = (cpu->status & ~F_CARRY) | (byte & 0x01 ? F_CARRY : 0);
      byte >>= 1;
      break;
    case ROL:
      cpu->status = (cpu->status & ~F_CARRY) | (byte & 0x80 ? F_CARRY : 0);
      byte = (byte << 1) | (carry ? 0x01 : 0);
      break;
    case ROR:
      cpu->status = (cpu->status & ~F_CARRY) | (byte & 0x01 ? F_CARRY : 0);
      byte = (byte >> 1) | (carry ? 0x80 : 0);
      break;
    default:
      fprintf(stderr, "unsupported shift op %s\n", opName(op.op));
      assert(0);
  }

  cpuSetNZ(cpu, byte);
  
  if (op.addrMode == A_ACC) {
    cpu->a = byte;
  } else {
    writeByte(&cpu->memory, addr, byte);
  }
}

int step(CpuState *cpu, const PrgRom *prgRom) {
  assert(cpu);
  assert(prgRom);

  uint8_t b1 = readByte(&cpu->memory, prgRom, cpu->pc);
  Operation op = opcodes[b1];

  if (op.op == UND) {
    fprintf(stderr, "invalid operation %x\n", b1);
    return 0;
  }

  printState(cpu, prgRom, cpu->pc);

  // Instructions take one clock per byte read or written, as a base.
  // (from awesome resource at https://llx.com/Neil/a2/opcodes.html)
  int clocksTaken = op.cycles;

  uint16_t nextPc = cpu->pc + opLen(op.addrMode);

  // A few intermediary variables to use below.
  uint16_t addr;
  uint8_t byteOp;
  uint8_t result;

  switch (op.op) {
    case NOP:
      break;

    case JMP:
      nextPc = getAddrOp(cpu, prgRom, op, cpu->pc);
      break;
    case JSR:
      cpuStackPushWord(cpu, cpu->pc + 2);
      nextPc = getAddrOp(cpu, prgRom, op, cpu->pc);
      break;
    case RTS:
      nextPc = cpuStackPopWord(cpu) + 1;
      break;
    case RTI:
      cpu->status = (cpu->status & 0x30) | (cpuStackPop(cpu) & ~0x30);
      nextPc = cpuStackPopWord(cpu);
      break;

    case BCC:
      if (!(cpu->status & F_CARRY)) {
        nextPc = getAddrOp(cpu, prgRom, op, cpu->pc);
        clocksTaken += 1;
        if (nextPc & 0xFF00 != cpu->pc) clocksTaken += 1;
      }
      break;
    case BCS:
      if (cpu->status & F_CARRY) {
        nextPc = getAddrOp(cpu, prgRom, op, cpu->pc);
        clocksTaken += 1;
        if (nextPc & 0xFF00 != cpu->pc) clocksTaken += 1;
      }
      break;
    case BEQ:
      if (cpu->status & F_ZERO) {
        nextPc = getAddrOp(cpu, prgRom, op, cpu->pc);
        clocksTaken += 1;
        if (nextPc & 0xFF00 != cpu->pc) clocksTaken += 1;
      }
      break;
    case BNE:
      if (!(cpu->status & F_ZERO)) {
        nextPc = getAddrOp(cpu, prgRom, op, cpu->pc);
        clocksTaken += 1;
        if (nextPc & 0xFF00 != cpu->pc) clocksTaken += 1;
      }
      break;
    case BMI:
      if (cpu->status & F_NEGATIVE) {
        nextPc = getAddrOp(cpu, prgRom, op, cpu->pc);
        clocksTaken += 1;
        if (nextPc & 0xFF00 != cpu->pc) clocksTaken += 1;
      }
      break;
    case BPL:
      if (!(cpu->status & F_NEGATIVE)) {
        nextPc = getAddrOp(cpu, prgRom, op, cpu->pc);
        clocksTaken += 1;
        if (nextPc & 0xFF00 != cpu->pc) clocksTaken += 1;
      }
      break;
    case BVS:
      if (cpu->status & F_OVERFLOW) {
        nextPc = getAddrOp(cpu, prgRom, op, cpu->pc);
        clocksTaken += 1;
        if (nextPc & 0xFF00 != cpu->pc) clocksTaken += 1;
      }
      break;
    case BVC:
      if (!(cpu->status & F_OVERFLOW)) {
        nextPc = getAddrOp(cpu, prgRom, op, cpu->pc);
        clocksTaken += 1;
        if (nextPc & 0xFF00 != cpu->pc) clocksTaken += 1;
      }
      break;

    case LDA:
      cpu->a = getByteOp(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->a);
      break;
    case STA:
      addr = getAddrOp(cpu, prgRom, op, cpu->pc);
      writeByte(&cpu->memory, addr, cpu->a);
      break;
    case LDX:
      cpu->x = getByteOp(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->x);
      break;
    case STX:
      addr = getAddrOp(cpu, prgRom, op, cpu->pc);
      writeByte(&cpu->memory, addr, cpu->x);
      break;
    case LDY:
      cpu->y = getByteOp(cpu, prgRom, op, cpu->pc);
      cpuSetNZ(cpu, cpu->y);
      break;
    case STY:
      addr = getAddrOp(cpu, prgRom, op, cpu->pc);
      writeByte(&cpu->memory, addr, cpu->y);
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
      result = cpu->a + byteOp + (cpu->status & F_CARRY ? 1 : 0);
      // Set overflow if the operands' MSBs are equal and the result's MSB is
      // not the same. Can this be simplified?
      cpu->status = (cpu->status & ~F_OVERFLOW) |
          (!((cpu->a ^ byteOp) & 0x80) &&
           ((result ^ byteOp) & 0x80) ? F_OVERFLOW : 0);
      cpu->a = result;
      cpuSetNZ(cpu, cpu->a);
      cpu->status = (cpu->status & ~F_CARRY) | (cpu->a < byteOp ? F_CARRY : 0);
      break;
    case SBC:
      byteOp = getByteOp(cpu, prgRom, op, cpu->pc);
      result = cpu->a - byteOp - (cpu->status & F_CARRY ? 0 : 1);
      cpu->status = (cpu->status & ~F_OVERFLOW) |
          (((cpu->a ^ byteOp) & 0x80) &&
           ((result ^ cpu->a) & 0x80) ? F_OVERFLOW : 0);
      cpu->status = (cpu->status & ~F_CARRY) | (cpu->a >= byteOp ? F_CARRY : 0);
      cpu->a = result;
      cpuSetNZ(cpu, cpu->a);
      break;

    case INC:
      addr = getAddrOp(cpu, prgRom, op, cpu->pc);
      result = readByte(&cpu->memory, prgRom, addr) + 1;
      writeByte(&cpu->memory, addr, result);
      cpuSetNZ(cpu, result);
      break;
    case INX:
      cpu->x += 1;
      cpuSetNZ(cpu, cpu->x);
      break;
    case INY:
      cpu->y += 1;
      cpuSetNZ(cpu, cpu->y);
      break;
    case DEC:
      addr = getAddrOp(cpu, prgRom, op, cpu->pc);
      result = readByte(&cpu->memory, prgRom, addr) - 1;
      writeByte(&cpu->memory, addr, result);
      cpuSetNZ(cpu, result);
      break;
    case DEX:
      cpu->x -= 1;
      cpuSetNZ(cpu, cpu->x);
      break;
    case DEY:
      cpu->y -= 1;
      cpuSetNZ(cpu, cpu->y);
      break;

    case ASL:
    case LSR:
    case ROL:
    case ROR:
      shiftOp(cpu, prgRom, op, cpu->pc);
      break;

    case TAX:
      cpu->x = cpu->a;
      cpuSetNZ(cpu, cpu->x);
      break;
    case TAY:
      cpu->y = cpu->a;
      cpuSetNZ(cpu, cpu->y);
      break;
    case TXA:
      cpu->a = cpu->x;
      cpuSetNZ(cpu, cpu->x);
      break;
    case TYA:
      cpu->a = cpu->y;
      cpuSetNZ(cpu, cpu->y);
      break;
    case TSX:
      cpu->x = cpu->s;
      cpuSetNZ(cpu, cpu->x);
      break;
    case TXS:
      cpu->s = cpu->x;
      break;

    default:
      printf("unsupported op %s\n", opName(op.op));
      return 0;
  }

  cpu->cycle += clocksTaken * 3;
  if (cpu->cycle >= CYCLES_PER_SCANLINE) {
    cpu->cycle -= CYCLES_PER_SCANLINE;
    cpu->scanline += 1;
    if (cpu->scanline >= SCANLINES_PER_VSYNC) {
      cpu->scanline = -1;
    }
  }

  cpu->pc = nextPc;

  return 1;
}
