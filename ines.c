#include "ines.h"

#include <malloc.h>
#include <string.h>

int readInesHeader(const char* rom, InesHeader *header) {
  if (rom == NULL || header == NULL) {
    return 0;
  }

  memcpy(header, rom, sizeof(InesHeader));

  if (header->magic[0] != 'N' ||
      header->magic[1] != 'E' ||
      header->magic[2] != 'S' ||
      header->magic[3] != 0x1A) {
    return 0;
  }

  if (header->flags7 & 0x0C == 0x0C) {
    printf("Implement iNES 2.0!\n");
    return 0;
  }

  return 1;
}

int prgAddr(InesHeader header) {
  if (header.prgBlocks < 1) {
    return -1;
  }
  if (header.flags6 & 0x04) {
    // Trainer present.
    return 16 + 512;
  } else {
    return 16;
  }
}

int prgLen(InesHeader header) {
  return header.prgBlocks * 16 * 1024;
}

int chrAddr(InesHeader header) {
  if (header.chrBlocks < 0) {
    return -1;
  }
  int prg = prgAddr(header);
  if (prg < 0) {
    return -1;
  }
  return prgAddr(header) + prgLen(header);
}

int chrLen(InesHeader header) {
  return header.chrBlocks * 8 * 1024;
}
