#include "vm.h"
#include <iostream>
#include <unordered_map>

int main() {
    // Create a VirtualMachine instance with memory size of 1024
    VirtualMachine vm(1024);

    // Define initial memory values using an unordered_map
    std::unordered_map<std::uint32_t, std::uint32_t> init_memory = {
        {0, 104},                      // ASCII code for 'h'
        {1, 101},                      // ASCII code for 'e'
        {2, 108},                      // ASCII code for 'l'
        {3, 108},                      // ASCII code for 'l'
        {4, 111},                      // ASCII code for 'o'
        {5, 44},                       // ASCII code for ','
        {6, 32},                       // ASCII code for ' '
        {7, 119},                      // ASCII code for 'w'
        {8, 111},                      // ASCII code for 'o'
        {9, 114},                     // ASCII code for 'r'
        {10, 108},                     // ASCII code for 'l'
        {11, 100},                     // ASCII code for 'd'
        {12, 10},                      // ASCII code for newline
    };

    // Initialize memory using the init_memory map
    vm.initializeMemory(init_memory);

    // Define the program to print the characters
    std::vector<Instr> bytecode = {
        makeRI(Op::SET, 0, 0),      // addr = 0
        makeRI(Op::SET, 2, 1),      // step = 1
        makeRI(Op::SET, 5, 0),      // counter = 0
        makeRI(Op::SET, 6, 13),     // length = 13 characters

        {Op::LOAD, 0, 0, 0, 0},     // r0 = mem[addr]
        makeINT(PRINT_CHAR),        // print r0
        {Op::ADD, 0, 0, 2, 0},      // addr += step
        {Op::ADD, 5, 5, 2, 0},      // counter += step
        {Op::CMP, 5, 6, 0, 0},      // compare counter vs length (flags only)
        {Op::JNZ, 0, 0, 0, 4},      // if not done, jump back to LOAD
        {Op::HALT, 0, 0, 0, 0}
    };
        
        
        
   
    // Run the VirtualMachine with the defined bytecode
    vm.run(bytecode);
    // vm.printMemoryAtAddress(100, false);

    return 0;
}
