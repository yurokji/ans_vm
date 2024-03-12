#include "vm.h"
#include <iostream>

VirtualMachine::VirtualMachine(size_t memorySize) {
    memory.resize(memorySize, 0);
    stack_pointer = memorySize - 1;
    program_counter = 0;
    // Initialize integer and float registers to zero
    std::fill(int_registers, int_registers + 16, 0);
    std::fill(float_registers, float_registers + 16, 0.0);
}

void VirtualMachine::run(const std::vector<std::pair<ByteCode, std::uint64_t>>& bytecode) {
    std::uint64_t reg1, reg2, reg3;
    while (program_counter < bytecode.size()) {
        auto [opcode, operand] = bytecode[program_counter++];
        switch (opcode) {
            case NOP:
                // No operation, do nothing
                break;
            case LOAD:
                int_registers[operand] = memory[int_registers[++program_counter]];
                break;
            case STORE:
                memory[int_registers[++program_counter]] = int_registers[operand];
                break;
            case SETH:
                // Only sets the upper 32 bits of a 64-bit register to an immediate value
                int_registers[operand >> 32] &= 0xFFFFFFFF; // Clear upper 32 bits
                int_registers[operand >> 32] |= (operand & 0xFFFFFFFF) << 32; // Set upper 32 bits
                break;
            case SETL:
                // Only sets the lower 32 bits of a 64-bit register to an immediate value
                int_registers[operand >> 32] &= 0xFFFFFFFF00000000; // Clear lower 32 bits
                int_registers[operand >> 32] |= operand & 0xFFFFFFFF; // Set lower 32 bits
                break;
            case CLEAR:
                // Clear the contents of the given register
                int_registers[operand] = 0;
                break;

            case ADD:
                auto [reg1, reg2, reg3] = decodeThreeRegisters(operand);
                int_registers[reg1] = int_registers[reg2] + int_registers[reg3];
                break;


                



            // case AND:
            //     int_registers[operand] &= int_registers[++program_counter];
            //     break;
            // case OR:
            //     int_registers[operand] |= int_registers[++program_counter];
            //     break;
            // case XOR:
            //     int_registers[operand] ^= int_registers[++program_counter];
            //     break;
            // case NOT:
            //     int_registers[operand] = ~int_registers[operand];



            //     break;
            // case SHL:
            //     int_registers[operand] <<= int_registers[++program_counter];
            //     break;
            // case SHR:
            //     int_registers[operand] >>= int_registers[++program_counter];
            //     break;



            // case JMP:
            //     program_counter = operand;
            //     break;
            // case JZ:
            //     if (int_registers[0] == 0) {
            //         program_counter = operand;
            //     }
            //     break;
            // case JNZ:
            //     if (int_registers[0] != 0) {
            //         program_counter = operand;
            //     }
            //     break;





            // case PUSH:
            //     memory[stack_pointer--] = int_registers[operand];
            //     break;
            // case POP:
            //     int_registers[operand] = memory[++stack_pointer];
            //     break;
            // case CALL:
            //     memory[stack_pointer--] = program_counter;
            //     program_counter = operand;
            //     break;
            // case RET:
            //     program_counter = memory[++stack_pointer];
            //     break;
            case CMP:
                if (int_registers[operand] == int_registers[++program_counter]) {
                    flags = 0;
                } else if (int_registers[operand] < int_registers[program_counter]) {
                    flags = 1;
                } else {
                    flags = 2;
                }
                break;
            // case FCMP: {
            //     double a = float_registers[operand >> 8];
            //     double b = float_registers[operand & 0xFF];
            //     flags = (a == b) ? EQ : (a < b) ? LT : GT;
            //     break;
            // }






            // case FADD:
            //     float_registers[operand] += float_registers[++program_counter];
            //     break;
            // case FSUB:
            //     float_registers[operand] -= float_registers[++program_counter];
            //     break;
            // case FMUL:
            //     float_registers[operand] *= float_registers[++program_counter];
            //     break;
            // case FDIV:
            //     if (float_registers[program_counter] != 0) {
            //         float_registers[operand] /= float_registers[++program_counter];
            //     }
            //     break;

            
            
            
            // case FLT:
            //     flags = (float_registers[R1] < float_registers[R2]) ? 1 : 0;
            //     break;
            // case FGT:
            //     flags = (float_registers[R1] > float_registers[R2]) ? 1 : 0;
            //     break;
            // case FEQ:
            //     flags = (float_registers[R1] == float_registers[R2]) ? 1 : 0;
            //     break;
            // case FLE:
            //     flags = (float_registers[R1] <= float_registers[R2]) ? 1 : 0;
            //     break;
            // case FGE:
            //     flags = (float_registers[R1] >= float_registers[R2]) ? 1 : 0;
            //     break;
            // case FNE:
            //     flags = (float_registers[R1] != float_registers[R2]) ? 1 : 0;
            //     break;

            default:
                std::cerr << "Unknown opcode: " << opcode << std::endl;
                return;
        }
    }
}

void VirtualMachine::handleInterrupt(std::uint64_t intNumber) {
    switch (intNumber) {
        case PRINT_CHAR:
            std::cout << static_cast<char>(int_registers[0]);
            break;
        case PRINT_INT:
            std::cout << int_registers[0];
            break;
        case PRINT_FLOAT:
            std::cout << float_registers[0];
            break;
        default:
            std::cerr << "Unknown interrupt: " << intNumber << std::endl;
    }
}

void VirtualMachine::initializeMemory(const std::unordered_map<std::uint64_t, std::uint64_t>& init_values) {
    for (const auto& [address, value] : init_values) {
        if (address < memory.size()) {
            memory[address] = value;
        } else {
            std::cerr << "Memory address out of range: " << address << std::endl;
        }
    }
}

void VirtualMachine::setFlag(Flags flag) {
    flags |= flag;
}
void VirtualMachine::setFlag(Flags flag) {
    flags &= ~flag;
}
bool  VirtualMachine::setFlag(Flags flag) {
    return (flags & flag) != 0;
}