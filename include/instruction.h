#include <iostream>
#include <cstdint>
#include <bitset>

// Define the Instruction class
class Instruction {
public:
    // Constructor that takes a 64-bit variable and encodes it
    Instruction(uint64_t variable) {
        encode(variable);
    }

    // Method to display the instruction
    void display() const {
        std::cout << "Opcode: " << std::hex << static_cast<int>(opcode) << std::endl;
        std::cout << "RegA:   " << std::hex << static_cast<int>(regA) << std::endl;
        std::cout << "RegB:   " << std::hex << static_cast<int>(regB) << std::endl;
        std::cout << "RegC:   " << std::hex << static_cast<int>(regC) << std::endl;
        std::cout << "Value:  " << std::hex << static_cast<int>(value) << std::endl;
    }

    // Method to decode the instruction into a 64-bit variable
    uint64_t decode() const {
        uint64_t result = 0;
        result |= static_cast<uint64_t>(opcode) << 56;
        result |= static_cast<uint64_t>(regA) << 48;
        result |= static_cast<uint64_t>(regB) << 40;
        result |= static_cast<uint64_t>(regC) << 32;
        result |= static_cast<uint64_t>(value) & 0xFFFFFFFF;
        return result;
    }

private:
    // Private method to encode a 64-bit variable into the instruction
    void encode(uint64_t variable) {
        opcode = static_cast<uint8_t>((variable >> 56) & 0xFF);  // Extracting the upper 8 bits
        regA = static_cast<uint8_t>((variable >> 48) & 0x0F);    // Extracting the next 8 bits and drop upper 4bits
        regB = static_cast<uint8_t>((variable >> 40) & 0x0F);    // Extracting the next 8 bits and drop upper 4bits
        regC = static_cast<uint8_t>((variable >> 32) & 0x0F);    // Extracting the next 8 bits and drop upper 4bits
        value = static_cast<uint32_t>(variable & 0xFFFFFFFF);    // Extracting the lower 32 bits
    }

    // Member variables
    uint8_t opcode;
    uint8_t regA;
    uint8_t regB;
    uint8_t regC;
    uint32_t value;
};

