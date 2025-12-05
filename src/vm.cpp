#include "vm.h"
#include <iostream>

VirtualMachine::VirtualMachine(size_t memorySize) : display(DISPLAY_WIDTH, DISPLAY_HEIGHT) {
    memory.resize(memorySize, 0);
    stack_pointer = memorySize - 1;
    program_counter = 0;
    redraw_flag = true;

    // Initialize integer and float registers to zero
    std::fill(int_registers, int_registers + 16, 0);
    // std::fill(float_registers, float_registers + 16, 0.0);
    font_width = 32;
    pixel_depth = 8;
    screen_width = font_width*16;
    screen_height = font_width*16;
    
    
    const int VIDEO_RAM_SIZE = screen_width * screen_height * (pixel_depth / 8); // 비디오 메모리 크기
    // 비디오 메모리
    //텍스트 폰트 RAM에 로딩
    begin_font_address = 0x100f;
    copy_font_to_memory(begin_font_address);
    

}

// 비디오 업데이트 함수
void VirtualMachine::copyMemToVideoMem() {
    std::uint32_t ascii_val = int_registers[1];
    std::uint32_t x_relative_pos = int_registers[2];
    std::uint32_t y_relative_pos = int_registers[3];
    std::uint32_t begin_idx = begin_font_address + ascii_val * (font_width * font_width); 
        std::uint32_t grayscaleValue;
        // cout <<int_registers[1]<<" "<<int_registers[2]<<" "<<int_registers[3]<<endl;
        size_t idx;
        for (int y = 0; y < font_width; y++) {
            for (int x = 0; x < font_width; x++) {
                idx = begin_idx + (y * font_width) + x;
                
                std::uint32_t grayscaleValue = memory.at(idx);
                sf::Color pixelColor(grayscaleValue,grayscaleValue,grayscaleValue);
                pixelColor.r = 0;
                pixelColor.g = grayscaleValue;
                pixelColor.b = 0;

                display.setPixel(x_relative_pos*font_width + x, y_relative_pos*font_width + y, pixelColor);
            }
        }
}
    

int VirtualMachine::copy_font_to_memory(int begin_addr){
    sf::Image image;
    if (!image.loadFromFile("../images/asciifont_32x32.png")) {
        return 1;
    }

    // 이미지 크기를 가져옵니다.
    sf::Vector2u imageSize = image.getSize();
    std::size_t imageWidth = imageSize.x;
    std::size_t imageHeight = imageSize.y;

    // 그레이스케일 이미지를 메모리에 복사합니다.
    std::uint32_t idx  = 0;
    size_t szBlockWidth = 16;
    for (std::size_t i = 0; i < szBlockWidth; i++) {
        for (std::size_t j = 0; j < szBlockWidth; j++) {
            for (std::size_t y = 0; y < font_width; y++) {
                for (std::size_t x = 0; x < font_width; x++) {
                    size_t x_pos = x + j * font_width;
                    size_t y_pos = y + i * font_width;
                    sf::Color pixelColor = image.getPixel(x_pos, y_pos);
                    std::uint8_t grayscaleValue = pixelColor.r;
                    std::uint32_t pixelValue = grayscaleValue;
                    memory.at(begin_addr + idx) = pixelValue;
                    // cout <<begin_addr + idx <<": "<<pixelValue<<endl;
                    idx++;
                }
            }
        }
    }


    return 0;
}

void VirtualMachine::load(const std::vector<Instr>& bytecode) {
    program = bytecode;
    program_counter = 0;
}

void VirtualMachine::run(const std::vector<Instr>& bytecode) {
    load(bytecode);
    while (program_counter < program.size()) {
        proceed();
    }
}

