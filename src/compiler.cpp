#include <iostream>
#include <vector>
#include <unordered_map>
#include "vm.h"  // 가상 머신 헤더 포함

enum SourceCode {
    PRINT
};

// 소스 코드에서 문자열을 추출 (실제 구현에서는 파싱 로직이 필요)
std::string extractString() {
    return "Hello";
}

// 문자열을 가상 머신의 메모리에 로드
void loadStringToMemory(VirtualMachine& vm, const std::string& str, std::uint64_t startAddr) {
    for (std::size_t i = 0; i < str.size(); ++i) {
        vm.initializeMemory({{startAddr + i, str[i]}});
    }
}

// 가상 머신을 위한 바이트코드 생성
std::vector<std::pair<ByteCode, std::uint64_t>> generateByteCode(const std::string& str, std::uint64_t startAddr) {
    std::vector<std::pair<ByteCode, std::uint64_t>> bytecode;
    for (std::size_t i = 0; i < str.size(); ++i) {
        bytecode.push_back({LOAD, startAddr + i});
        bytecode.push_back({INT, PRINT_CHAR});
    }
    bytecode.push_back({HALT, 0});
    return bytecode;
}


void outputAssembly(const std::vector<std::pair<ByteCode, std::uint64_t>>& bytecode) {
    for (const auto& instruction : bytecode) {
        switch (instruction.first) {
            case LOAD:
                std::cout << "LOAD " << instruction.second << std::endl;
                break;
            case ADD:
                std::cout << "ADD " << instruction.second << std::endl;
                break;
            case INT:
                std::cout << "INT " << instruction.second << std::endl;
                break;
            case HALT:
                std::cout << "HALT" << std::endl;
                break;
            default:
                std::cout << "UNKNOWN" << std::endl;
        }
    }
}

int main() {
    // 가상 머신 생성
    VirtualMachine vm(1024);

    // 소스 코드에서 문자열을 추출 (실제로는 파싱을 해야 함)
    std::string str = extractString();

    // 문자열을 가상 머신의 메모리에 로드
    loadStringToMemory(vm, str, 10);  // 시작 주소는 10으로 가정

    // 바이트코드 생성
    auto bytecode = generateByteCode(str, 10);

    // // 어셈블리코드 출력
    // outputAssembly(bytecode);


// Load instructions directly (each instruction is some enum or integer)
    std::vector<uint64_t> program = {
        LOAD, 0, 0,       // Load 0 into register 0 (Initialize position to 0)
        LABEL_LOOP,
        LOAD, 1, 0,       // Load current position from register 0 to register 1
        ADD, 1, 0x20,     // Add 0x20 to register 1
        STORE, 1, 2,      // Store this "screen position" into memory at index 2
        // ... (more instructions to print and sleep)
        JMP, LABEL_LOOP   // Jump back to start of loop
    };

    vm.loadProgram(program);  // Load this program into the VM's memory
    vm.run();  // Run the VM
    


    // 가상 머신 실행
    vm.run(bytecode);
    

    return 0;
}