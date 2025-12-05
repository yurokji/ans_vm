#include "vm.h"
#include <unordered_map>
#include <vector>

int main() {
    VirtualMachine vm(1024);  // Initialize virtual machine with 1024 bytes of memory

    // Initialize 'HELLO' at memory[0] to memory[4]
    std::unordered_map<std::uint32_t, std::uint32_t> init_values = {
        {0, 'H'},
        {1, 'E'},
        {2, 'L'},
        {3, 'L'},
        {4, 'O'},
    };

    vm.initializeMemory(init_values);

    // Simple loop to print "HELLO"
    // r5: address, r6: step(1), r7: counter, r2: length(5)
    std::vector<Instr> bytecode = {
        makeRI(Op::SET, 5, 0),     // addr = 0
        makeRI(Op::SET, 6, 1),     // step = 1
        makeRI(Op::SET, 7, 0),     // counter = 0
        makeRI(Op::SET, 2, 5),     // length = 5

        {Op::LOAD, 4, 5, 0, 0},    // r4 = mem[addr]
        {Op::OR, 0, 4, 4, 0},      // r0 = r4 (move for INT)
        makeINT(PRINT_CHAR),       // print char in r0
        {Op::ADD, 5, 5, 6, 0},     // addr += step
        {Op::ADD, 7, 7, 6, 0},     // counter += step
        {Op::CMP, 7, 2, 0, 0},     // counter - length -> flags
        {Op::JNZ, 0, 0, 0, 4},     // loop until counter == length
        {Op::HALT, 0, 0, 0, 0}
    };

    vm.run(bytecode);

    return 0;
}
