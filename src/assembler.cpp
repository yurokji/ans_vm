#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include "../include/instruction.h"

enum ByteCode {
    NOP,    // No operation
    SETH,   // Set upper 32 bits of register to immediate value
    JMP,    // Jump to address
};

std::unordered_map<std::string, ByteCode> opcode_map = {
    {"NOP", NOP},
    {"SETH", SETH},
    {"JMP", JMP}
};


아래는 Instruction 클래스를 instruction.h 파일에 저장하고, assembler.cpp에서 명령어를 기계어 코드로 어셈블하는 예제 코드입니다.

instruction.h:

cpp
Copy code
// instruction.h
#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>

class Instruction {
public:
    Instruction(uint64_t variable);

    void display() const;

    uint64_t decode() const;

private:
    void encode(uint64_t variable);

    uint8_t opcode;
    uint8_t reg1;
    uint8_t reg2;
    uint8_t reg3;
    uint32_t value;
};

#endif // INSTRUCTION_H
instruction.cpp:

cpp
Copy code
// instruction.cpp
#include "instruction.h"
#include <iostream>
#include <bitset>

Instruction::Instruction(uint64_t variable) {
    encode(variable);
}

void Instruction::display() const {
    std::cout << "Opcode: " << std::bitset<8>(opcode) << std::endl;
    std::cout << "Reg1:   " << std::bitset<8>(reg1) << std::endl;
    std::cout << "Reg2:   " << std::bitset<8>(reg2) << std::endl;
    std::cout << "Reg3:   " << std::bitset<8>(reg3) << std::endl;
    std::cout << "Value:  " << std::bitset<32>(value) << std::endl;
}

uint64_t Instruction::decode() const {
    uint64_t result = 0;
    result |= static_cast<uint64_t>(opcode) << 56;
    result |= static_cast<uint64_t>(reg1) << 48;
    result |= static_cast<uint64_t>(reg2) << 40;
    result |= static_cast<uint64_t>(reg3) << 32;
    result |= static_cast<uint64_t>(value) & 0xFFFFFFFF;
    return result;
}

void Instruction::encode(uint64_t variable) {
    opcode = static_cast<uint8_t>((variable >> 56) & 0xFF);  // Extracting the upper 8 bits
    reg1 = static_cast<uint8_t>((variable >> 48) & 0x0F);    // Extracting the next 4 bits
    reg2 = static_cast<uint8_t>((variable >> 40) & 0xFF);    // Extracting the next 8 bits
    reg3 = static_cast<uint8_t>((variable >> 32) & 0xFF);    // Extracting the next 8 bits
    value = static_cast<uint32_t>(variable & 0xFFFFFFFF);    // Extracting the lower 32 bits
}
assembler.cpp:

cpp
Copy code
// assembler.cpp
#include "instruction.h"
#include <vector>
#include <string>
#include <sstream>
#include <map>

// Define opcodes
const uint64_t OPCODE_NOP = 0x01;
const uint64_t OPCODE_SET = 0x02;
const uint64_t OPCODE_JMP = 0x03;
const uint64_t OPCODE_LOAD = 0x04;

// Map opcodes to their corresponding values
std::map<std::string, uint64_t> opcodeMap = {
    {"NOP", OPCODE_NOP},
    {"SET", OPCODE_SET},
    {"JMP", OPCODE_JMP},
    {"LOAD", OPCODE_LOAD}
};

// Function to assemble assembly code into machine code
std::vector<uint64_t> assemble(const std::vector<std::string>& assemblyCode) {
    std::vector<uint64_t> machineCode;
    
    int address = 0;
    int org_address = 0;

    // First pass: Identify labels and addresses
    for (const std::string& line : assembly) {
        std::istringstream iss(line);
        std::string opcode;
        iss >> opcode;
        
        if (opcode == "ORG") {
            iss >> std::hex >> org_address;
            address = org_address;
            continue;
        }

        if (opcode.back() == ':') {
            label_table[opcode.substr(0, opcode.size() - 1)] = address;
        } else if (opcode_map.find(opcode) != opcode_map.end()) {
            ByteCode code = opcode_map[opcode];
            if (code == JMP) {
                iss >> operand;
            } else {
                std::getline(iss >> std::ws, operand);
            }
            instructions.push_back({code, operand});
            address += 8;
        }
    }

    // Second pass: Generate bytecode and initialize memory
    address = org_address;
    for (const std::string& line : assembly) {
        std::istringstream iss(line);
        std::string opcode;
        iss >> opcode;
        
        if (opcode == "ORG") {
            continue;
        }

        if (opcode_map.find(opcode) != opcode_map.end()) {
            ByteCode code = opcode_map[opcode];
            std::uint64_t operand = 0;

            // ... (피연산자 파싱 코드)
            // Example for LOADI
            if (code == LOADI) {
                std::uint64_t reg, value;
                iss >> reg >> std::hex >> value;
                operand = encodeRegisterAndImmediate(reg, value);
            }

            // ... (기타 명령어에 대한 코드)

            // For jump instructions, look up the label
            if (code == JMP) {
                std::string label;
                iss >> label;
                operand = label_table[label];
            }

            instructions.push_back({code, operand});
            init_values[address] = encodeInstruction(code, operand);  // Assuming you have a function to encode the instruction and operand into a single uint64_t
            address += 8;
        }
    }

    return instructions;
}


int main() {
    std::unordered_map<std::string, int> label_table;
    std::unordered_map<std::uint64_t, std::uint64_t> init_values;
    std::vector<std::string> assembly_code = {
        "ORG 0x1000",
        "NOP",
        "SETH R6, R1",
        "JMP LABEL1",
        "SETH R1, 10",
        "LABEL1:",
        "SETH R2, 20"
    };

    std::vector<uint64_t> machineCode = assemble(assembly_code, label_table, init_values);


    return 0;
}


