#ifndef OPCODE_H
#define OPCODE_H

enum Op {
	UND = 0, ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC,
	CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP, JSR,
	LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL, ROR, RTI, RTS, SBC,
	SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA,
};

enum AddrMode {
	A_UND = 0,
	A_ABS,
	A_ABS_X,
	A_ABS_Y,
	A_ACC,
	A_IMM,
	A_IMPL,
	A_IND,
	A_IND_Y,
	A_REL,
	A_X_IND,
	A_ZPG,
	A_ZPG_X,
	A_ZPG_Y,
};

typedef struct {
  int op;
  int addrMode;
} Operation;

extern const Operation opcodes[256];

const char *opName(int op);
const char *addrModeName(int addrMode);

// -1 on invalid.
int opLen(int addrMode);

#endif  // OPCODE_H
