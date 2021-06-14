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

uint8_t readByte(const Memory *memory, const PrgRom *prgRom, uint16_t addr) {
  assert(addr < 0x0800 || addr >= 0x8000);
  if (addr < 0x0800) {
    return memory->memory[addr];
  } else if (addr >= 0x8000) {
    return prgRom->rom[(addr - 0x8000) % PRG_SIZE];
  }
  return 0x55;
}

uint16_t readWord(const Memory *memory, const PrgRom *prgRom, uint16_t addr) {
  uint16_t word = readByte(memory, prgRom, addr+1);
  word <<= 8;
  word += readByte(memory, prgRom, addr);
  return word;
}

void writeByte(Memory *memory, const PrgRom *prgRom, uint16_t addr,
               uint8_t byte) {
  assert(addr < 0x0800);
  memory->memory[addr] = byte;
}
