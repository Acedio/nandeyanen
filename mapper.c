#include "mapper.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

PrgRom *makePrgRom(const uint8_t *prgBytes, int romLen) {
  if (romLen != PRG_SIZE) {
    printf("incorrect romLen: %d\n", romLen);
    return NULL;
  }

  PrgRom *prgRom = malloc(sizeof(PrgRom));
  memcpy(prgRom->rom, prgBytes, romLen);
  prgRom->romLen = PRG_SIZE;
  return prgRom;
}

uint8_t readByte(Memory *memory, const PrgRom *prgRom, uint16_t addr) {
  assert(addr >= 0x8000);
  return prgRom->rom[(addr - 0x8000) % PRG_SIZE];
}

uint16_t readWord(Memory *memory, const PrgRom *prgRom, uint16_t addr) {
  uint16_t word = readByte(memory, prgRom, addr+1);
  word <<= 8;
  word += readByte(memory, prgRom, addr);
  return word;
}
