cmake_minimum_required(VERSION 3.14)

project(VirtualMachine)

# 빌드 형식을 설정합니다. 여기서는 디버그 모드로 설정했습니다.
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 컴파일러에게 헤더 파일의 위치를 알려줍니다.
include_directories(${CMAKE_SOURCE_DIR}/include)

# 소스 파일을 변수에 추가합니다.
set(SOURCES
    src/vm.cpp
    src/main.cpp
)

include(FetchContent)

# Optional: GoogleTest for unit tests.
find_package(GTest QUIET)
if(NOT GTest_FOUND)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()

# SFML: try system first, otherwise fetch from upstream. Audio disabled to reduce deps.
find_package(SFML 2.5 COMPONENTS graphics window system QUIET)
if(NOT SFML_FOUND)
  message(STATUS "SFML not found on system; fetching SFML 2.6.1 via FetchContent")
  FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
  )
  set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_GRAPHICS ON CACHE BOOL "" FORCE)
  set(SFML_BUILD_WINDOW ON CACHE BOOL "" FORCE)
  set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(SFML_INSTALL_PKGCONFIG_FILES OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(SFML)
endif()

# 실행 파일을 만듭니다.
add_executable(VirtualMachine ${SOURCES})

# 실행 파일과 SFML 라이브러리를 연결합니다.
target_link_libraries(
  VirtualMachine
  sfml-graphics
  sfml-window
  sfml-system
  pthread
)

enable_testing()

add_executable(vm_tests
  src/vm.cpp
  tests/test_instr.cpp
  tests/test_vm.cpp
)

target_link_libraries(
  vm_tests
  gtest_main
  sfml-graphics
  sfml-window
  sfml-system
  pthread
)

add_test(NAME vm_tests COMMAND vm_tests)
