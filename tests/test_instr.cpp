#include "instruction.h"
#include <gtest/gtest.h>

TEST(InstrTest, MakeRRR) {
    Instr ins = makeRRR(Op::ADD, 1, 2, 3);
    EXPECT_EQ(ins.op, Op::ADD);
    EXPECT_EQ(ins.r1, 1);
    EXPECT_EQ(ins.r2, 2);
    EXPECT_EQ(ins.r3, 3);
    EXPECT_EQ(ins.imm, 0u);
}

TEST(InstrTest, MakeRI) {
    Instr ins = makeRI(Op::SET, 4, 123);
    EXPECT_EQ(ins.op, Op::SET);
    EXPECT_EQ(ins.r1, 4);
    EXPECT_EQ(ins.r2, 0);
    EXPECT_EQ(ins.r3, 0);
    EXPECT_EQ(ins.imm, 123u);
}

TEST(InstrTest, MakeRAndJ) {
    Instr j = makeJ(Op::JMP, 42);
    EXPECT_EQ(j.op, Op::JMP);
    EXPECT_EQ(j.imm, 42u);

    Instr r = makeR(Op::CALLR, 5);
    EXPECT_EQ(r.op, Op::CALLR);
    EXPECT_EQ(r.r1, 5);
}
