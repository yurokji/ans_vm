# CMake generated Testfile for 
# Source directory: /home/doldoki/code/ans_vm
# Build directory: /home/doldoki/code/ans_vm/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(vm_tests "/home/doldoki/code/ans_vm/build/vm_tests")
set_tests_properties(vm_tests PROPERTIES  _BACKTRACE_TRIPLES "/home/doldoki/code/ans_vm/CMakeLists.txt;83;add_test;/home/doldoki/code/ans_vm/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
subdirs("_deps/sfml-build")
