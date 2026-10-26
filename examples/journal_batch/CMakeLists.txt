# Umicom Framework
# File: examples/journal_batch/CMakeLists.txt
# PURPOSE: Exercise complete journal-batch review using one explicitly selected installed SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomJournalBatch LANGUAGES C)
if(NOT IS_ABSOLUTE "${UMICOM_SDK_PREFIX}" OR NOT IS_DIRECTORY "${UMICOM_SDK_PREFIX}")
    message(FATAL_ERROR "Set UMICOM_SDK_PREFIX to an absolute installed Framework prefix")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS finance
    PATHS "${UMICOM_SDK_PREFIX}" NO_DEFAULT_PATH)
umicom_require_installed_sdk("${UMICOM_SDK_PREFIX}" Umicom::finance Umicom::base)
add_executable(journal-batch main.c)
target_link_libraries(journal-batch PRIVATE Umicom::finance)
set_target_properties(journal-batch PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(journal-batch PRIVATE /W4)
else()
    target_compile_options(journal-batch PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    foreach(scenario apply draft-change chart-change duplicate later-refusal report report-stale report-after-apply activity activity-range)
        add_test(NAME installed.accounting.journal.batch.${scenario} COMMAND journal-batch ${scenario})
        set_tests_properties(installed.accounting.journal.batch.${scenario} PROPERTIES TIMEOUT 30)
    endforeach()
endif()