void VirtualMachine::proceed() {
    if (program_counter >= program.size()) {
        return;
    }

    const Instr& ins = program[program_counter++];

    switch (ins.op) {
        case Op::NOP:
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            break;

        case Op::INT:
            handleInterrupt(ins.imm);
            break;

        case Op::HALT:
            program_counter = program.size();
            return;

        case Op::LOAD: {
            uint32_t addr = int_registers[ins.r2] + ins.imm;
            int_registers[ins.r1] = memory.at(addr);
            break;
        }

        case Op::LOADI:
            int_registers[ins.r1] = memory.at(ins.imm);
            break;

        case Op::STORE: {
            uint32_t addr = int_registers[ins.r2] + ins.imm;
            memory.at(addr) = int_registers[ins.r3];
            break;
        }

        case Op::STOREI:
            memory.at(ins.imm) = int_registers[ins.r3];
            break;

        case Op::SET:
            int_registers[ins.r1] = ins.imm;
            break;

        case Op::ADD: {
            int32_t result = static_cast<int32_t>(int_registers[ins.r2]) + static_cast<int32_t>(int_registers[ins.r3]);
            checkFlags(result);
            int_registers[ins.r1] = static_cast<std::uint32_t>(result);
            break;
        }

        case Op::SUB: {
            int32_t result = static_cast<int32_t>(int_registers[ins.r2]) - static_cast<int32_t>(int_registers[ins.r3]);
            checkFlags(result);
            int_registers[ins.r1] = static_cast<std::uint32_t>(result);
            break;
        }

        case Op::MUL: {
            int64_t wide = static_cast<int64_t>(static_cast<int32_t>(int_registers[ins.r2])) *
                           static_cast<int64_t>(static_cast<int32_t>(int_registers[ins.r3]));
            if (wide > std::numeric_limits<std::int32_t>::max() || wide < std::numeric_limits<std::int32_t>::min()) {
                flags |= OVERFLOW_FLAG;
            } else {
                int_registers[ins.r1] = static_cast<std::uint32_t>(wide);
                checkFlags(static_cast<int32_t>(wide));
            }
            break;
        }

        case Op::DIV: {
            int32_t divisor = static_cast<int32_t>(int_registers[ins.r3]);
            if (divisor != 0) {
                int32_t result = static_cast<int32_t>(int_registers[ins.r2]) / divisor;
                int_registers[ins.r1] = static_cast<std::uint32_t>(result);
                checkFlags(result);
            } else {
                flags |= DIVISION_BY_ZERO_FLAG;
            }
            break;
        }

        case Op::MOD: {
            int32_t divisor = static_cast<int32_t>(int_registers[ins.r3]);
            if (divisor != 0) {
                int32_t result = static_cast<int32_t>(int_registers[ins.r2]) % divisor;
                int_registers[ins.r1] = static_cast<std::uint32_t>(result);
                checkFlags(result);
            } else {
                flags |= DIVISION_BY_ZERO_FLAG;
            }
            break;
        }

        case Op::CMP: {
            int32_t result = static_cast<int32_t>(int_registers[ins.r1]) - static_cast<int32_t>(int_registers[ins.r2]);
            checkFlags(result);
            break;
        }

        case Op::AND:
            int_registers[ins.r1] = int_registers[ins.r2] & int_registers[ins.r3];
            checkFlags(static_cast<int32_t>(int_registers[ins.r1]));
            break;

        case Op::OR:
            int_registers[ins.r1] = int_registers[ins.r2] | int_registers[ins.r3];
            checkFlags(static_cast<int32_t>(int_registers[ins.r1]));
            break;

        case Op::XOR:
            int_registers[ins.r1] = int_registers[ins.r2] ^ int_registers[ins.r3];
            checkFlags(static_cast<int32_t>(int_registers[ins.r1]));
            break;

        case Op::NOT_OP:
            int_registers[ins.r1] = ~int_registers[ins.r2];
            checkFlags(static_cast<int32_t>(int_registers[ins.r1]));
            break;

        case Op::SHL:
            int_registers[ins.r1] = int_registers[ins.r2] << int_registers[ins.r3];
            checkFlags(static_cast<int32_t>(int_registers[ins.r1]));
            break;

        case Op::SHR:
            int_registers[ins.r1] = int_registers[ins.r2] >> int_registers[ins.r3];
            checkFlags(static_cast<int32_t>(int_registers[ins.r1]));
            break;

        case Op::JMP:
            program_counter = ins.imm;
            break;

        case Op::JMPR:
            program_counter = int_registers[ins.r1];
            break;

        case Op::JZ:
            if (flags & ZERO_FLAG) {
                program_counter = ins.imm;
            }
            break;

        case Op::JNZ:
            if (!(flags & ZERO_FLAG)) {
                program_counter = ins.imm;
            }
            break;

        case Op::JC:
            if (flags & CARRY_FLAG) {
                program_counter = ins.imm;
            }
            break;

        case Op::JNC:
            if (!(flags & CARRY_FLAG)) {
                program_counter = ins.imm;
            }
            break;

        case Op::JS:
            if (flags & NEGATIVE_FLAG) {
                program_counter = ins.imm;
            }
            break;

        case Op::JNS:
            if (!(flags & NEGATIVE_FLAG)) {
                program_counter = ins.imm;
            }
            break;

        case Op::JO:
            if (flags & OVERFLOW_FLAG) {
                program_counter = ins.imm;
            }
            break;

        case Op::JNO:
            if (!(flags & OVERFLOW_FLAG)) {
                program_counter = ins.imm;
            }
            break;

        case Op::PUSH:
            memory[stack_pointer--] = int_registers[ins.r1];
            break;

        case Op::POP:
            int_registers[ins.r1] = memory[++stack_pointer];
            break;

        case Op::CALL:
            memory[stack_pointer--] = program_counter;
            program_counter = ins.imm;
            break;

        case Op::CALLR:
            memory[stack_pointer--] = program_counter;
            program_counter = int_registers[ins.r1];
            break;

        case Op::RET:
            program_counter = memory[++stack_pointer];
            break;

        default:
            std::cerr << "Unknown opcode" << std::endl;
            return;
    }
}

