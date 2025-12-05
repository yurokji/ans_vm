#pragma once

#include <cstdint>

// Core opcode set for the VM (ISA v0.1).
enum class Op : std::uint8_t {
    NOP,
    HALT,
    INT,

    LOAD,      // rd <- mem[ra + imm]
    LOADI,     // rd <- mem[imm]
    STORE,     // mem[ra + imm] <- rb
    STOREI,    // mem[imm] <- rb
    SET,       // rd <- imm

    ADD, SUB, MUL, DIV, MOD,
    AND, OR, XOR, NOT_OP,
    SHL, SHR,
    CMP,       // flags-only: CMP ra, rb

    JMP,       // pc <- imm
    JMPR,      // pc <- r1
    JZ, JNZ, JC, JNC, JS, JNS, JO, JNO,
    CALL,      // call imm
    CALLR,     // call r1
    RET,

    PUSH,
    POP,
};

// Instruction representation: operands are explicit for readability.
struct Instr {
    Op       op;
    uint8_t  r1;   // usually dst or single operand
    uint8_t  r2;   // src1 or address base register
    uint8_t  r3;   // src2
    uint32_t imm;  // immediate value or address
};

// Small helpers to make instruction construction less noisy.
inline Instr makeRRR(Op op, uint8_t rd, uint8_t ra, uint8_t rb) {
    return Instr{op, rd, ra, rb, 0};
}

inline Instr makeRI(Op op, uint8_t rd, uint32_t imm) {
    return Instr{op, rd, 0, 0, imm};
}

inline Instr makeR(Op op, uint8_t rd) {
    return Instr{op, rd, 0, 0, 0};
}

inline Instr makeJ(Op op, uint32_t target) {
    return Instr{op, 0, 0, 0, target};
}

inline Instr makeINT(uint32_t num) {
    return Instr{Op::INT, 0, 0, 0, num};
}
