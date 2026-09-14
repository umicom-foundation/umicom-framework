#-----------------------------------------------------------------------------
# Umicom Framework
# File: tools/source_contracts/CMakeLists.txt
# PURPOSE: Build a reusable native source-contract library, command and regressions.
# Author: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomSourceContracts LANGUAGES C)
    include(CTest)
endif()
include(GNUInstallDirs)
get_filename_component(_umicom_contract_root "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

add_library(umicom_source_contracts STATIC
    "${_umicom_contract_root}/src/source_contracts/text.c"
    "${_umicom_contract_root}/src/source_contracts/metadata.c"
    "${_umicom_contract_root}/src/source_contracts/files.c"
    "${_umicom_contract_root}/src/source_contracts/audit.c"
    "${_umicom_contract_root}/src/source_contracts/identity.c"
    "${_umicom_contract_root}/src/source_contracts/presets.c"
    "${_umicom_contract_root}/src/source_contracts/workbench.c"
    "${_umicom_contract_root}/src/source_contracts/portfolio.c"
    # Compile the existing dependency-free Framework reader in this small tool
    # closure; do not introduce or copy another JSON implementation.
    "${_umicom_contract_root}/src/language_runtime/json.c")
add_library(Umicom::source_contracts ALIAS umicom_source_contracts)
set_target_properties(umicom_source_contracts PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    EXPORT_NAME source_contracts)
target_include_directories(umicom_source_contracts PUBLIC
    "$<BUILD_INTERFACE:${_umicom_contract_root}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

add_executable(umicom-source-contracts main.c)
target_link_libraries(umicom-source-contracts PRIVATE Umicom::source_contracts)
set_target_properties(umicom-source-contracts PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

if(BUILD_TESTING)
    add_executable(umicom-source-contract-tests
        "${_umicom_contract_root}/tests/source_contracts/test_source_contracts.c")
    target_link_libraries(umicom-source-contract-tests PRIVATE Umicom::source_contracts)
    target_include_directories(umicom-source-contract-tests PRIVATE
        "${_umicom_contract_root}/src/source_contracts")
    set_target_properties(umicom-source-contract-tests PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    foreach(_case metadata lexer identity presets filesystem portfolio rules)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fixtures/${_case}")
        add_test(NAME "framework.source_contracts.${_case}"
            COMMAND umicom-source-contract-tests "${_case}"
                "${CMAKE_CURRENT_BINARY_DIR}/fixtures/${_case}")
        set_tests_properties("framework.source_contracts.${_case}" PROPERTIES
            LABELS "framework;native;source-contracts;regression")
    endforeach()
endif()

foreach(_target umicom_source_contracts umicom-source-contracts umicom-source-contract-tests)
    if(TARGET "${_target}")
        if(MSVC)
            target_compile_options("${_target}" PRIVATE /W4)
        else()
            target_compile_options("${_target}" PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${_target}")
        endif()
    endif()
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-source-contracts)
    if(TARGET umicom-source-contract-tests)
        umicom_register_validation_target(umicom-source-contract-tests)
    endif()
endif()

# Installation adds one read-only development command; no source validation
# runs while installing, and no application is started as an install hook.
install(TARGETS umicom_source_contracts umicom-source-contracts
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT Framework)
install(FILES "${_umicom_contract_root}/include/umicom/source_contracts/source_contracts.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/source_contracts" COMPONENT Framework)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    install(EXPORT UmicomFrameworkTargets NAMESPACE Umicom::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomSourceContracts" COMPONENT Framework)
endif()
