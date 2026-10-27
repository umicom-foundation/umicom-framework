# Umicom Framework
# File: tests/sanitizers/CMakeLists.txt
# Purpose: Verify disabled policy, explicit refusal and transitive sanitizer linking.
# Author and organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomSanitizerConsumer LANGUAGES C)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomSanitizers.cmake")
if(MODE STREQUAL "disabled")
    set(UMICOM_ENABLE_SANITIZERS OFF)
elseif(MODE STREQUAL "unsupported")
    set(UMICOM_ENABLE_SANITIZERS ON)
    # Isolate the policy rejection without requiring a second installed compiler.
    set(CMAKE_C_COMPILER_ID UnsupportedFixture)
    umicom_check_sanitizers()
    message(FATAL_ERROR "Unsupported compiler was silently accepted")
elseif(MODE STREQUAL "static-link")
    set(UMICOM_ENABLE_SANITIZERS ON)
else()
    message(FATAL_ERROR "Unknown sanitizer fixture mode '${MODE}'")
endif()
add_library(instrumented STATIC value.c)
umicom_apply_sanitizers(instrumented)
umicom_apply_sanitizers(instrumented)
add_executable(consumer main.c)
# Intentionally do not instrument the consumer directly. Its link must inherit
# the runtime requirements from the static library's public usage requirements.
target_link_libraries(consumer PRIVATE instrumented)
if(MODE STREQUAL "disabled")
    get_target_property(options instrumented COMPILE_OPTIONS)
    get_target_property(links instrumented INTERFACE_LINK_OPTIONS)
    if(options OR links)
        message(FATAL_ERROR "Disabled sanitizers changed consumer options")
    endif()
else()
    get_target_property(options instrumented COMPILE_OPTIONS)
    list(LENGTH options count)
    if(NOT count EQUAL 2)
        message(FATAL_ERROR "Repeated sanitizer application duplicated options")
    endif()
endif()
