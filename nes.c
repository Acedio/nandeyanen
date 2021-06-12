#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <assert.h>

#include "cpu.h"
#include "examine.h"
#include "ines.h"
#include "opcode.h"

int readAll(const char* filename, uint8_t **buffer) {
  if (filename == NULL || buffer == NULL) {
    return -1;
  }

  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    return -1;
  }

  fseek(f, 0, SEEK_END);
  int len = ftell(f);

  *buffer = malloc(len);
  if (*buffer == NULL) {
    fclose(f);
    return -1;
  }

  fseek(f, 0, SEEK_SET);
  int read = fread(*buffer, sizeof(uint8_t), len, f);
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

  uint8_t *rom = NULL;
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
    printf("PRG blocks: %d\nCHR blocks: %d\n", header.prgBlocks,
           header.chrBlocks);
    printf("flags6: %X\nflags7: %X\n", header.flags6, header.flags7);
  }

  uint8_t *prgRom = rom + prgAddr(header);

  CpuState cpu;
  initCpu(&cpu);

  // examineRom(prgRom, prgLen(header));

  int success = 1;
  while (success) {
    success = step(&cpu, prgRom, prgLen(header));
  }

  free(rom);
  printf("Goodbye!\n");
  return 0;
}
