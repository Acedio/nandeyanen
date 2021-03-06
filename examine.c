#include "examine.h"

#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include "opcode.h"
#include "memory.h"
#include "mapper.h"

uint16_t printOp(const CpuState* cpu, const PrgRom* rom, uint16_t pc) {
  assert(rom);

  uint8_t b1 = readByte(&cpu->memory, rom, pc);
  uint8_t b2 = readByte(&cpu->memory, rom, pc+1);
  uint8_t b3 = readByte(&cpu->memory, rom, pc+2);

  Operation op = opcodes[b1];
  int addrMode = opcodes[b1].addrMode;

  uint16_t len = opLen(addrMode);
  switch (len) {
    case 1:
      printf("%02X        ", b1);
      break;
    case 2:
      printf("%02X %02X     ", b1, b2);
      break;
    case 3:
      printf("%02X %02X %02X  ", b1, b2, b3);
      break;
    default:
      printf("bad op: %02X\n", b1);
      return -1;
  }

  printf("%s ", opName(op.op));

  char opStr[29];

  uint16_t addr = 0xF00D;
  switch (addrMode) {
    case A_ABS:
      addr = getAddrOp(cpu, rom, op, pc);
      if (op.op == JMP || op.op == JSR) {
        snprintf(opStr, sizeof(opStr), "$%04X", addr);
      } else {
        snprintf(opStr, sizeof(opStr), "$%04X = %02X ", addr, readByte(&cpu->memory, rom, addr));
      }
      break;
    case A_ABS_X:
      addr = getAddrOp(cpu, rom, op, pc);
      snprintf(opStr, sizeof(opStr), "$%02X%02X,X @ %04X = %02X", b3, b2, addr,
            readByte(&cpu->memory, rom, addr));
      break;
    case A_ABS_Y:
      addr = getAddrOp(cpu, rom, op, pc);
      snprintf(opStr, sizeof(opStr), "$%02X%02X,Y @ %04X = %02X", b3, b2, addr,
            readByte(&cpu->memory, rom, addr));
      break;
    case A_ACC:
      snprintf(opStr, sizeof(opStr), "A");
      break;
    case A_IMM:
      snprintf(opStr, sizeof(opStr), "#$%02X", b2);
      break;
    case A_IMPL:
      snprintf(opStr, sizeof(opStr), "");
      break;
    case A_IND:
      addr = getAddrOp(cpu, rom, op, pc);
      snprintf(opStr, sizeof(opStr), "($%02X%02X) = %04X", b3, b2, addr);
      break;
    case A_IND_Y:
      addr = getAddrOp(cpu, rom, op, pc);
      // Y = addr from ZPG @ addr + Y = value
      snprintf(opStr, sizeof(opStr), "($%02X),Y = %04X @ %04X = %02X", b2,
             readByte(&cpu->memory, rom, b2)
             | (readByte(&cpu->memory, rom, (b2+1)&0xFF) << 8), addr,
             readByte(&cpu->memory, rom, addr));
      break;
    case A_REL:
      snprintf(opStr, sizeof(opStr), "$%04X", pc + 2 + (int8_t)b2);
      break;
    case A_X_IND:
      addr = getAddrOp(cpu, rom, op, pc);
      snprintf(opStr, sizeof(opStr), "($%02X,X) @ %02X = %04X = %02X", b2, (b2 + cpu->x)&0xFF, addr,
             readByte(&cpu->memory, rom, addr));
      break;
    case A_ZPG:
      snprintf(opStr, sizeof(opStr), "$%02X = %02X", b2, readByte(&cpu->memory, rom, b2));
      break;
    case A_ZPG_X:
      addr = getAddrOp(cpu, rom, op, pc);
      snprintf(opStr, sizeof(opStr), "$%02X,X @ %02X = %02X", b2, addr,
               readByte(&cpu->memory, rom, addr));
      break;
    case A_ZPG_Y:
      addr = getAddrOp(cpu, rom, op, pc);
      snprintf(opStr, sizeof(opStr), "$%02X,Y @ %02X = %02X", b2, addr,
               readByte(&cpu->memory, rom, addr));
      break;
    default:
      snprintf(opStr, sizeof(opStr), "Unsupported address mode!\n");
  }
  printf("%-28s", opStr);

  return len;
}

// Returns bytes read.
uint16_t printOps(const CpuState *cpu, const PrgRom* rom, uint16_t pc, int numOps) {
  uint16_t start = pc;
  for (; numOps > 0; --numOps) {
    int b = printOp(cpu, rom, pc);
    printf("\n");
    if (b < 1) {
      // Skip to the next op if this one was invalid.
      pc += 1;
    } else {
      pc += b;
    }
  }
  return pc - start;
}

void printCpuState(const CpuState *cpu) {
  printf("A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%3d SL:%d", cpu->a, cpu->x,
         cpu->y, cpu->status, cpu->s, cpu->cycle, cpu->scanline);
}

uint16_t printState(const CpuState *cpu, const PrgRom *rom, uint16_t pc) {
  printf("%04X  ", cpu->pc);
  printOp(cpu, rom, pc);
  printCpuState(cpu);
  printf("\n");
}

int isWhitespace(char c) {
  return c == ' ' || c == '\n' || c == '\t';
}

char getCommand() {
  int c = ' ';
  while (isWhitespace(c)) {
    c = fgetc(stdin);
  }
  return c;
}

int getInt(int *out) {
  int r = scanf("%d", out);
  if (r != 1) {
    return 0;
  }
  return 1;
}

int getHex(int *out) {
  int r = scanf("%x", out);
  if (r != 1) {
    return 0;
  }
  return 1;
}

void examineRom(const CpuState *cpu, const PrgRom *rom, uint16_t pc) {
  while (1) {
    printf("; %04X\n", pc);
    if (feof(stdin)) {
      break;
    }
    uint16_t bytesRead = printOps(cpu, rom, pc, 10);
    char cmd = getCommand();
    if (cmd == 'q') {
      break;
    }
    int param;
    switch (cmd) {
      case 'n':
        pc += bytesRead;
        break;
      case 'i':
        pc += 1;
        break;
      case 'd':
        pc -= 1;
        break;
      case 'a':
        if (!getHex(&param) || param < 0) {
          printf("bad param\n");
          break;
        }
        pc = param;
        break;
      default:
        printf("error: unknown command '%c'\n", cmd);
        break;
    }
  }
}
