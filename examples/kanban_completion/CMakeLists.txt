# Umicom Framework
# File: examples/kanban_completion/CMakeLists.txt
# PURPOSE: Consume the installed public planning and private evidence-file APIs.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomKanbanCompletionConsumer LANGUAGES C)
if(NOT IS_ABSOLUTE "${UMICOM_SDK_PREFIX}" OR NOT IS_DIRECTORY "${UMICOM_SDK_PREFIX}")
    message(FATAL_ERROR "Set UMICOM_SDK_PREFIX to an absolute installed Framework prefix")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS kanban_storage
    PATHS "${UMICOM_SDK_PREFIX}" NO_DEFAULT_PATH)
umicom_require_installed_sdk("${UMICOM_SDK_PREFIX}" Umicom::kanban_storage Umicom::kanban Umicom::base Umicom::platform)
add_executable(kanban-completion-consumer main.c)
target_link_libraries(kanban-completion-consumer PRIVATE Umicom::kanban_storage)
set_target_properties(kanban-completion-consumer PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(kanban-completion-consumer PRIVATE /W4)
else()
    target_compile_options(kanban-completion-consumer PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    foreach(scenario normal blocked stale existing suffix)
        add_test(NAME installed.kanban_completion.${scenario} COMMAND "${CMAKE_COMMAND}"
            "-DPROGRAM=$<TARGET_FILE:kanban-completion-consumer>"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/evidence/${scenario}"
            "-DSCENARIO=${scenario}" -P "${CMAKE_CURRENT_SOURCE_DIR}/check.cmake")
        set_tests_properties(installed.kanban_completion.${scenario} PROPERTIES TIMEOUT 30)
    endforeach()
endif()
