#include "examine.h"

#include <stdio.h>
#include <assert.h>

#include "opcode.h"

// TODO: Can overflow.
int printOp(const unsigned char *rom) {
  assert(rom);

  unsigned char b1 = *rom;

  int op = opcodes[b1].op;
  int addrMode = opcodes[b1].addrMode;

  int len = opLen(addrMode);
  switch (len) {
    case 1:
      printf("%02X -- -- | ", b1);
      break;
    case 2:
      printf("%02X %02X -- | ", b1, rom[1]);
      break;
    case 3:
      printf("%02X %02X %02X | ", b1, rom[1], rom[2]);
      break;
    default:
      printf("bad op: %02X\n", b1);
      return -1;
  }

  printf("%s %s\n", opName(op), addrModeName(addrMode));

  return len;
}

// Returns bytes read.
int printOps(const char *rom, const char *romEnd, int numOps) {
  const char *start = rom;
  for (; rom < romEnd && numOps > 0; --numOps) {
    int b = printOp(rom);
    if (b < 1) {
      // Skip to the next op if this one was invalid.
      rom += 1;
    } else {
      rom += b;
    }
  }
  return rom - start;
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
void examineRom(const char *rom, int romLen) {
  const char *cur = rom;
  while (1) {
    printf("; @0x%04X\n", cur - rom);
    if (feof(stdin)) {
      break;
    }
    if (cur >= rom + romLen) {
      printf("ROM ended at %x.\n", romLen);
      break;
    }
    int bytesRead = printOps(cur, rom + romLen, 10);
    char cmd = getCommand();
    if (cmd == 'q') {
      break;
    }
    int param;
    switch (cmd) {
      case 'n':
        cur += bytesRead;
        break;
      case 'i':
        cur += 1;
        break;
      case 'd':
        cur -= 1;
        break;
      case 'a':
        if (!getHex(&param) || param < 0) {
          printf("bad param\n");
          break;
        }
        cur = rom + param;
        break;
      default:
        printf("error: unknown command '%c'\n", cmd);
        break;
    }
  }
}
