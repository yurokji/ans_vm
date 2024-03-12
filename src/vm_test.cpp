#include "vm.h"
#include <iostream>
#include <unordered_map>

int main() {
    // Create a VirtualMachine instance with memory size of 1024
    VirtualMachine vm(1024);

    // Define initial memory values using an unordered_map
    std::unordered_map<std::uint64_t, std::uint64_t> init_memory = {
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

    // Define the bytecode for the program to print the characters
    std::vector<std::pair<ByteCode, std::uint64_t>> bytecode = {
        {SETH, vm.encodeRegisterAndImmediate(0, 0)},
        {SETL, vm.encodeRegisterAndImmediate(0, 0)},
        {LOAD, 0},        // Load the interrupt value from memory
        {SETL, vm.encodeRegisterAndImmediate(2, 0)},
        {SETL, vm.encodeRegisterAndImmediate(3, 1)},
        
        
        {LOAD, vm.encodeTwoRegisters(0, 2)},
        {INT, PRINT_CHAR},
        {ADD, vm.encodeThreeRegisters(2, 2, 3)},

        {LOAD, vm.encodeTwoRegisters(0, 2)},
        {INT, PRINT_CHAR},
        {ADD, vm.encodeThreeRegisters(2, 2, 3)},

        {LOAD, vm.encodeTwoRegisters(0, 2)},
        {INT, PRINT_CHAR},
        {ADD, vm.encodeThreeRegisters(2, 2, 3)},

        {LOAD, vm.encodeTwoRegisters(0, 2)},
        {INT, PRINT_CHAR},
        {ADD, vm.encodeThreeRegisters(2, 2, 3)},

        {LOAD, vm.encodeTwoRegisters(0, 2)},
        {INT, PRINT_CHAR},
        {ADD, vm.encodeThreeRegisters(2, 2, 3)},

        {SETL, vm.encodeRegisterAndImmediate(4, 100)},
        {STOREI, vm.encodeRegisterAndImmediate(4, 0xa202f)},
        {LOADI, vm.encodeRegisterAndImmediate(5, 100)}
 };
        
        
        
   
    // Run the VirtualMachine with the defined bytecode
    vm.run(bytecode);
    // vm.printMemoryAtAddress(100, false);

    return 0;
}