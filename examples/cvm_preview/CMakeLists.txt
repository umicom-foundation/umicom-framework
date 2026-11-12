#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/cvm_preview/CMakeLists.txt
#
# PURPOSE:
#   Expose bounded source preview with explicit result ownership.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#---------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomCvmPreview LANGUAGES C)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomCPreview.cmake")
umicom_add_cvm_preview(umicom_cvm_preview)
add_executable(umicom-cvm-preview main.c)
target_link_libraries(umicom-cvm-preview PRIVATE umicom_cvm_preview)
if(BUILD_TESTING)
    add_executable(umicom-cvm-preview-tests ../../tests/toolchain/test_cvm_preview.c)
    target_link_libraries(umicom-cvm-preview-tests PRIVATE umicom_cvm_preview)
    add_test(NAME framework.toolchain.cvm_preview COMMAND umicom-cvm-preview-tests)
    set_tests_properties(framework.toolchain.cvm_preview PROPERTIES TIMEOUT 15 LABELS "toolchain;cvm")
endif()

# The optional adapter is qualified against the installed compiler/runtime SDK.
if(BUILD_TESTING)
    add_executable(umicom-cvm-trace-tests ../../tests/toolchain/test_cvm_trace.c)
    target_link_libraries(umicom-cvm-trace-tests PRIVATE umicom_cvm_preview)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-cvm-trace-tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    foreach(case IN ITEMS
        success ordinary-parity syntax-after-success null-source arithmetic-failure fuel-zero fuel-return shorter-request independent-storage workspace-size result-size trace-size null-workspace null-result null-trace)
        add_test(NAME "framework.toolchain.cvm_trace.${case}" COMMAND umicom-cvm-trace-tests "${case}")
        set_tests_properties("framework.toolchain.cvm_trace.${case}" PROPERTIES TIMEOUT 15 LABELS "toolchain;cvm;trace")
    endforeach()
endif()

if(BUILD_TESTING)
    # Preserve the trace-only list; comparison results use the same process path.
    # foreach(case IN ITEMS ordinary trace trace-source trace-fault trace-syntax extra-arguments)
    foreach(case IN ITEMS ordinary trace trace-source trace-fault trace-syntax extra-arguments
        comparison-true comparison-false comparison-eager)
        add_test(NAME "framework.toolchain.cvm_preview_cli.${case}"
            COMMAND "${CMAKE_COMMAND}" "-DTOOL=$<TARGET_FILE:umicom-cvm-preview>" "-DCASE=${case}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../../tests/toolchain/check_cvm_preview_cli.cmake")
        set_tests_properties("framework.toolchain.cvm_preview_cli.${case}" PROPERTIES
            TIMEOUT 15 LABELS "toolchain;cvm;trace;cli")
    endforeach()
endif()

# Qualification is explicit: use matching comparison-capable SDK headers and libraries.
if(BUILD_TESTING)
    add_executable(umicom-cvm-comparison-tests ../../tests/toolchain/test_cvm_comparisons.c)
    target_link_libraries(umicom-cvm-comparison-tests PRIVATE umicom_cvm_preview)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-cvm-comparison-tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    foreach(case IN ITEMS true false precedence signed-extremes syntax-reset eager-fault fuel-before-compare fuel-before-return ordinary-parity trace-reset)
        add_test(NAME "framework.toolchain.cvm_comparison.${case}" COMMAND umicom-cvm-comparison-tests "${case}")
        set_tests_properties("framework.toolchain.cvm_comparison.${case}" PROPERTIES TIMEOUT 15 LABELS "toolchain;cvm;comparison;sdk")
    endforeach()
endif()

# Local preview uses the same Framework adapter; no parser or VM is copied here.
if(BUILD_TESTING)
    add_executable(umicom-cvm-local-tests ../../tests/toolchain/test_cvm_locals.c)
    target_link_libraries(umicom-cvm-local-tests PRIVATE umicom_cvm_preview)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-cvm-local-tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    foreach(case IN ITEMS success ordinary-parity syntax-reset fuel-reset capacity-reset)
        add_test(NAME "framework.toolchain.cvm_local.${case}" COMMAND umicom-cvm-local-tests "${case}")
        set_tests_properties("framework.toolchain.cvm_local.${case}" PROPERTIES TIMEOUT 15 LABELS "toolchain;cvm;locals;sdk")
    endforeach()
endif()

# Logical execution remains in Umicom C; Framework owns fresh preview state.
if(BUILD_TESTING)
    add_executable(umicom-cvm-logical-tests ../../tests/toolchain/test_cvm_logical.c)
    target_link_libraries(umicom-cvm-logical-tests PRIVATE umicom_cvm_preview)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-cvm-logical-tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    # The prior four logical cases remain; conditional source uses the same adapter.
    foreach(case IN ITEMS success ordinary-parity fuel-reset syntax-reset
        conditional-fallback conditional-true conditional-fault)
        add_test(NAME "framework.toolchain.cvm_logical.${case}" COMMAND umicom-cvm-logical-tests "${case}")
        set_tests_properties("framework.toolchain.cvm_logical.${case}" PROPERTIES TIMEOUT 15 LABELS "toolchain;cvm;logical;sdk")
    endforeach()
endif()
