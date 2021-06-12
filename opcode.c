#include "opcode.h"

const Operation opcodes[256] = {
	{BRK, A_IMPL},
	{ORA, A_X_IND},
	{UND, A_UND},
	{UND, A_UND},
	{UND, A_UND},
	{ORA, A_ZPG},
	{ASL, A_ZPG},
	{UND, A_UND},
	{PHP, A_IMPL},
	{ORA, A_IMM},
	{ASL, A_ACC},
	{UND, A_UND},
	{UND, A_UND},
	{ORA, A_ABS},
	{ASL, A_ABS},
	{UND, A_UND},
	{BPL, A_REL},
	{ORA, A_IND_Y},
	{UND, A_UND},
	{UND, A_UND},
	{UND, A_UND},
	{ORA, A_ZPG_X},
	{ASL, A_ZPG_X},
	{UND, A_UND},
	{CLC, A_IMPL},
	{ORA, A_ABS_Y},
	{UND, A_UND},
	{UND, A_UND},
	{UND, A_UND},
	{ORA, A_ABS_X},
	{ASL, A_ABS_X},
	{UND, A_UND},
	{JSR, A_ABS},
	{AND, A_X_IND},
	{UND, A_UND},
	{UND, A_UND},
	{BIT, A_ZPG},
	{AND, A_ZPG},
	{ROL, A_ZPG},
	{UND, A_UND},
	{PLP, A_IMPL},
	{AND, A_IMM},
	{ROL, A_ACC},
	{UND, A_UND},
	{BIT, A_ABS},
	{AND, A_ABS},
	{ROL, A_ABS},
	{UND, A_UND},
  {BMI, A_REL},
  {AND, A_IND_Y}, 
  {UND, A_UND},
  {UND, A_UND},
  {UND, A_UND},
  {AND, A_ZPG_X},
  {ROL, A_ZPG_X},
  {UND, A_UND},
  {SEC, A_IMPL},
  {AND, A_ABS_Y},
  {UND, A_UND},
  {UND, A_UND},
  {UND, A_UND},
  {AND, A_ABS_X}, 
  {ROL, A_ABS_X}, 
  {UND, A_UND},
	{RTI, A_IMPL},
	{EOR, A_X_IND},
	{UND, A_UND},
	{UND, A_UND},
	{UND, A_UND},
	{EOR, A_ZPG},
	{LSR, A_ZPG},
	{UND, A_UND},
	{PHA, A_IMPL},
	{EOR, A_IMM},
	{LSR, A_ACC},
	{UND, A_UND},
	{JMP, A_ABS},
	{EOR, A_ABS},
	{LSR, A_ABS},
	{UND, A_UND},
	{BVC, A_REL},
	{EOR, A_IND_Y},
	{UND, A_UND},
	{UND, A_UND},
	{UND, A_UND},
	{EOR, A_ZPG_X},
	{LSR, A_ZPG_X},
	{UND, A_UND},
	{CLI, A_IMPL},
	{EOR, A_ABS_Y},
	{UND, A_UND},
	{UND, A_UND},
	{UND, A_UND},
	{EOR, A_ABS_X},
	{LSR, A_ABS_X},
	{UND, A_UND},
	{RTS, A_IMPL},
	{ADC, A_X_IND},
	{UND, A_UND},
	{UND, A_UND},
	{UND, A_UND},
	{ADC, A_ZPG},
	{ROR, A_ZPG},
	{UND, A_UND},
	{PLA, A_IMPL},
	{ADC, A_IMM},
	{ROR, A_ACC},
	{UND, A_UND},
	{JMP, A_IND},
	{ADC, A_ABS},
	{ROR, A_ABS},
	{UND, A_UND},
	{BVS, A_REL},
	{ADC, A_IND_Y},
	{UND, A_UND},
	{UND, A_UND},
	{UND, A_UND},
	{ADC, A_ZPG_X},
	{ROR, A_ZPG_X},
	{UND, A_UND},
	{SEI, A_IMPL},
	{ADC, A_ABS_Y},
	{UND, A_UND},
	{UND, A_UND},
	{UND, A_UND},
	{ADC, A_ABS_X},
	{ROR, A_ABS_X},
	{UND, A_UND},
	{UND, A_UND},
	{STA, A_X_IND},
	{UND, A_UND},
	{UND, A_UND},
	{STY, A_ZPG},
	{STA, A_ZPG},
	{STX, A_ZPG},
	{UND, A_UND},
	{DEY, A_IMPL},
	{UND, A_UND},
	{TXA, A_IMPL},
	{UND, A_UND},
	{STY, A_ABS},
	{STA, A_ABS},
	{STX, A_ABS},
	{UND, A_UND},
	{BCC, A_REL},
	{STA, A_IND_Y},
	{UND, A_UND},
	{UND, A_UND},
	{STY, A_ZPG_X},
	{STA, A_ZPG_X},
	{STX, A_ZPG_Y},
	{UND, A_UND},
	{TYA, A_IMPL},
	{STA, A_ABS_Y},
	{TXS, A_IMPL},
	{UND, A_UND},
	{UND, A_UND},
	{STA, A_ABS_X},
	{UND, A_UND},
	{UND, A_UND},
	{LDY, A_IMM},
	{LDA, A_X_IND},
	{LDX, A_IMM},
	{UND, A_UND},
	{LDY, A_ZPG},
	{LDA, A_ZPG},
	{LDX, A_ZPG},
	{UND, A_UND},
	{TAY, A_IMPL},
	{LDA, A_IMM},
	{TAX, A_IMPL},
	{UND, A_UND},
	{LDY, A_ABS},
	{LDA, A_ABS},
	{LDX, A_ABS},
	{UND, A_UND},
	{BCS, A_REL},
	{LDA, A_IND_Y},
	{UND, A_UND},
	{UND, A_UND},
	{LDY, A_ZPG_X},
	{LDA, A_ZPG_X},
	{LDX, A_ZPG_Y},
	{UND, A_UND},
	{CLV, A_IMPL},
	{LDA, A_ABS_Y},
	{TSX, A_IMPL},
	{UND, A_UND},
	{LDY, A_ABS_X},
	{LDA, A_ABS_X},
	{LDX, A_ABS_Y},
	{UND, A_UND},
	{CPY, A_IMM},
	{CMP, A_X_IND},
	{UND, A_UND},
	{UND, A_UND},
	{CPY, A_ZPG},
	{CMP, A_ZPG},
	{DEC, A_ZPG},
	{UND, A_UND},
	{INY, A_IMPL},
	{CMP, A_IMM},
	{DEX, A_IMPL},
	{UND, A_UND},
	{CPY, A_ABS},
	{CMP, A_ABS},
	{DEC, A_ABS},
	{UND, A_UND},
	{BNE, A_REL},
	{CMP, A_IND_Y},
	{UND, A_UND},
	{UND, A_UND},
	{UND, A_UND},
	{CMP, A_ZPG_X},
	{DEC, A_ZPG_X},
	{UND, A_UND},
	{CLD, A_IMPL},
	{CMP, A_ABS_Y},
	{UND, A_UND},
	{UND, A_UND},
	{UND, A_UND},
	{CMP, A_ABS_X},
	{DEC, A_ABS_X},
	{UND, A_UND},
	{CPX, A_IMM},
	{SBC, A_X_IND},
	{UND, A_UND},
	{UND, A_UND},
	{CPX, A_ZPG},
	{SBC, A_ZPG},
	{INC, A_ZPG},
	{UND, A_UND},
	{INX, A_IMPL},
	{SBC, A_IMM},
	{NOP, A_IMPL},
	{UND, A_UND},
	{CPX, A_ABS},
	{SBC, A_ABS},
	{INC, A_ABS},
	{UND, A_UND},
	{BEQ, A_REL},
	{SBC, A_IND_Y},
	{UND, A_UND},
	{UND, A_UND},
	{UND, A_UND},
	{SBC, A_ZPG_X},
	{INC, A_ZPG_X},
	{UND, A_UND},
	{SED, A_IMPL},
	{SBC, A_ABS_Y},
	{UND, A_UND},
	{UND, A_UND},
	{UND, A_UND},
	{SBC, A_ABS_X},
	{INC, A_ABS_X},
	{UND, A_UND},
};

