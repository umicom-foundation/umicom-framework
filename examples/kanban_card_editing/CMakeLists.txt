# Umicom Framework
# File: examples/kanban_card_editing/CMakeLists.txt
# PURPOSE: Consume installed public card-editing services independently of the source tree.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomKanbanCardEditingConsumer LANGUAGES C)
if(NOT IS_ABSOLUTE "${UMICOM_SDK_PREFIX}" OR NOT IS_DIRECTORY "${UMICOM_SDK_PREFIX}")
    message(FATAL_ERROR "Set UMICOM_SDK_PREFIX to an absolute installed Framework prefix")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS kanban
    PATHS "${UMICOM_SDK_PREFIX}" NO_DEFAULT_PATH)
umicom_require_installed_sdk("${UMICOM_SDK_PREFIX}" Umicom::kanban Umicom::base)
add_executable(kanban-card-editing-consumer main.c)
target_link_libraries(kanban-card-editing-consumer PRIVATE Umicom::kanban)
set_target_properties(kanban-card-editing-consumer PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(kanban-card-editing-consumer PRIVATE /W4)
else()
    target_compile_options(kanban-card-editing-consumer PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    foreach(scenario roundtrip stale cycle labels-limits)
        add_test(NAME installed.kanban_card_editing.${scenario} COMMAND kanban-card-editing-consumer ${scenario})
        set_tests_properties(installed.kanban_card_editing.${scenario} PROPERTIES TIMEOUT 30)
    endforeach()
endif()
