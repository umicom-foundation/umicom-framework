#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomToolchainKitPlatform.cmake
#
# PURPOSE:
#   Complete the Framework-owned native development-kit control plane with
#   project/application kit selection, cross-target toolchains, SDK/sysroot
#   composition, CPython discovery, provenance, health and machine output.
#
# ARCHITECTURE:
#   Extend the established umicom_toolchain target. Do not create a competing
#   compiler, SDK, environment or application-owned discovery subsystem.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_TOOLCHAIN_KIT_FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
if(NOT TARGET umicom_toolchain)
    message(FATAL_ERROR "UmicomToolchainKitPlatform.cmake requires umicom_toolchain.")
endif()

target_sources(umicom_toolchain PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/cross_toolchain.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/python_runtime.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_discovery.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_selection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_health.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_provenance.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_environment.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_cmake.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/kit/kit_json.c"
)

if(BUILD_TESTING)
    function(umicom_add_toolchain_kit_test target test_name source)
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${UMICOM_TOOLCHAIN_KIT_FRAMEWORK_ROOT}/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::toolchain)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES LABELS "framework;toolchain-kit")
    endfunction()

    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-contract-test
        framework.toolchain_kit.contract
        tests/toolchain_kit/test_kit.c)
    umicom_add_toolchain_kit_test(
        umicom-cross-toolchain-plan-test
        framework.toolchain_kit.cross_toolchain
        tests/toolchain_kit/test_cross_toolchain.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-catalogue-test
        framework.toolchain_kit.catalogue
        tests/toolchain_kit/test_kit_catalogue.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-discovery-test
        framework.toolchain_kit.discovery
        tests/toolchain_kit/test_kit_discovery.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-selection-test
        framework.toolchain_kit.selection
        tests/toolchain_kit/test_kit_selection.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-health-test
        framework.toolchain_kit.health
        tests/toolchain_kit/test_kit_health.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-provenance-test
        framework.toolchain_kit.provenance
        tests/toolchain_kit/test_kit_provenance.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-environment-test
        framework.toolchain_kit.environment
        tests/toolchain_kit/test_kit_environment.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-cmake-test
        framework.toolchain_kit.cmake
        tests/toolchain_kit/test_kit_cmake.c)
    umicom_add_toolchain_kit_test(
        umicom-toolchain-kit-json-test
        framework.toolchain_kit.json
        tests/toolchain_kit/test_kit_json.c)
    umicom_add_toolchain_kit_test(
        umicom-python-runtime-contract-test
        framework.toolchain_kit.python_runtime
        tests/toolchain_kit/test_python_runtime.c)
endif()

message(STATUS "Umicom developer kits, cross-target toolchains and provenance enabled")
