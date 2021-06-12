#include <stdio.h>
#include <assert.h>

#include "opcode.h"

const unsigned char ROM[] = {
  0x45, 0xFF,
  0x6C, 0x12, 0x34,
  0x00,
};
const int ROM_LEN = sizeof ROM;

int printOp(const unsigned char *rom) {
  assert(rom);

  unsigned char b1 = *rom;

  int op = opcodes[b1].op;
  int addrMode = opcodes[b1].addrMode;

  int len = opLen(addrMode);
  switch (len) {
    case 1:
      printf("%02X __ __ | ", b1);
      break;
    case 2:
      printf("%02X %02X __ | ", b1, rom[1]);
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
int printOps(const char *rom, int numOps) {
  const char *start = rom;
  for (; numOps > 0; --numOps) {
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

int main(int argc, char **argv) {
  const unsigned char *cur = ROM;
  while (1) {
    printf("; @0x%04X\n", cur - ROM);
    if (feof(stdin)) {
      break;
    }
    int bytesRead = printOps(cur, 10);
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
        if (!getInt(&param) || param < 0) {
          printf("bad param\n");
          break;
        }
        cur = ROM + param;
        break;
      default:
        printf("error: unknown command '%c'\n", cmd);
        break;
    }
  }
  printf("Goodbye!\n");
  return 0;
}
