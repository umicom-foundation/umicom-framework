#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/cmake/test_build_contract/fixture/CMakeLists.txt
# PURPOSE: Construct deliberately valid and invalid CMake consumer graphs.
# AUTHOR: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomTestBuildContractFixture LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
if(NOT DEFINED FRAMEWORK_ROOT OR NOT DEFINED CONTRACT_CASE)
    message(FATAL_ERROR "FRAMEWORK_ROOT and CONTRACT_CASE are required.")
endif()
set(_valid_cases valid alias-target concrete-base missing-link missing-header
    missing-sdk missing-target non-executable bypass-guard)
if(NOT CONTRACT_CASE IN_LIST _valid_cases)
    message(FATAL_ERROR "Unknown contract fixture: ${CONTRACT_CASE}")
endif()

# The fixture models ONLY base's public include usage requirement. It does
# not compile or pretend to validate Framework's production base library.
if(NOT CONTRACT_CASE STREQUAL "missing-sdk")
    add_library(fixture_base INTERFACE)
    add_library(Umicom::base ALIAS fixture_base)
    target_include_directories(fixture_base INTERFACE "${FRAMEWORK_ROOT}/include")
endif()
if(CONTRACT_CASE STREQUAL "non-executable")
    add_library(consumer INTERFACE)
elseif(NOT CONTRACT_CASE STREQUAL "missing-target")
    add_executable(consumer "${CMAKE_CURRENT_LIST_DIR}/../test_check_header.c")
    if(MSVC)
        target_compile_options(consumer PRIVATE /W4 /WX)
    else()
        target_compile_options(consumer PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    if(CONTRACT_CASE STREQUAL "concrete-base")
        target_link_libraries(consumer PRIVATE fixture_base)
    elseif(NOT CONTRACT_CASE MATCHES "^(missing-link|missing-sdk|bypass-guard)$")
        target_link_libraries(consumer PRIVATE Umicom::base)
    endif()
endif()

set(_contract_module "${FRAMEWORK_ROOT}/cmake/UmicomTestBuildContract.cmake")
if(CONTRACT_CASE STREQUAL "missing-header")
    # Copy only the contract module into an isolated SDK with no header.
    # Never rename, delete or modify the caller's actual Framework checkout.
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/empty-sdk/cmake")
    file(COPY "${_contract_module}"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/empty-sdk/cmake")
    set(_contract_module
        "${CMAKE_CURRENT_BINARY_DIR}/empty-sdk/cmake/UmicomTestBuildContract.cmake")
endif()
include("${_contract_module}")
if(CONTRACT_CASE STREQUAL "alias-target")
    add_executable(consumer_alias ALIAS consumer)
    umicom_assert_test_build_contract(consumer_alias)
elseif(NOT CONTRACT_CASE STREQUAL "bypass-guard")
    umicom_assert_test_build_contract(consumer)
endif()
if(CONTRACT_CASE MATCHES "^(valid|alias-target|concrete-base)$")
    enable_testing()
    add_test(NAME fixture.header COMMAND consumer)
endif()
