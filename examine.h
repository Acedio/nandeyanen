#ifndef EXAMINE_H
#define EXAMINE_H

#include <stdint.h>

// Returns bytes read.
int printOp(const uint8_t *rom);

// Returns bytes read.
int printOps(const uint8_t *rom, const uint8_t *romEnd, int numOps);

void examineRom(const uint8_t *rom, int romLen);

#endif  // EXAMINE_H
