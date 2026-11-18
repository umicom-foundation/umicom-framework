# -----------------------------------------------------------------------------
# Umicom Framework
# File: tests/frontend_reload/CMakeLists.txt
#
# PURPOSE:
#   Qualify the production reload queue without starting a GUI or server.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomReloadChecks LANGUAGES C)
    enable_testing()
endif()
get_filename_component(_reload_framework "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
add_executable(umicom-frontend-reload-batch-tests test_reload_batch.c
    "${_reload_framework}/src/frontend/hot_reload.c"
    "${_reload_framework}/src/frontend/dev_types.c")
target_include_directories(umicom-frontend-reload-batch-tests PRIVATE "${_reload_framework}/include")
set_target_properties(umicom-frontend-reload-batch-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-frontend-reload-batch-tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
foreach(_case IN ITEMS interleaved failed all-newer invalid limits)
    add_test(NAME "framework.frontend.reload_batch.${_case}"
        COMMAND umicom-frontend-reload-batch-tests "${_case}")
    set_tests_properties("framework.frontend.reload_batch.${_case}" PROPERTIES TIMEOUT 15 LABELS "frontend;reload;memory")
endforeach()
