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
      return 1;
  }

  printf("%s %s\n", opName(op), addrModeName(addrMode));

  return len;
}

// Returns bytes read.
int printOps(const char *rom, int numOps) {
  const char *start = rom;
  for (; numOps > 0; --numOps) {
    int b = printOp(rom);
    rom += b;
  }
  return rom - start;
}

int main(int argc, char **argv) {
  printf("Hello, world!\n");
  printOps(ROM + 2, 3);
  return 0;
}
