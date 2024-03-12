#include "vm.h"  // Your virtual machine header file
#include <vector>
#include <utility>

void setImmediate64(std::vector<std::pair<ByteCode, std::uint64_t>>& bytecode, std::uint32_t registerIndex, std::uint64_t value) {
    std::uint64_t upper32 = value >> 32;
    std::uint64_t lower32 = value & 0xFFFFFFFF;

    // Set the upper 32 bits
    bytecode.push_back({SETH, (static_cast<std::uint64_t>(registerIndex) << 32) | upper32});
    
    // Set the lower 32 bits
    bytecode.push_back({SETL, (static_cast<std::uint64_t>(registerIndex) << 32) | lower32});
}


int main() {
    VirtualMachine vm(1024);  // Initialize virtual machine with 1024 bytes of memory

    // Initialize 'HELLO' at memory[0] to memory[4]
    std::unordered_map<std::uint64_t, std::uint64_t> init_values = {
        {0, 'H'},
        {1, 'E'},
        {2, 'L'},
        {3, 'L'},
        {4, 'O'},
    };
    std::uint64_t operand;

    vm.initializeMemory(init_values);

    // Generate bytecode for the animation
    std::vector<std::pair<ByteCode, std::uint64_t>> bytecode;


    setImmediate64(bytecode, 3, 0x1234567890ABCDEF);

    // // Store initial position (10) to register 0
    // bytecode.push_back({LOAD, (0 << 32) | 10});

    // // Label for loop start
    // std::size_t loop_start = bytecode.size();

    // // Set register 1 = 0 (counter for "HELLO" letters)
    // bytecode.push_back({LOAD, (1 << 32) | 0});

    // // Label for inner loop start
    // std::size_t inner_loop_start = bytecode.size();

    // // Load from memory[register 1] to register 2
    // std::uint64_t operand = encodeThreeRegisters(1, 2, 0); // Assuming 0 as dummy third register
    // bytecode.push_back({LDR, operand});

    // // Add register 0 and register 1, and store the result in register 3
    // bytecode.push_back({ADD, encodeThreeRegisters(0, 1, 3)});

    // // Store to memory[register 3] from register 2
    // bytecode.push_back({STR, encodeThreeRegisters(3, 2, 0)}); // Assuming 0 as dummy third register

    // // Increment register 1 by 1
    // bytecode.push_back({ADD, encodeThreeRegisters(1, 1, 1)});

    // // Compare register 1 with 5
    // bytecode.push_back({CMP, encodeThreeRegisters(1, 5, 0)}); // Assuming 0 as dummy third register

    // // Jump to inner loop start if register 1 < 5
    // bytecode.push_back({JZ, inner_loop_start});

    // // Output current screen using interrupt
    // bytecode.push_back({INT, PRINT_CHAR << 32 | 0}); // Assuming 0 as dummy third register

    // // Increment register 0 by 1 (moving position)
    // bytecode.push_back({ADD, encodeThreeRegisters(0, 0, 0)}); // Assuming 0 as dummy third register

    // // Jump back to start of loop
    // bytecode.push_back({JMP, loop_start});

    // Halt
    bytecode.push_back({HALT, 0});

    vm.run(bytecode);

    return 0;
}