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

// 가상 머신을 위한 인스트럭션 생성
std::vector<Instr> generateInstr(const std::string& str, std::uint32_t startAddr) {
    std::vector<Instr> program;
    program.push_back(makeRI(Op::SET, 6, startAddr));              // r6 = address pointer
    program.push_back(makeRI(Op::SET, 1, 0));                      // counter
    program.push_back(makeRI(Op::SET, 2, 1));                      // step
    program.push_back(makeRI(Op::SET, 3, static_cast<uint32_t>(str.size()))); // length

    std::size_t loop_idx = program.size();
    program.push_back({Op::LOAD, 4, 6, 0, 0});                    // r4 = mem[r6]
    program.push_back({Op::OR, 0, 4, 4, 0});                      // r0 = r4 (for printing)
    program.push_back(makeINT(PRINT_CHAR));                       // print r0
    program.push_back({Op::ADD, 6, 6, 2, 0});                     // r6 += 1
    program.push_back({Op::ADD, 1, 1, 2, 0});                     // counter += 1
    program.push_back({Op::CMP, 1, 3, 0, 0});                     // counter - length (flags only)
    program.push_back({Op::JNZ, 0, 0, 0, static_cast<uint32_t>(loop_idx)}); // loop if not done
    program.push_back({Op::HALT, 0, 0, 0, 0});
    return program;
}

int main() {
    // 가상 머신 생성
    VirtualMachine vm(1024);

    // 소스 코드에서 문자열을 추출 (실제로는 파싱을 해야 함)
    std::string str = extractString();

    // 문자열을 가상 머신의 메모리에 로드
    loadStringToMemory(vm, str, 10);  // 시작 주소는 10으로 가정

    // 인스트럭션 생성
    auto program = generateInstr(str, 10);

    // 가상 머신 실행
    vm.run(program);
    

    return 0;
}
