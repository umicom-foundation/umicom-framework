#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/cmake/test_build_contract/CMakeLists.txt
#
# PURPOSE:
#   Run small diagnostic-header/target-contract regressions standalone or
#   through the complete application suite, without a parallel test runtime.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
get_filename_component(_umicom_contract_root "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomTestBuildContract LANGUAGES C)
    set(CMAKE_C_STANDARD 23)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS OFF)
    include(CTest)
    if(NOT BUILD_TESTING)
        message(FATAL_ERROR "This standalone regression project requires BUILD_TESTING=ON.")
    endif()
    # Standalone fixtures exercise header propagation only. Integrated builds
    # below consume the real Umicom::base target supplied by Framework.
    add_library(umicom_contract_fixture_base INTERFACE)
    add_library(Umicom::base ALIAS umicom_contract_fixture_base)
    target_include_directories(umicom_contract_fixture_base INTERFACE
        "${_umicom_contract_root}/include")
    message(STATUS "Standalone header-contract fixtures; not a complete Framework build.")
endif()
include("${_umicom_contract_root}/cmake/UmicomTestBuildContract.cmake")

foreach(_variant IN ITEMS debug release)
    if(_variant STREQUAL "release")
        set(_target umicom-test-header-release-contract-test)
    else()
        set(_target umicom-test-header-contract-test)
    endif()
    add_executable(${_target} test_check_header.c)
    target_link_libraries(${_target} PRIVATE Umicom::base)
    umicom_assert_test_build_contract(${_target})
    if(_variant STREQUAL "release")
        target_compile_definitions(${_target} PRIVATE NDEBUG)
    endif()
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${_target})
    elseif(MSVC)
        target_compile_options(${_target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${_target} PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${_target})
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${_target})
    endif()
    if(NOT CMAKE_CROSSCOMPILING OR CMAKE_CROSSCOMPILING_EMULATOR)
        add_test(NAME framework.build_contract.header.${_variant} COMMAND ${_target})
        set_tests_properties(framework.build_contract.header.${_variant}
            PROPERTIES LABELS "framework;build-contract;header;regression" TIMEOUT 30)
    endif()
    if(NOT CMAKE_CROSSCOMPILING)
        add_test(NAME framework.build_contract.diagnostic.${_variant}
            COMMAND "${CMAKE_COMMAND}"
                "-DTEST_EXECUTABLE=$<TARGET_FILE:${_target}>"
                -P "${CMAKE_CURRENT_LIST_DIR}/verify_diagnostic.cmake")
        set_tests_properties(framework.build_contract.diagnostic.${_variant}
            PROPERTIES LABELS "framework;build-contract;diagnostic;regression" TIMEOUT 30)
    endif()
endforeach()

# These fixtures require native execution and intentionally do not masquerade
# as cross-compiled target validation without a qualified emulator/toolchain.
if(NOT CMAKE_CROSSCOMPILING)
    foreach(_case IN ITEMS valid alias-target concrete-base missing-link
            missing-header missing-sdk missing-target non-executable bypass-guard)
        add_test(NAME framework.build_contract.graph.${_case}
            COMMAND "${CMAKE_COMMAND}"
                "-DFRAMEWORK_ROOT=${_umicom_contract_root}"
                "-DFIXTURE_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}/fixtures/$<CONFIG>/${_case}"
                "-DCONTRACT_CASE=${_case}"
                "-DTEST_COMPILER=${CMAKE_C_COMPILER}"
                "-DTEST_GENERATOR=${CMAKE_GENERATOR}"
                "-DTEST_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
                "-DTEST_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
                "-DTEST_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
                "-DTEST_CONFIG=$<CONFIG>"
                -P "${CMAKE_CURRENT_LIST_DIR}/run_fixture.cmake")
        set_tests_properties(framework.build_contract.graph.${_case}
            PROPERTIES LABELS "framework;build-contract;cmake;regression" TIMEOUT 240)
    endforeach()
else()
    message(STATUS "Native build-contract configure/diagnostic fixtures omitted while cross-compiling.")
endif()
