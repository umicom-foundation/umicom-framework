#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomRepositoryControlPlatform.cmake
#
# PURPOSE:
#   Extend the established Umicom::repository target with the native repository
#   control plane: submodule inventories, safe lock planning, dependency graphs,
#   transactions, health, audit and Framework controller/service contracts.
#
# ARCHITECTURE:
#   No competing repository library is created. The existing repository target
#   remains authoritative, while applications consume the capability through
#   Umicom::Framework. The native `umicom repo` CLI delegates into Framework.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_REPOSITORY_CONTROL_FRAMEWORK_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/..")

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_repository)
    message(FATAL_ERROR
        "UmicomRepositoryControlPlatform.cmake requires umicom_repository.")
endif()

target_sources(umicom_repository PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/control_types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/sha.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/ref.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/path.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/submodule.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/inventory.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/gitmodules.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/gitlink.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/head_set.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/lock_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/lock_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/health.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/issue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/snapshot.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/dependency.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/dependency_graph.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/transaction.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/event.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/audit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/controller.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/control/service.c"
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add repository control test build helper so parent and application projects
    # apply one consistent rule.
    function(umicom_add_repository_control_test target test_name source)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()

        add_executable(
            "${target}"
            "${UMICOM_REPOSITORY_CONTROL_FRAMEWORK_ROOT}/${source}"
        )
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

    umicom_add_repository_control_test(
        umicom-repository-control-types-contract-test
        framework.repository_control.control_types.contract
        tests/repository_control/test_control_types_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-control-types-invalid-test
        framework.repository_control.control_types.invalid
        tests/repository_control/test_control_types_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-control-types-boundary-test
        framework.repository_control.control_types.boundary
        tests/repository_control/test_control_types_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-sha-contract-test
        framework.repository_control.sha.contract
        tests/repository_control/test_sha_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-sha-invalid-test
        framework.repository_control.sha.invalid
        tests/repository_control/test_sha_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-sha-boundary-test
        framework.repository_control.sha.boundary
        tests/repository_control/test_sha_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-ref-contract-test
        framework.repository_control.ref.contract
        tests/repository_control/test_ref_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-ref-invalid-test
        framework.repository_control.ref.invalid
        tests/repository_control/test_ref_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-ref-boundary-test
        framework.repository_control.ref.boundary
        tests/repository_control/test_ref_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-path-contract-test
        framework.repository_control.path.contract
        tests/repository_control/test_path_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-path-invalid-test
        framework.repository_control.path.invalid
        tests/repository_control/test_path_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-path-boundary-test
        framework.repository_control.path.boundary
        tests/repository_control/test_path_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-submodule-contract-test
        framework.repository_control.submodule.contract
        tests/repository_control/test_submodule_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-submodule-invalid-test
        framework.repository_control.submodule.invalid
        tests/repository_control/test_submodule_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-submodule-boundary-test
        framework.repository_control.submodule.boundary
        tests/repository_control/test_submodule_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-inventory-contract-test
        framework.repository_control.inventory.contract
        tests/repository_control/test_inventory_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-inventory-invalid-test
        framework.repository_control.inventory.invalid
        tests/repository_control/test_inventory_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-inventory-boundary-test
        framework.repository_control.inventory.boundary
        tests/repository_control/test_inventory_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-gitmodules-contract-test
        framework.repository_control.gitmodules.contract
        tests/repository_control/test_gitmodules_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-gitmodules-invalid-test
        framework.repository_control.gitmodules.invalid
        tests/repository_control/test_gitmodules_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-gitmodules-boundary-test
        framework.repository_control.gitmodules.boundary
        tests/repository_control/test_gitmodules_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-gitlink-contract-test
        framework.repository_control.gitlink.contract
        tests/repository_control/test_gitlink_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-gitlink-invalid-test
        framework.repository_control.gitlink.invalid
        tests/repository_control/test_gitlink_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-gitlink-boundary-test
        framework.repository_control.gitlink.boundary
        tests/repository_control/test_gitlink_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-head-set-contract-test
        framework.repository_control.head_set.contract
        tests/repository_control/test_head_set_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-head-set-invalid-test
        framework.repository_control.head_set.invalid
        tests/repository_control/test_head_set_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-head-set-boundary-test
        framework.repository_control.head_set.boundary
        tests/repository_control/test_head_set_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-lock-policy-contract-test
        framework.repository_control.lock_policy.contract
        tests/repository_control/test_lock_policy_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-lock-policy-invalid-test
        framework.repository_control.lock_policy.invalid
        tests/repository_control/test_lock_policy_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-lock-policy-boundary-test
        framework.repository_control.lock_policy.boundary
        tests/repository_control/test_lock_policy_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-lock-plan-contract-test
        framework.repository_control.lock_plan.contract
        tests/repository_control/test_lock_plan_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-lock-plan-invalid-test
        framework.repository_control.lock_plan.invalid
        tests/repository_control/test_lock_plan_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-lock-plan-boundary-test
        framework.repository_control.lock_plan.boundary
        tests/repository_control/test_lock_plan_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-health-contract-test
        framework.repository_control.health.contract
        tests/repository_control/test_health_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-health-invalid-test
        framework.repository_control.health.invalid
        tests/repository_control/test_health_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-health-boundary-test
        framework.repository_control.health.boundary
        tests/repository_control/test_health_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-issue-contract-test
        framework.repository_control.issue.contract
        tests/repository_control/test_issue_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-issue-invalid-test
        framework.repository_control.issue.invalid
        tests/repository_control/test_issue_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-issue-boundary-test
        framework.repository_control.issue.boundary
        tests/repository_control/test_issue_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-snapshot-contract-test
        framework.repository_control.snapshot.contract
        tests/repository_control/test_snapshot_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-snapshot-invalid-test
        framework.repository_control.snapshot.invalid
        tests/repository_control/test_snapshot_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-snapshot-boundary-test
        framework.repository_control.snapshot.boundary
        tests/repository_control/test_snapshot_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-dependency-contract-test
        framework.repository_control.dependency.contract
        tests/repository_control/test_dependency_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-dependency-invalid-test
        framework.repository_control.dependency.invalid
        tests/repository_control/test_dependency_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-dependency-boundary-test
        framework.repository_control.dependency.boundary
        tests/repository_control/test_dependency_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-dependency-graph-contract-test
        framework.repository_control.dependency_graph.contract
        tests/repository_control/test_dependency_graph_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-dependency-graph-invalid-test
        framework.repository_control.dependency_graph.invalid
        tests/repository_control/test_dependency_graph_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-dependency-graph-boundary-test
        framework.repository_control.dependency_graph.boundary
        tests/repository_control/test_dependency_graph_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-transaction-contract-test
        framework.repository_control.transaction.contract
        tests/repository_control/test_transaction_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-transaction-invalid-test
        framework.repository_control.transaction.invalid
        tests/repository_control/test_transaction_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-transaction-boundary-test
        framework.repository_control.transaction.boundary
        tests/repository_control/test_transaction_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-event-contract-test
        framework.repository_control.event.contract
        tests/repository_control/test_event_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-event-invalid-test
        framework.repository_control.event.invalid
        tests/repository_control/test_event_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-event-boundary-test
        framework.repository_control.event.boundary
        tests/repository_control/test_event_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-audit-contract-test
        framework.repository_control.audit.contract
        tests/repository_control/test_audit_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-audit-invalid-test
        framework.repository_control.audit.invalid
        tests/repository_control/test_audit_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-audit-boundary-test
        framework.repository_control.audit.boundary
        tests/repository_control/test_audit_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-controller-contract-test
        framework.repository_control.controller.contract
        tests/repository_control/test_controller_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-controller-invalid-test
        framework.repository_control.controller.invalid
        tests/repository_control/test_controller_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-controller-boundary-test
        framework.repository_control.controller.boundary
        tests/repository_control/test_controller_boundary.c
    )
    umicom_add_repository_control_test(
        umicom-repository-service-contract-test
        framework.repository_control.service.contract
        tests/repository_control/test_service_contract.c
    )
    umicom_add_repository_control_test(
        umicom-repository-service-invalid-test
        framework.repository_control.service.invalid
        tests/repository_control/test_service_invalid.c
    )
    umicom_add_repository_control_test(
        umicom-repository-service-boundary-test
        framework.repository_control.service.boundary
        tests/repository_control/test_service_boundary.c
    )

    # Configure the optional target only when its feature has created it.
    if(TARGET umicom)
        add_test(
            NAME framework.native_cli.repo_help
            COMMAND umicom repo help
        )
    endif()
endif()

message(STATUS
    "Umicom native repository control plane and regression suite enabled")