const char *opName(int op) {
  switch (op) {
    case ADC: return "ADC";
    case AND: return "AND";
    case ASL: return "ASL";
    case BCC: return "BCC";
    case BCS: return "BCS";
    case BEQ: return "BEQ";
    case BIT: return "BIT";
    case BMI: return "BMI";
    case BNE: return "BNE";
    case BPL: return "BPL";
    case BRK: return "BRK";
    case BVC: return "BVC";
    case BVS: return "BVS";
    case CLC: return "CLC";
    case CLD: return "CLD";
    case CLI: return "CLI";
    case CLV: return "CLV";
    case CMP: return "CMP";
    case CPX: return "CPX";
    case CPY: return "CPY";
    case DEC: return "DEC";
    case DEX: return "DEX";
    case DEY: return "DEY";
    case EOR: return "EOR";
    case INC: return "INC";
    case INX: return "INX";
    case INY: return "INY";
    case JMP: return "JMP";
    case JSR: return "JSR";
    case LDA: return "LDA";
    case LDX: return "LDX";
    case LDY: return "LDY";
    case LSR: return "LSR";
    case NOP: return "NOP";
    case ORA: return "ORA";
    case PHA: return "PHA";
    case PHP: return "PHP";
    case PLA: return "PLA";
    case PLP: return "PLP";
    case ROL: return "ROL";
    case ROR: return "ROR";
    case RTI: return "RTI";
    case RTS: return "RTS";
    case SBC: return "SBC";
    case SEC: return "SEC";
    case SED: return "SED";
    case SEI: return "SEI";
    case STA: return "STA";
    case STX: return "STX";
    case STY: return "STY";
    case TAX: return "TAX";
    case TAY: return "TAY";
    case TSX: return "TSX";
    case TXA: return "TXA";
    case TXS: return "TXS";
    case TYA: return "TYA";
    default: return "UND";
  }
}

const char *addrModeName(int addrMode) {
  switch (addrMode) {
    case A_ABS: return "ABS";
    case A_ABS_X: return "ABS_X";
    case A_ABS_Y: return "ABS_Y";
    case A_ACC: return "ACC";
    case A_IMM: return "IMM";
    case A_IMPL: return "IMPL";
    case A_IND: return "IND";
    case A_IND_Y: return "IND_Y";
    case A_REL: return "REL";
    case A_X_IND: return "X_IND";
    case A_ZPG: return "ZPG";
    case A_ZPG_X: return "ZPG_X";
    case A_ZPG_Y: return "ZPG_Y";
    default: return "UND";
  }
}

int opLen(int addrMode) {
  switch (addrMode) {
    case A_ABS: return 3;
    case A_ABS_X: return 3;
    case A_ABS_Y: return 3;
    case A_ACC: return 1;
    case A_IMM: return 2;
    case A_IMPL: return 1;
    case A_IND: return 3;
    case A_IND_Y: return 2;
    case A_REL: return 2;
    case A_X_IND: return 2;
    case A_ZPG: return 2;
    case A_ZPG_X: return 2;
    case A_ZPG_Y: return 2;
    default: return -1;
  }
}
