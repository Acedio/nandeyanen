#include "opcode.h"

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

uint16_t opLen(int addrMode) {
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
