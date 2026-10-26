# Umicom Framework
# File: examples/report_comparison/CMakeLists.txt
# PURPOSE: Compile a saved-comparison consumer against one explicit installed SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomReportComparisonConsumer LANGUAGES C)
if(NOT IS_ABSOLUTE "${UMICOM_SDK_PREFIX}" OR NOT IS_DIRECTORY "${UMICOM_SDK_PREFIX}")
    message(FATAL_ERROR "Set UMICOM_SDK_PREFIX to an absolute installed Framework prefix")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS testing
    PATHS "${UMICOM_SDK_PREFIX}" NO_DEFAULT_PATH)
umicom_require_installed_sdk("${UMICOM_SDK_PREFIX}" Umicom::testing Umicom::platform)
add_executable(report-comparison-consumer main.c)
target_link_libraries(report-comparison-consumer PRIVATE Umicom::testing)
set_target_properties(report-comparison-consumer PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(report-comparison-consumer PRIVATE /W4)
else()
    target_compile_options(report-comparison-consumer PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    foreach(scenario recovered regressed removed unchanged-failure empty malformed)
        add_test(NAME installed.comparison.${scenario} COMMAND "${CMAKE_COMMAND}"
            "-DPROGRAM=$<TARGET_FILE:report-comparison-consumer>"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/evidence/${scenario}"
            "-DSCENARIO=${scenario}" -P "${CMAKE_CURRENT_SOURCE_DIR}/check.cmake")
        set_tests_properties(installed.comparison.${scenario} PROPERTIES TIMEOUT 30)
    endforeach()
endif()
