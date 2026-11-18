# Umicom Framework
# File: tests/toolchain/document_check/CMakeLists.txt
# PURPOSE: Qualify bounded document checking with real portable services.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT

cmake_minimum_required(VERSION 3.25)
project(UmicomDocumentSourceChecks LANGUAGES C)
include(CTest)
get_filename_component(_framework "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
find_package(Threads REQUIRED)
add_library(document-check-platform STATIC
    "${_framework}/src/platform/document_store.c"
    "${_framework}/src/platform/threading.c"
    "${_framework}/src/platform/filesystem.c"
    "${_framework}/src/platform/path.c"
    "${_framework}/src/platform/atomic_file.c")
target_include_directories(document-check-platform PUBLIC "${_framework}/include")
target_compile_features(document-check-platform PUBLIC c_std_23)
target_link_libraries(document-check-platform PUBLIC Threads::Threads)
include("${_framework}/cmake/UmicomCPreview.cmake")
umicom_add_cvm_document_check(document-check-subject document-check-platform)
add_executable(umicom-document-source-check-test "../test_cvm_document_check.c")
target_link_libraries(umicom-document-source-check-test PRIVATE document-check-subject)
add_executable(umicom-document-copy-allocation-test "../test_document_copy_allocation.c")
target_link_libraries(umicom-document-copy-allocation-test PRIVATE document-check-platform)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    foreach(_target document-check-platform document-check-subject umicom-document-source-check-test umicom-document-copy-allocation-test)
        target_compile_options(${_target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endforeach()
endif()
foreach(_case IN ITEMS warning error stale closed isolation capacity arguments projection nonexecution copy concurrent)
    add_test(NAME framework.toolchain.document-check.${_case} COMMAND umicom-document-source-check-test ${_case})
    set_tests_properties(framework.toolchain.document-check.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;document;source-check;ownership")
endforeach()
add_test(NAME framework.document.bounded-copy-allocation COMMAND umicom-document-copy-allocation-test)
