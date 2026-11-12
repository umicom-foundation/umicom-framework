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
    "${_umicom_contract_root}/src/source_contracts/literals.c"
    "${_umicom_contract_root}/src/source_contracts/metadata.c"
    "${_umicom_contract_root}/src/source_contracts/files.c"
    "${_umicom_contract_root}/src/source_contracts/audit.c"
    "${_umicom_contract_root}/src/source_contracts/identity.c"
    "${_umicom_contract_root}/src/source_contracts/guide_branding.c"
    "${_umicom_contract_root}/src/source_contracts/guide_files.c"
    "${_umicom_contract_root}/src/source_contracts/presets.c"
    "${_umicom_contract_root}/src/source_contracts/workbench.c"
    "${_umicom_contract_root}/src/source_contracts/portfolio.c"
    # Compile the existing dependency-free Framework reader in this small tool
    # closure; do not introduce or copy another JSON implementation.
    "${_umicom_contract_root}/src/language_runtime/json.c")
# Reuse Framework base in the composition. The small standalone tool compiles
# the same source files when no base target exists; it does not fork the codec.
if(TARGET Umicom::base)
    target_link_libraries(umicom_source_contracts PUBLIC Umicom::base)
else()
    target_sources(umicom_source_contracts PRIVATE
        "${_umicom_contract_root}/src/base/base64.c"
        "${_umicom_contract_root}/src/base/sha256.c")
endif()
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
    foreach(_case metadata lexer identity presets filesystem portfolio rules binary-source literal-source)
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

# Source contracts are exported to installed SDK consumers from this subdirectory.
# Register both artifacts here, beside their existing full-install ownership;
# a root-only library scan misses this archive and then its command executable.
include("${_umicom_contract_root}/cmake/UmicomSdkComponent.cmake")
umicom_register_sdk_libraries(umicom_source_contracts)
umicom_register_sdk_tools(umicom-source-contracts)
install(FILES "${_umicom_contract_root}/include/umicom/source_contracts/source_contracts.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/source_contracts"
    COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Preserve the same standalone development export when selecting only SDK files.
    install(EXPORT UmicomFrameworkTargets NAMESPACE Umicom::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomSourceContracts"
        COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    umicom_generate_sdk_artifact_inventory()
endif()

# Literal admission is independent of a full product build and catches source
# text corruption before attribution or include-guard checks can obscure it.
if(BUILD_TESTING)
    add_executable(umicom-source-literals-test
        "${_umicom_contract_root}/tests/source_contracts/test_literals.c")
    target_link_libraries(umicom-source-literals-test PRIVATE Umicom::source_contracts)
    target_include_directories(umicom-source-literals-test PRIVATE
        "${_umicom_contract_root}/src/source_contracts")
    set_target_properties(umicom-source-literals-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(MSVC)
        target_compile_options(umicom-source-literals-test PRIVATE /W4)
    else()
        target_compile_options(umicom-source-literals-test PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-source-literals-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-source-literals-test)
    endif()
    foreach(_literal_case escaped splice-lf splice-crlf numeric-separators comments adjacent
        raw-string raw-character raw-crlf raw-cr unterminated-string unterminated-character
        unterminated-comment disabled-malformed physical-location invalid-api)
        add_test(NAME framework.source_contracts.literal.${_literal_case}
            COMMAND umicom-source-literals-test ${_literal_case})
        set_tests_properties(framework.source_contracts.literal.${_literal_case} PROPERTIES
            TIMEOUT 20 LABELS "framework;native;source-contracts;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.source_contracts.literal.${_literal_case}
                TARGETS umicom-source-literals-test)
        endif()
    endforeach()
    unset(_literal_case)
endif()

# Native guide checks inspect source bytes without running a renderer or app.
if(BUILD_TESTING)
    add_executable(umicom-source-guide-test
        "${_umicom_contract_root}/tests/source_contracts/test_guide_branding.c")
    add_executable(umicom-source-guide-files-test
        "${_umicom_contract_root}/tests/source_contracts/test_guide_files.c")
    foreach(_target IN ITEMS umicom-source-guide-test umicom-source-guide-files-test)
        target_link_libraries("${_target}" PRIVATE Umicom::source_contracts)
        target_include_directories("${_target}" PRIVATE "${_umicom_contract_root}/src/source_contracts")
        set_target_properties("${_target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(MSVC)
            target_compile_options("${_target}" PRIVATE /W4)
        else()
            target_compile_options("${_target}" PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${_target}")
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${_target}")
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs("${_target}" FILES
                "${_umicom_contract_root}/tests/source_contracts/guide_branding_fixture.h")
        endif()
    endforeach()
    foreach(_case IN ITEMS dark light symbol comment template nested-template script textarea quoted-delimiter single-quotes uppercase missing-meta wrong-meta duplicate-meta missing-logo wrong-hash wrong-bytes short-asset long-asset external-asset raster-asset missing-alt unowned-image missing-icon wrong-icon-type icon-bytes missing-fonts font-comment font-role unknown-surface only-comment only-template unclosed-template unmatched-template unclosed-comment unclosed-style duplicate-attribute unclosed-quote empty-asset oversized-asset null-inputs embedded-nul)
        add_test(NAME "framework.source_contracts.guide.${_case}" COMMAND umicom-source-guide-test "${_case}")
        set_tests_properties("framework.source_contracts.guide.${_case}" PROPERTIES
            TIMEOUT 10 LABELS "framework;source-contracts;branding;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.source_contracts.guide.${_case}" TARGETS umicom-source-guide-test)
        endif()
    endforeach()
    foreach(_case IN ITEMS valid duplicates empty missing-root missing-asset empty-asset binary-html altered-asset unrelated-file nested read-bytes ignored-directory)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/guide-fixtures/${_case}")
        add_test(NAME "framework.source_contracts.guide_files.${_case}"
            COMMAND umicom-source-guide-files-test "${_case}" "${CMAKE_CURRENT_BINARY_DIR}/guide-fixtures/${_case}")
        set_tests_properties("framework.source_contracts.guide_files.${_case}" PROPERTIES
            TIMEOUT 10 LABELS "framework;source-contracts;branding;filesystem;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.source_contracts.guide_files.${_case}" TARGETS umicom-source-guide-files-test)
        endif()
    endforeach()
    # Audit the maintained source tree as well as controlled failure fixtures.
    add_test(NAME framework.source_contracts.learning_branding
        COMMAND umicom-source-contracts guides --assets "${_umicom_contract_root}/resources/brand"
            --root "${_umicom_contract_root}/docs/learning")
    set_tests_properties(framework.source_contracts.learning_branding PROPERTIES
        TIMEOUT 60 LABELS "framework;source-contracts;branding;learning")
    # Source trees are mutable observations, not closed executable-only inputs.
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.source_contracts.learning_branding TARGETS umicom-source-contracts ALWAYS)
    endif()
endif()
