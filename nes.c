#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "ines.h"
#include "opcode.h"

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

int readAll(const char* filename, char **buffer) {
  if (filename == NULL || buffer == NULL) {
    return -1;
  }

  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    return -1;
  }

  fseek(f, 0, SEEK_END);
  int len = ftell(f);

  *buffer = calloc(len, sizeof(char));
  if (*buffer == NULL) {
    fclose(f);
    return -1;
  }

  fseek(f, 0, SEEK_SET);
  int read = fread(*buffer, sizeof(char), len, f);
  if (read != len) {
    printf("read != length: %d != %d\n", read, len);
    fclose(f);
    return -1;
  }

  fclose(f);

  return len;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage\n");
    printf("%s [path]\n", argv[0]);
    return -1;
  }

  char *rom = NULL;
  int romLen = readAll(argv[1], &rom);
  if (romLen < 0) {
    printf("Error reading ROM at %s\n", argv[1]);
    free(rom);
    return -1;
  } else {
    printf("Successfully opened ROM at %s (%d bytes).\n", argv[1], romLen);
  }

  InesHeader header;
  if (!readInesHeader(rom, &header)) {
    printf("Error reading iNES header.\n");
    free(rom);
    return -1;
  } else {
    printf("Successfully read iNES header.\n");
    printf("PRG blocks: %d\nCHR blocks: %d\n", header.prgLen, header.chrLen);
    printf("flags6: %X\nflags7: %X\n", header.flags6, header.flags7);
  }

  char *prgRom = rom + prgAddr(header);

  const char *cur = prgRom;
  while (1) {
    printf("; @0x%04X\n", cur - prgRom);
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
        cur = prgRom + param;
        break;
      default:
        printf("error: unknown command '%c'\n", cmd);
        break;
    }
  }
  free(rom);
  printf("Goodbye!\n");
  return 0;
}
