#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/action_recovery/CMakeLists.txt
# PURPOSE: Build the failure-feedback example against the installed or source SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomActionRecoveryExample LANGUAGES C)
    include(CTest)
    find_package(UmicomFramework CONFIG REQUIRED)
endif()
add_executable(umicom-action-recovery-example main.c)
target_link_libraries(umicom-action-recovery-example PRIVATE Umicom::trading_ui Umicom::application)
set_target_properties(umicom-action-recovery-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-action-recovery-example PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-action-recovery-example)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-action-recovery-example)
endif()
if(BUILD_TESTING)
    add_test(NAME framework.action_recovery.example COMMAND umicom-action-recovery-example)
    set_tests_properties(framework.action_recovery.example PROPERTIES TIMEOUT 20 LABELS "framework;action-recovery;example")
endif()
