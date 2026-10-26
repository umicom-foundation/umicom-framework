# Umicom Framework
# File: examples/host_coverage/CMakeLists.txt
# PURPOSE: Build a public-API consumer solely from an explicitly selected SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomHostCoverageConsumer LANGUAGES C)
if(NOT IS_ABSOLUTE "${UMICOM_SDK_PREFIX}" OR NOT IS_DIRECTORY "${UMICOM_SDK_PREFIX}")
    message(FATAL_ERROR "Set UMICOM_SDK_PREFIX to an absolute installed Framework prefix")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS testing
    PATHS "${UMICOM_SDK_PREFIX}" NO_DEFAULT_PATH)
# Checking exported targets and their public header fingerprint prevents an
# incidental source-tree or globally registered package from satisfying this example.
umicom_require_installed_sdk("${UMICOM_SDK_PREFIX}" Umicom::testing Umicom::platform)
add_executable(host-coverage-consumer main.c)
target_link_libraries(host-coverage-consumer PRIVATE Umicom::testing)
set_target_properties(host-coverage-consumer PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(host-coverage-consumer PRIVATE /W4)
else()
    target_compile_options(host-coverage-consumer PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    foreach(scenario passed skipped stale malformed filtered-passed filtered-empty filtered-text)
        add_test(NAME installed.coverage.${scenario} COMMAND "${CMAKE_COMMAND}"
            "-DPROGRAM=$<TARGET_FILE:host-coverage-consumer>"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/evidence/${scenario}"
            "-DSCENARIO=${scenario}" -P "${CMAKE_CURRENT_SOURCE_DIR}/check.cmake")
        set_tests_properties(installed.coverage.${scenario} PROPERTIES TIMEOUT 30)
    endforeach()
endif()
