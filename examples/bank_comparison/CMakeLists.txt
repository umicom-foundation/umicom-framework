# Umicom Framework
# File: examples/bank_comparison/CMakeLists.txt
# PURPOSE: Consume bank comparison from one explicit installed Framework SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomBankComparisonConsumer LANGUAGES C)
if(NOT IS_ABSOLUTE "${UMICOM_SDK_PREFIX}" OR NOT IS_DIRECTORY "${UMICOM_SDK_PREFIX}")
    message(FATAL_ERROR "Set UMICOM_SDK_PREFIX to an absolute installed Framework prefix")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS bank_feed
    PATHS "${UMICOM_SDK_PREFIX}" NO_DEFAULT_PATH)
umicom_require_installed_sdk("${UMICOM_SDK_PREFIX}" Umicom::bank_feed Umicom::finance Umicom::base)
add_executable(bank-comparison-consumer main.c)
target_link_libraries(bank-comparison-consumer PRIVATE Umicom::bank_feed)
set_target_properties(bank-comparison-consumer PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(bank-comparison-consumer PRIVATE /W4)
else()
    target_compile_options(bank-comparison-consumer PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    foreach(scenario compare missing-id)
        add_test(NAME installed.bank_comparison.${scenario} COMMAND bank-comparison-consumer ${scenario})
        set_tests_properties(installed.bank_comparison.${scenario} PROPERTIES TIMEOUT 30)
    endforeach()
endif()
