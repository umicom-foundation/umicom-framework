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
