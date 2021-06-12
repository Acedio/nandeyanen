#ifndef EXAMINE_H
#define EXAMINE_H

// Returns bytes read.
int printOp(const unsigned char *rom);

// Returns bytes read.
int printOps(const char *rom, const char *romEnd, int numOps);

void examineRom(const char *rom, int romLen);

#endif  // EXAMINE_H
