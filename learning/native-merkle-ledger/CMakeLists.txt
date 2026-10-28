#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-merkle-ledger/CMakeLists.txt
#
# PURPOSE:
#   Compile the actual Framework Merkle module alongside its existing SHA-256
#   and native regression test in a headless standalone educational build.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeMerkle LANGUAGES C)
include(CTest)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
get_filename_component(FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
add_library(umicom-merkle-native STATIC
    "${FRAMEWORK_ROOT}/src/record_chain/merkle.c"
    "${FRAMEWORK_ROOT}/src/base/sha256.c")
target_include_directories(umicom-merkle-native PUBLIC "${FRAMEWORK_ROOT}/include")
if(MSVC)
  target_compile_options(umicom-merkle-native PRIVATE /W4)
else()
  target_compile_options(umicom-merkle-native PRIVATE
    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
add_executable(umicom-native-merkle-demo demo.c)
target_link_libraries(umicom-native-merkle-demo PRIVATE umicom-merkle-native)
if(BUILD_TESTING)
    add_executable(umicom-native-merkle-test
        "${FRAMEWORK_ROOT}/tests/record_chain/test_merkle.c")
    target_link_libraries(umicom-native-merkle-test PRIVATE umicom-merkle-native)
    add_test(NAME framework.record_chain.merkle_standalone COMMAND umicom-native-merkle-test)
    add_test(NAME framework.record_chain.merkle_demo COMMAND umicom-native-merkle-demo)
endif()
