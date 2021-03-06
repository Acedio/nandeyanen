#ifndef INES_H
#define INES_H

#include <stdint.h>

typedef struct __attribute__((__packed__)) {
  char magic[4];
  unsigned char prgBlocks;
  unsigned char chrBlocks;
  unsigned char flags6;
  unsigned char flags7;
  char unused[8];
} InesHeader;

// Returns false if header is invalid.
int readInesHeader(const uint8_t* rom, InesHeader *header);

int prgAddr(InesHeader header);
int prgLen(InesHeader header);
int chrAddr(InesHeader header);
int chrLen(InesHeader header);

#endif  // INES_H
