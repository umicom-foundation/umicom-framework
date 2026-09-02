#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomToolchainOperationPlatform.cmake
#
# PURPOSE:
#   Add operation-scoped native tool discovery to the existing toolchain target.
#   Repository operations therefore require Git only and never depend on a C
#   compiler compile-link-run probe.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_TOOLCHAIN_OPERATION_FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_toolchain)
    message(FATAL_ERROR "UmicomToolchainOperationPlatform.cmake requires umicom_toolchain.")
endif()

target_sources(umicom_toolchain PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/operation/operation_types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/operation/tool_set.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/operation/requirement.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/operation/requirement_set.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/operation/operation_profile.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/operation/operation_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/operation/probe_report.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/operation/tool_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/operation/scoped_discovery.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/operation/scoped_environment.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/operation/operation_context.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/operation/operation_service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/operation/operation_health.c"
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add toolchain operation test build helper so parent and application projects
    # apply one consistent rule.
    function(umicom_add_toolchain_operation_test target test_name source)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${UMICOM_TOOLCHAIN_OPERATION_FRAMEWORK_ROOT}/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::Framework)
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
    endfunction()
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-operation-types-contract-test
        framework.toolchain_operation.operation_types.contract
        tests/toolchain_operation/test_operation_types_contract.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-operation-types-invalid-test
        framework.toolchain_operation.operation_types.invalid
        tests/toolchain_operation/test_operation_types_invalid.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-tool-set-contract-test
        framework.toolchain_operation.tool_set.contract
        tests/toolchain_operation/test_tool_set_contract.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-tool-set-invalid-test
        framework.toolchain_operation.tool_set.invalid
        tests/toolchain_operation/test_tool_set_invalid.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-requirement-contract-test
        framework.toolchain_operation.requirement.contract
        tests/toolchain_operation/test_requirement_contract.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-requirement-invalid-test
        framework.toolchain_operation.requirement.invalid
        tests/toolchain_operation/test_requirement_invalid.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-requirement-set-contract-test
        framework.toolchain_operation.requirement_set.contract
        tests/toolchain_operation/test_requirement_set_contract.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-requirement-set-invalid-test
        framework.toolchain_operation.requirement_set.invalid
        tests/toolchain_operation/test_requirement_set_invalid.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-operation-profile-contract-test
        framework.toolchain_operation.operation_profile.contract
        tests/toolchain_operation/test_operation_profile_contract.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-operation-profile-invalid-test
        framework.toolchain_operation.operation_profile.invalid
        tests/toolchain_operation/test_operation_profile_invalid.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-operation-catalogue-contract-test
        framework.toolchain_operation.operation_catalogue.contract
        tests/toolchain_operation/test_operation_catalogue_contract.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-operation-catalogue-invalid-test
        framework.toolchain_operation.operation_catalogue.invalid
        tests/toolchain_operation/test_operation_catalogue_invalid.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-probe-report-contract-test
        framework.toolchain_operation.probe_report.contract
        tests/toolchain_operation/test_probe_report_contract.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-probe-report-invalid-test
        framework.toolchain_operation.probe_report.invalid
        tests/toolchain_operation/test_probe_report_invalid.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-tool-probe-contract-test
        framework.toolchain_operation.tool_probe.contract
        tests/toolchain_operation/test_tool_probe_contract.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-tool-probe-invalid-test
        framework.toolchain_operation.tool_probe.invalid
        tests/toolchain_operation/test_tool_probe_invalid.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-scoped-discovery-contract-test
        framework.toolchain_operation.scoped_discovery.contract
        tests/toolchain_operation/test_scoped_discovery_contract.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-scoped-discovery-invalid-test
        framework.toolchain_operation.scoped_discovery.invalid
        tests/toolchain_operation/test_scoped_discovery_invalid.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-scoped-environment-contract-test
        framework.toolchain_operation.scoped_environment.contract
        tests/toolchain_operation/test_scoped_environment_contract.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-scoped-environment-invalid-test
        framework.toolchain_operation.scoped_environment.invalid
        tests/toolchain_operation/test_scoped_environment_invalid.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-operation-context-contract-test
        framework.toolchain_operation.operation_context.contract
        tests/toolchain_operation/test_operation_context_contract.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-operation-context-invalid-test
        framework.toolchain_operation.operation_context.invalid
        tests/toolchain_operation/test_operation_context_invalid.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-operation-service-contract-test
        framework.toolchain_operation.operation_service.contract
        tests/toolchain_operation/test_operation_service_contract.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-operation-service-invalid-test
        framework.toolchain_operation.operation_service.invalid
        tests/toolchain_operation/test_operation_service_invalid.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-operation-health-contract-test
        framework.toolchain_operation.operation_health.contract
        tests/toolchain_operation/test_operation_health_contract.c
    )
    umicom_add_toolchain_operation_test(
        umicom-toolchain-operation-operation-health-invalid-test
        framework.toolchain_operation.operation_health.invalid
        tests/toolchain_operation/test_operation_health_invalid.c
    )
endif()

message(STATUS "Umicom operation-scoped native tool discovery enabled")
