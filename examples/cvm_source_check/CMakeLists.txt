#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/cvm_source_check/CMakeLists.txt
#
# PURPOSE:
#   Compose the optional source-check adapter with explicit compiler SDK or source selection.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#---------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)
project(UmicomCvmSourceCheck LANGUAGES C)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomCPreview.cmake")
umicom_add_cvm_source_check(umicom_cvm_source_check)
add_executable(umicom-cvm-source-check main.c)
target_link_libraries(umicom-cvm-source-check PRIVATE umicom_cvm_source_check)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-cvm-source-check PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(WIN32)
    enable_language(RC)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomApplicationBranding.cmake")
get_filename_component(UMICOM_FRAMEWORK_RESOURCE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../resources" ABSOLUTE)
umicom_apply_application_branding(TARGET umicom-cvm-source-check
    PRODUCT_NAME "Umicom Source Check Example" INTERNAL_NAME "umicom-cvm-source-check" NO_RUNTIME_DEPLOYMENT)
if(BUILD_TESTING)
    add_executable(umicom-cvm-source-check-tests ../../tests/toolchain/test_cvm_source_check.c)
    target_link_libraries(umicom-cvm-source-check-tests PRIVATE umicom_cvm_source_check)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-cvm-source-check-tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    foreach(case success reset arguments storage nonexecution parity independent prefixes)
        add_test(NAME "framework.toolchain.source-check.${case}" COMMAND umicom-cvm-source-check-tests "${case}")
        set_tests_properties("framework.toolchain.source-check.${case}" PROPERTIES TIMEOUT 20 LABELS "toolchain;cvm;source-check")
    endforeach()
    add_test(NAME framework.toolchain.source-check.example COMMAND umicom-cvm-source-check)
    set_tests_properties(framework.toolchain.source-check.example PROPERTIES TIMEOUT 20 LABELS "toolchain;cvm;source-check;example"
        PASS_REGULAR_EXPRESSION "Revision 41: 1 warnings; 4 instructions checked, not executed")
endif()