bool VirtualMachine::getRedrawFlag()
{
    return redraw_flag;
}

void VirtualMachine::setRedrawFlag(bool flag)
{
    redraw_flag = flag;
}


void VirtualMachine::handleInterrupt(std::uint64_t intNumber) {
    switch (intNumber) {
        case PRINT_CHAR:
            std::cout << static_cast<char>(int_registers[0]);
            break;
        case PRINT_INT:
            std::cout << int_registers[0];
            break;
        // case PRINT_FLOAT:
        //     std::cout << float_registers[0];
        //     break;
        case VGA_UPDATE:
            // std::cout << "VGA_UPDATE: ";
            redraw_flag = true;
            copyMemToVideoMem(); // VGA 업데이트
            break;
        default:
            std::cerr << "Unknown interrupt: " << intNumber << std::endl;
    }
}

void VirtualMachine::initializeMemory(const std::unordered_map<std::uint32_t, std::uint32_t>& init_values) {
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
void VirtualMachine::clearFlag(Flags flag) {
    flags &= ~flag;
}
bool  VirtualMachine::isFlagSet(Flags flag) {
    return (flags & flag) != 0;
}

void VirtualMachine::checkFlags(int32_t result)
{

        // Set overflow flag if the result overflows the 32-bit range
    if (result > std::numeric_limits<std::int32_t>::max() || result < std::numeric_limits<std::int32_t>::min()) {
        flags |= OVERFLOW_FLAG;
    } else {
        flags &= ~OVERFLOW_FLAG;
    }

    // Set zero flag if the result is zero
    if (result == 0) {
        flags |= ZERO_FLAG;
    } else {
        flags &= ~ZERO_FLAG;
    }

    // Set negative flag if the result is negative
    if (result < 0) {
        flags |= NEGATIVE_FLAG;
    } else {
        flags &= ~NEGATIVE_FLAG;
    }

    // Set carry flag if the result is greater than the maximum value of 32-bit integer
    if (result > std::numeric_limits<std::int32_t>::max()) {
        flags |= CARRY_FLAG;
    } else {
        flags &= ~CARRY_FLAG;
    }
}

// void VirtualMachine::checkFloatFlags(double value) {
//     if (std::isnan(value)) {
//         // Handle NaN case
//         // Set appropriate flags or take actions
//     } else if (std::isinf(value)) {
//         // Handle Infinity case
//         // Set appropriate flags or take actions
//     } else if (value == 0.0) {
//         flags |= FLOAT_ZERO_FLAG;
//     } else if (value < 0.0) {
//         flags |= FLOAT_NEGATIVE_FLAG;
//     }
//     // You can add more checks depending on your architecture's design
// }



void VirtualMachine::printMemoryAtAddress(std::uint32_t address, bool asAscii = false) {
     std::cout << std::dec << std::endl;
    if (address < memory.size()) {
        std::cout << "Memory[" << address << "] = " << std::hex << std::setw(16) << std::setfill('0') << memory[address];
        
        if (asAscii) {
            char asciiChar = static_cast<char>(memory[address] & 0xFF);
            // std::cout << " (ASCII: " << asciiChar << ")";
        }

        
        std::cout << std::dec << std::endl;
    } else {
        std::cerr << "Memory address out of range: " << address << std::endl;
    }
}
std::uint32_t VirtualMachine::get_int_register(int idx){
    return int_registers[idx];
}


VirtualDisplay& VirtualMachine::getDisplay(void){
    return display;
}
