#include "vm.h"
#include <iostream>

VirtualMachine::VirtualMachine(size_t memorySize) : display(DISPLAY_WIDTH, DISPLAY_HEIGHT) {
    memory.resize(memorySize, 0);
    stack_pointer = memorySize - 1;
    program_counter = 0;
    redraw_flag = true;

    // Initialize integer and float registers to zero
    std::fill(int_registers, int_registers + 16, 0);
    std::fill(float_registers, float_registers + 16, 0.0);
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
                    idx++;
                }
            }
        }
    }


    return 0;
}

void VirtualMachine::load(const std::vector<std::pair<ByteCode, std::uint32_t>>& bytecode) {
    program = bytecode;
}

void VirtualMachine::proceed() {
    std::uint32_t reg1, reg2, reg3, value;
    if (program_counter < program.size()) {
        auto [opcode, operand] = program[program_counter++];
        switch (opcode) {
            case NOP:
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                break;

            case INT:
                handleInterrupt(operand);
                break;

            case HALT:
                return;

            case LOAD: {
                std::tie(reg1, reg2) = decodeTwoRegisters(operand);
                int_registers[reg1] = memory[int_registers[reg2]];
                break;
            }

                
            case LOADI: {
                std::tie(reg1, value) = decodeRegisterAndImmediate(operand);
                int_registers[reg1] = memory[value];
                // std::cout<<std::hex<<int_registers[reg1]<<std::endl;
                break;
            }


            case STORE:
                memory[int_registers[++program_counter]] = int_registers[operand];
                break;

            case STOREI: {
                std::tie(reg1, value) = decodeRegisterAndImmediate(operand);
                // std::cout<<reg1 <<" "<<int_registers[reg1]<<" "<<value <<std::endl;
                memory[int_registers[reg1]] = value;
                break;
            }

            case SETH: {
                std::tie(reg1, value) = decodeRegisterAndImmediate(operand);
                int_registers[reg1] &= value; // Clear upper 32 bits
                break;
            }

            case CLEAR:
                // Clear the contents of the given register
                int_registers[operand] = 0;
                break;

            case ADD: {
                std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
                int32_t result = static_cast<int32_t>(int_registers[reg2]) + static_cast<int32_t>(int_registers[reg3]);
                checkFlags(result);
                // std::cout << reg1 <<reg2 <<reg3 << int_registers[reg1] <<int_registers[reg2] << result <<std::endl;
                int_registers[reg1] = static_cast<std::uint32_t>(result);
                break;
            }

            case SUB: {
                std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
                int32_t result = static_cast<int32_t>(int_registers[reg2]) - static_cast<int32_t>(int_registers[reg3]);
                checkFlags(result);
                int_registers[reg1] = static_cast<std::uint32_t>(result);
                break;
            }
            
            case MUL: {
                std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
                int32_t result = static_cast<int32_t>(int_registers[reg2]) * static_cast<int32_t>(int_registers[reg3]);

                // Check for overflow·
                if (result > std::numeric_limits<std::uint32_t>::max() || result < std::numeric_limits<std::int32_t>::min()) {
                    // Handle overflow error
                    // You can choose to set flags, throw an exception, or take other actions
                    // depending on your architecture design
                    flags |= OVERFLOW_FLAG;
                } else {
                    int_registers[reg1] = static_cast<std::uint32_t>(result);
                    checkFlags(int_registers[reg1]);
                }
                break;
            }

            case DIV: {
                std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
                int32_t divisor = static_cast<int32_t>(int_registers[reg3]);

                if (divisor != 0) {
                    int32_t result = static_cast<int32_t>(int_registers[reg2]) / divisor;
                    int_registers[reg1] = static_cast<std::uint32_t>(result);
                    checkFlags(result);
                } else {
                    // Handle division by zero error
                    // For example, set the division by zero flag
                    flags |= DIVISION_BY_ZERO_FLAG;
                }
                break;
            }

            case MOD: {
                std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
                int32_t divisor = static_cast<int32_t>(int_registers[reg3]);

                if (divisor != 0) {
                    int32_t result = static_cast<int32_t>(int_registers[reg2]) % divisor;
                    int_registers[reg1] = static_cast<std::uint32_t>(result);
                    checkFlags(int_registers[reg1]);
                } else {
                    // Handle division by zero error
                    // You can choose to set flags, throw an exception, or take other actions
                    // depending on your architecture design
                    flags |= DIVISION_BY_ZERO_FLAG;
                }
                break;
            }


            case CMP: {
                std::tie(reg1, reg2) = decodeTwoRegisters(operand);
                int32_t result = static_cast<int32_t>(int_registers[reg1]) - static_cast<int32_t>(int_registers[reg2]);
                checkFlags(result);
                break;
            }
            


            case AND: {
                std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
                int_registers[reg1] = int_registers[reg2] & int_registers[reg3];
                checkFlags(int_registers[reg1]);
                break;
            }

            case OR: {
                std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
                int_registers[reg1] = int_registers[reg2] | int_registers[reg3];
                checkFlags(int_registers[reg1]);
                break;
            }

            case XOR: {
                std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
                int_registers[reg1] = int_registers[reg2] ^ int_registers[reg3];
                checkFlags(int_registers[reg1]);
                break;
            }

            case NOT: {
                std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
                int_registers[reg1] = ~int_registers[reg2];
                checkFlags(int_registers[reg1]);
                break;
            }

            case SHL: {
                std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
                int_registers[reg1] = int_registers[reg2] << int_registers[reg3];
                checkFlags(int_registers[reg1]);
                break;
            }

            case SHR: {
                std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
                int_registers[reg1] = int_registers[reg2] >> int_registers[reg3];
                checkFlags(int_registers[reg1]);
                break;
            }



            case JMP: {
                program_counter = operand;
                break;
            }


            case JZ: {
                if (flags & ZERO_FLAG) {
                    program_counter = operand;
                }
                break;
            }

            case JNZ: {
                if (!(flags & ZERO_FLAG)) {
                    program_counter = operand;
                }
                break;
            }

            case JC: {
                if (flags & CARRY_FLAG) {
                    program_counter = operand;
                }
                break;
            }

            case JNC: {
                if (!(flags & CARRY_FLAG)) {
                    program_counter = operand;
                }
                break;
            }

            case JS: {
                if (flags & NEGATIVE_FLAG) {
                    program_counter = operand;
                }
                break;
            }

            case JNS: {
                if (!(flags & NEGATIVE_FLAG)) {
                    program_counter = operand;
                }
                break;
            }

            case JO: {
                if (flags & OVERFLOW_FLAG) {
                    program_counter = operand;
                }
                break;
            }

            case JNO: {
                if (!(flags & OVERFLOW_FLAG)) {
                    program_counter = operand;
                }
                break;
            }


            case PUSH: {
                reg1 = operand;
                // Push the value in the specified register onto the stack
                // Decrement the stack_pointer, and then store the value from the register into memory at the stack_pointer address
                memory[stack_pointer--] = int_registers[reg1];
                break;
            }

            case POP: {
                reg1 = operand;
                // Pop a value from the stack into the specified register
                // Increment the stack_pointer, and then load the value from memory at the stack_pointer+1 address into the register
                int_registers[reg1] = memory[++stack_pointer];
                break;
            }

            case CALL: {
                reg1 = operand;
                // Save the return address (program_counter + 1) onto the stack
                memory[stack_pointer--] = program_counter + 1;

                // Set program_counter to the address in the specified register
                program_counter = int_registers[reg1];
                break;
            }

            case RET: {
                // Pop the return address from the stack and assign it to program_counter
                program_counter = memory[++stack_pointer];
                break;
            }


            // case FADD: {
            //     std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
            //     float result = float_registers[reg2] + float_registers[reg3];
            //     float_registers[reg1] = result;
            //     checkFloatFlags(result);
            //     break;
            // }

            // case FSUB: {
            //     std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
            //     float result = float_registers[reg2] - float_registers[reg3];
            //     float_registers[reg1] = result;
            //     checkFloatFlags(result);
            //     break;
            // }

            // case FMUL: {
            //     std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
            //     float result = float_registers[reg2] * float_registers[reg3];
            //     float_registers[reg1] = result;
            //     checkFloatFlags(result);
            //     break;
            // }

            // case FDIV: {
            //     std::tie(reg1, reg2, reg3) = decodeThreeRegisters(operand);
            //     float result = float_registers[reg2] / float_registers[reg3];
            //     float_registers[reg1] = result;
            //     checkFloatFlags(result);
            //     break;
            // }

            // case FCMP: {
            //     std::tie(reg1, reg2) = decodeTwoRegisters(operand);
            //     float result = float_registers[reg1] - float_registers[reg2];
            //     checkFloatFlags(result);
            //     break;
            // }


            // case FJZ: {
            //     if (flags & FLOAT_ZERO_FLAG) {
            //         // Jump
            //         program_counter = static_cast<std::size_t>(operand);
            //     }
            //     break;
            // }

            // case FJNZ: {
            //     if (!(flags & FLOAT_ZERO_FLAG)) {
            //         // Jump
            //         program_counter = static_cast<std::size_t>(operand);
            //     }
            //     break;
            // }

            // case FJC: {
            //     if (flags & FLOAT_CARRY_FLAG) {
            //         // Jump
            //         program_counter = static_cast<std::size_t>(operand);
            //     }
            //     break;
            // }

            // case FJNC: {
            //     if (!(flags & FLOAT_CARRY_FLAG)) {
            //         // Jump
            //         program_counter = static_cast<std::size_t>(operand);
            //     }
            //     break;
            // }

            // case FJS: {
            //     if (flags & FLOAT_NEGATIVE_FLAG) {
            //         // Jump
            //         program_counter = static_cast<std::size_t>(operand);
            //     }
            //     break;
            // }

            // case FJNS: {
            //     if (!(flags & FLOAT_NEGATIVE_FLAG)) {
            //         // Jump
            //         program_counter = static_cast<std::size_t>(operand);
            //     }
            //     break;
            // }

            // case FJO: {
            //     if (flags & FLOAT_OVERFLOW_FLAG) {
            //         // Jump
            //         program_counter = static_cast<std::size_t>(operand);
            //     }
            //     break;
            // }

            // case FJNO: {
            //     if (!(flags & FLOAT_OVERFLOW_FLAG)) {
            //         // Jump
            //         program_counter = static_cast<std::size_t>(operand);
            //     }
            //     break;
            // }

            default:
                std::cerr << "Unknown opcode: " << opcode << std::endl;
                return;
        }
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


void VirtualMachine::handleInterrupt(std::uint32_t intNumber) {
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
        case VGA_UPDATE:
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

void VirtualMachine::checkFloatFlags(double value) {
    if (std::isnan(value)) {
        // Handle NaN case
        // Set appropriate flags or take actions
    } else if (std::isinf(value)) {
        // Handle Infinity case
        // Set appropriate flags or take actions
    } else if (value == 0.0) {
        flags |= FLOAT_ZERO_FLAG;
    } else if (value < 0.0) {
        flags |= FLOAT_NEGATIVE_FLAG;
    }
    // You can add more checks depending on your architecture's design
}



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
