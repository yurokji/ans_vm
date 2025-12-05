#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <cstddef>
#include <limits>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <SFML/Graphics.hpp>


#include <chrono>
#include <thread>

#include "virtual_display.h"
#include "instruction.h"

using namespace std;


enum Flags {
    OVERFLOW_FLAG = 0x02,
    ZERO_FLAG = 0x04,
    NEGATIVE_FLAG = 0x08,
    CARRY_FLAG = 0x10,
    DIVISION_BY_ZERO_FLAG = 0x20,

    // FLOAT_OVERFLOW_FLAG = 0x20,
    // FLOAT_ZERO_FLAG = 0x40,
    // FLOAT_NEGATIVE_FLAG = 0x80,
    // FLOAT_CARRY_FLAG = 0x100,
    // FLOAT_DIVISION_BY_ZERO_FLAG = 0x200
    // // Add more flags here if needed
};


enum Interrupts {
    PRINT_CHAR = 1,  // Character print interrupt
    PRINT_INT = 2,       // Integer print interrupt
    // PRINT_FLOAT =3 ,     // Float print interrupt
    VGA_UPDATE = 4    
};

class VirtualMachine {
private:
    uint32_t int_registers[16];  // 32-bit integer general-purpose registers
    // double float_registers[16];       // 32-bit float general-purpose registers
    vector<uint32_t> memory;  // 32-bit 램 memory
    uint32_t stack_pointer;        // Stack pointer
    uint32_t program_counter;      // Program counter
    uint8_t flags;  // Add this line for flags
    size_t screen_width;  // 화면 가로 크기
    size_t screen_height; // 화면 세로 크기
    size_t pixel_depth;   // 색상 깊이
    size_t font_width;
    VirtualDisplay display;
    uint32_t begin_font_address;
    vector<Instr> program;
    bool redraw_flag;


public:
    VirtualMachine (size_t memorySize);
    void run(const vector<Instr>& program);
    void handleInterrupt(uint64_t intNumber);
    void initializeMemory(const unordered_map<uint32_t, uint32_t>& init_values);
    void setFlag(Flags flag);
    void clearFlag(Flags flag);
    bool isFlagSet(Flags flag);
    void checkFlags(int32_t result);
    // void checkFloatFlags(double value);
    uint32_t get_int_register(int idx);
    void set_int_register(uint8_t idx, uint32_t value);
    uint32_t get_program_counter() const;
    VirtualDisplay& getDisplay(void);
    

    void printMemoryAtAddress(uint32_t address, bool asAscii);
    void copyMemToVideoMem();
    int copy_font_to_memory(int begin_address);
    void load(const vector<Instr>& bytecode);
    void proceed();
    bool getRedrawFlag();
    void setRedrawFlag(bool flag);
    // vector<uint8_t>& getVideoMemory(void);
   
};


