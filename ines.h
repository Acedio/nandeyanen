#ifndef INES_H
#define INES_H

typedef struct __attribute__((__packed__)) {
  char magic[4];
  unsigned char prgLen;
  unsigned char chrLen;
  unsigned char flags6;
  unsigned char flags7;
  char unused[8];
} InesHeader;

// Returns false if header is invalid.
int readInesHeader(const char* rom, InesHeader *header);

int prgAddr(InesHeader header);
int chrAddr(InesHeader header);

#endif  // INES_H
