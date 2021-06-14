#ifndef MAPPER_H
#define MAPPER_H

#include <stdint.h>

#include "memory.h"

#define PRG_SIZE 0x4000

typedef struct {
  uint8_t rom[PRG_SIZE];
  int romLen;
} PrgRom;

PrgRom *makePrgRom(const uint8_t *prgBytes, int romLen);

uint8_t readMem(Memory *memory, const PrgRom *prgRom, uint16_t addr);
// Read's a little-endian word from memory.
uint16_t readWord(Memory *memory, const PrgRom *prgRom, uint16_t addr);

#endif  // MAPPER_H
