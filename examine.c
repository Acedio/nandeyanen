#include "examine.h"

#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include "opcode.h"
#include "memory.h"
#include "mapper.h"

uint16_t printOp(const PrgRom* rom, uint16_t pc) {
  assert(rom);

  uint8_t b1 = readByte(NULL, rom, pc);

  int op = opcodes[b1].op;
  int addrMode = opcodes[b1].addrMode;

  uint16_t len = opLen(addrMode);
  switch (len) {
    case 1:
      printf("%02X        ", b1);
      break;
    case 2:
      printf("%02X %02X     ", b1, readByte(NULL, rom, pc+1));
      break;
    case 3:
      printf("%02X %02X %02X  ", b1, readByte(NULL, rom, pc+1),
             readByte(NULL, rom, pc+2));
      break;
    default:
      printf("bad op: %02X\n", b1);
      return -1;
  }

  // TODO: Print args

  printf("%s %s", opName(op), addrModeName(addrMode));

  return len;
}

// Returns bytes read.
uint16_t printOps(const PrgRom* rom, uint16_t pc, int numOps) {
  uint16_t start = pc;
  for (; numOps > 0; --numOps) {
    int b = printOp(rom, pc);
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
  printf("A:%02X X:%02X Y:%02X P:%02X SP:%02X", cpu->a, cpu->x, cpu->y,
         cpu->status, cpu->s);
}

uint16_t printState(const CpuState *cpu, const PrgRom *rom, uint16_t pc) {
  printf("%04X  ", cpu->pc);
  printOp(rom, pc);
  printf("    ");
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

void examineRom(const PrgRom *rom, uint16_t pc) {
  while (1) {
    printf("%04X  ", pc);
    printOp(rom, pc);
    printf("\n");
    if (feof(stdin)) {
      break;
    }
    uint16_t bytesRead = printOps(rom, pc, 10);
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
