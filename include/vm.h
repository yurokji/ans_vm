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


enum ByteCode {
    NOP,    // No operation
    INT,    // Software interrupt
    HALT,   // Halt the machine
    
    LOAD,   // Load into register
    STORE,  // Store into memory
    LOADI,  //  로드 LOADI 4453, 2 -> 2번 특정 메모리 주소 (4453번지)를 직접 써서 그 값을 레지스터 4번에 로드 
    STOREI,  // 저장 LOADI 4, 4453 ->  레지스터 4번에 들어있는 값을 특정 메모리 주소 (4453번지)에 직접 저장
    
    SET,   // immediate값 하위 32비트를 레지스터에 설정, SET R1, 5   // R1 <- 5
    
    CLEAR,  // 레지스터 내용 0으로 채우기
    
    ADD,    // Integer Add operation
    SUB,    // Integer Subtract operation
    MUL,    // Integer Multiply operation
    DIV,    // Integer Divide operation
    MOD,    // Integer Modulo operation

    CMP,    // Compare
    AND,    // Bitwise AND operation
    OR,     // Bitwise OR operation
    XOR,    // Bitwise XOR operation
    NOT,    // Bitwise NOT operation
    SHL,    // Bitwise shift left
    SHR,    // Bitwise shift right
    

    JMP,   // Jump to address
    JZ,     // Jump if zero
    JNZ,    // Jump if not zero
    JC,     // Jump if Carry
    JNC,    // Jump if Not Carry
    JS,     // Jump if Negative
    JNS,    // Jump if Not Negative
    JO,     // Jump if Overflow
    JNO,    // Jump if No Overflow


    PUSH,   // Push to stack
    POP,    // Pop from stack
    
    CALL,   // Call subroutine
    RET,    // Return from subroutine


    // FADD,   // Float Add operation
    // FSUB,   // Float Subtract operation
    // FMUL,   // Float Multiply operation
    // FDIV,   // Float Divide operation
    // FCMP,   // Float Compare


    // FJZ,     // Jump if zero for floating point
    // FJNZ,    // Jump if not zero for floating point
    // FJC,     // Jump if Carry for floating point
    // FJNC,    // Jump if Not Carry for floating point
    // FJS,     // Jump if Negative for floating point
    // FJNS,    // Jump if Not Negative for floating point
    // FJO,     // Jump if Overflow for floating point
    // FJNO,    // Jump if No Overflow for floating point


};

enum Interrupts {
    PRINT_CHAR = 1,  // Character print interrupt
    PRINT_INT = 2,       // Integer print interrupt
    PRINT_FLOAT =3 ,     // Float print interrupt
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
    vector<pair<ByteCode, uint32_t>> program;
    bool redraw_flag;


public:
    VirtualMachine (size_t memorySize);
    void run(const vector<pair<ByteCode, uint32_t>>& bytecode);
    void handleInterrupt(uint32_t intNumber);
    void initializeMemory(const unordered_map<uint32_t, uint32_t>& init_values);
    void setFlag(Flags flag);
    void clearFlag(Flags flag);
    bool isFlagSet(Flags flag);
    void checkFlags(int32_t result);
    void checkFloatFlags(double value);
    uint32_t get_int_register(int idx);
    VirtualDisplay& getDisplay(void);
    

    void printMemoryAtAddress(uint32_t address, bool asAscii);
    void copyMemToVideoMem();
    int copy_font_to_memory(int begin_address);
    void load(const vector<pair<ByteCode, uint32_t>>& bytecode);
    void proceed();
    bool getRedrawFlag();
    void setRedrawFlag(bool flag);
    // vector<uint8_t>& getVideoMemory(void);
   
};




