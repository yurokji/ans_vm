cmake_minimum_required(VERSION 3.10)

project(VirtualMachine)

# 빌드 형식을 설정합니다. 여기서는 디버그 모드로 설정했습니다.
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 컴파일러에게 헤더 파일의 위치를 알려줍니다.
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${GTEST_INCLUDE_DIRS})


find_package(GTest REQUIRED)

# 소스 파일을 변수에 추가합니다.
set(SOURCES
    src/vm.cpp
    src/vm_display_test.cpp
)

# SFML을 찾도록 CMake에 지시합니다.
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

# 실행 파일을 만듭니다.
add_executable(VirtualMachine ${SOURCES})

# 실행 파일과 SFML 라이브러리를 연결합니다.
target_link_libraries(VirtualMachine sfml-graphics sfml-audio ${GTEST_LIBRARIES} pthread)