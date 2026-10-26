# Umicom Framework
# File: examples/journal_posting/CMakeLists.txt
# PURPOSE: Exercise checked journal services using one explicitly selected installed SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomJournalPosting LANGUAGES C)
if(NOT IS_ABSOLUTE "${UMICOM_SDK_PREFIX}" OR NOT IS_DIRECTORY "${UMICOM_SDK_PREFIX}")
    message(FATAL_ERROR "Set UMICOM_SDK_PREFIX to an absolute installed Framework prefix")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS finance
    PATHS "${UMICOM_SDK_PREFIX}" NO_DEFAULT_PATH)
umicom_require_installed_sdk("${UMICOM_SDK_PREFIX}" Umicom::finance Umicom::base)
add_executable(journal-posting main.c)
target_link_libraries(journal-posting PRIVATE Umicom::finance)
set_target_properties(journal-posting PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(journal-posting PRIVATE /W4)
else()
    target_compile_options(journal-posting PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    foreach(scenario apply stale-account closed-period amount-boundary stale-book)
        add_test(NAME installed.accounting.journal.${scenario} COMMAND journal-posting ${scenario})
        set_tests_properties(installed.accounting.journal.${scenario} PROPERTIES TIMEOUT 30)
    endforeach()
endif()
