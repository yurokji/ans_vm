#include "vm.h"
#include <gtest/gtest.h>

TEST(VirtualMachineTest, TestAddOperation) {
    // Step 1: Create VM with 1024 bytes of memory
    VirtualMachine vm(1024);

    // Step 2: Initialize bytecode for ADD operation
    std::vector<Instr> bytecode;
    bytecode.push_back(makeRI(Op::SET, 0, 5));  // R0 = 5
    bytecode.push_back(makeRI(Op::SET, 1, 3));  // R1 = 3
    bytecode.push_back(makeRRR(Op::ADD, 0, 0, 1));  // R0 = R0 + R1
    bytecode.push_back({Op::HALT, 0, 0, 0, 0});

    // Step 3: Run VM
    vm.run(bytecode);

    // Step 4: Verify result (assuming register 0 contains the result)
    // Here you would typically use an accessor method to retrieve the register value.
    // Let's assume for this example that int_registers is public (not good design though).
    EXPECT_EQ(vm.get_int_register(0), 8);  // 5 + 3 should be 8
}

TEST(VirtualMachineTest, TestMulOperation) {
    VirtualMachine vm(1024);
    std::vector<Instr> bytecode;
    bytecode.push_back(makeRI(Op::SET, 2, 5));
    bytecode.push_back(makeRI(Op::SET, 3, 3));
    bytecode.push_back(makeRRR(Op::MUL, 4, 2, 3));
    bytecode.push_back({Op::HALT, 0, 0, 0, 0});
    vm.run(bytecode);
    EXPECT_EQ(vm.get_int_register(4), 15);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
