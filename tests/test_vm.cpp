#include "vm.h"
#include <gtest/gtest.h>

class VMFixture : public ::testing::Test {
protected:
    VMFixture() : vm(256) {}
    VirtualMachine vm;
};

TEST_F(VMFixture, AddOperation) {
    vm.set_int_register(2, 10);
    vm.set_int_register(3, 32);
    std::vector<Instr> program = {
        makeRRR(Op::ADD, 1, 2, 3), // r1 = r2 + r3
    };
    vm.load(program);
    vm.proceed();
    EXPECT_EQ(vm.get_int_register(1), 42u);
}

TEST_F(VMFixture, CmpFlagsZero) {
    vm.set_int_register(1, 10);
    vm.set_int_register(2, 10);
    std::vector<Instr> program = {
        {Op::CMP, 1, 2, 0, 0},
    };
    vm.load(program);
    vm.proceed();
    EXPECT_TRUE(vm.isFlagSet(ZERO_FLAG));
    EXPECT_FALSE(vm.isFlagSet(NEGATIVE_FLAG));
}

TEST_F(VMFixture, LoadAndStore) {
    vm.initializeMemory({{50, 123}});
    vm.set_int_register(5, 50);  // base addr

    std::vector<Instr> program = {
        {Op::LOAD, 1, 5, 0, 0},          // r1 = mem[50]
        {Op::STORE, 0, 5, 1, 5},         // mem[55] = r1
        {Op::LOADI, 2, 0, 0, 55},        // r2 = mem[55]
    };
    vm.load(program);
    vm.proceed();
    vm.proceed();
    vm.proceed();
    EXPECT_EQ(vm.get_int_register(1), 123u);
    EXPECT_EQ(vm.get_int_register(2), 123u);
}

TEST_F(VMFixture, JumpImmediate) {
    std::vector<Instr> program = {
        makeJ(Op::JMP, 2),     // jump to index 2
        makeRI(Op::SET, 1, 1), // should be skipped
        makeRI(Op::SET, 2, 5), // executed
    };
    vm.load(program);
    vm.proceed(); // JMP
    EXPECT_EQ(vm.get_program_counter(), 2u);
    vm.proceed(); // SET r2
    EXPECT_EQ(vm.get_int_register(2), 5u);
}

TEST_F(VMFixture, JumpRegister) {
    vm.set_int_register(4, 2);
    std::vector<Instr> program = {
        makeR(Op::JMPR, 4),
        makeRI(Op::SET, 1, 1), // skipped
        makeRI(Op::SET, 2, 7), // executed
    };
    vm.load(program);
    vm.proceed(); // JMPR
    EXPECT_EQ(vm.get_program_counter(), 2u);
    vm.proceed();
    EXPECT_EQ(vm.get_int_register(2), 7u);
}

TEST_F(VMFixture, CallAndRetImmediate) {
    std::vector<Instr> program = {
        makeJ(Op::CALL, 2),    // call to index 2
        makeRI(Op::SET, 1, 1), // should run after return
        makeRI(Op::SET, 3, 9), // inside subroutine
        makeR(Op::RET, 0),     // return
    };
    vm.load(program);
    vm.proceed(); // CALL
    EXPECT_EQ(vm.get_program_counter(), 2u);
    vm.proceed(); // SET r3
    vm.proceed(); // RET
    EXPECT_EQ(vm.get_program_counter(), 1u);
    vm.proceed(); // SET r1
    EXPECT_EQ(vm.get_int_register(3), 9u);
    EXPECT_EQ(vm.get_int_register(1), 1u);
}

TEST_F(VMFixture, CallAndRetRegister) {
    vm.set_int_register(7, 3); // target index
    std::vector<Instr> program = {
        makeR(Op::CALLR, 7),   // call r7 -> index 3
        makeRI(Op::SET, 1, 1), // after return
        makeRI(Op::SET, 2, 2), // skipped
        makeRI(Op::SET, 4, 8), // inside subroutine
        makeR(Op::RET, 0),
    };
    vm.load(program);
    vm.proceed(); // CALLR
    EXPECT_EQ(vm.get_program_counter(), 3u);
    vm.proceed(); // SET r4
    vm.proceed(); // RET
    EXPECT_EQ(vm.get_program_counter(), 1u);
    vm.proceed(); // SET r1
    EXPECT_EQ(vm.get_int_register(4), 8u);
    EXPECT_EQ(vm.get_int_register(1), 1u);
}
