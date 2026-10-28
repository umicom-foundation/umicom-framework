#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-federated-evidence-anchor/CMakeLists.txt
#
# PURPOSE:
#   Build standalone tests with the canonical Framework SHA-256, ordered
#   record chain, Merkle service and additive federated evidence anchor.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeFederatedEvidenceAnchor LANGUAGES C)
include(CTest)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
get_filename_component(FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
add_library(umicom-evidence-anchor-native STATIC
    "${FRAMEWORK_ROOT}/src/base/sha256.c"
    "${FRAMEWORK_ROOT}/src/record_chain/chain.c"
    "${FRAMEWORK_ROOT}/src/record_chain/merkle.c"
    "${FRAMEWORK_ROOT}/src/ai/training/federated_evidence_anchor.c")
target_include_directories(umicom-evidence-anchor-native PUBLIC
    "${FRAMEWORK_ROOT}/include")
if(MSVC)
    target_compile_options(umicom-evidence-anchor-native PRIVATE /W4)
else()
    target_compile_options(umicom-evidence-anchor-native PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
add_executable(umicom-native-evidence-anchor-demo demo.c)
target_link_libraries(umicom-native-evidence-anchor-demo PRIVATE
    umicom-evidence-anchor-native)
if(BUILD_TESTING)
    add_executable(umicom-native-evidence-anchor-test
        "${FRAMEWORK_ROOT}/tests/ai_training/test_federated_evidence_anchor.c")
    target_link_libraries(umicom-native-evidence-anchor-test PRIVATE
        umicom-evidence-anchor-native)
    foreach(_case IN ITEMS happy tamper rejection)
        add_test(NAME "framework.ai.federated_evidence.${_case}"
            COMMAND umicom-native-evidence-anchor-test "${_case}")
    endforeach()
    add_test(NAME framework.ai.federated_evidence.demo
        COMMAND umicom-native-evidence-anchor-demo)
endif()
