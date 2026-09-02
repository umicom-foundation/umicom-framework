#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomRepositoryMaintenancePlatform.cmake
#
# PURPOSE:
#   Extend the existing repository target with toolkit-neutral maintenance and
#   doctor state models. No competing repository or VCS subsystem is created.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_REPOSITORY_MAINTENANCE_FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_repository)
    message(FATAL_ERROR "UmicomRepositoryMaintenancePlatform.cmake requires umicom_repository.")
endif()

target_sources(umicom_repository PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/maintenance/maintenance_types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/maintenance/branch_status.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/maintenance/worktree_status.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/maintenance/index_status.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/maintenance/submodule_status.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/maintenance/remote_status.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/maintenance/status_summary.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/maintenance/doctor_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/maintenance/doctor_issue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/maintenance/doctor_report.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/maintenance/doctor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/maintenance/maintenance_service.c"
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add repository maintenance test build helper so parent and application
    # projects apply one consistent rule.
    function(umicom_add_repository_maintenance_test target test_name source)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${UMICOM_REPOSITORY_MAINTENANCE_FRAMEWORK_ROOT}/${source}")
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
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-maintenance-types-contract-test
        framework.repository_maintenance.maintenance_types.contract
        tests/repository_maintenance/test_maintenance_types_contract.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-maintenance-types-invalid-test
        framework.repository_maintenance.maintenance_types.invalid
        tests/repository_maintenance/test_maintenance_types_invalid.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-branch-status-contract-test
        framework.repository_maintenance.branch_status.contract
        tests/repository_maintenance/test_branch_status_contract.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-branch-status-invalid-test
        framework.repository_maintenance.branch_status.invalid
        tests/repository_maintenance/test_branch_status_invalid.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-worktree-status-contract-test
        framework.repository_maintenance.worktree_status.contract
        tests/repository_maintenance/test_worktree_status_contract.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-worktree-status-invalid-test
        framework.repository_maintenance.worktree_status.invalid
        tests/repository_maintenance/test_worktree_status_invalid.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-index-status-contract-test
        framework.repository_maintenance.index_status.contract
        tests/repository_maintenance/test_index_status_contract.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-index-status-invalid-test
        framework.repository_maintenance.index_status.invalid
        tests/repository_maintenance/test_index_status_invalid.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-submodule-status-contract-test
        framework.repository_maintenance.submodule_status.contract
        tests/repository_maintenance/test_submodule_status_contract.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-submodule-status-invalid-test
        framework.repository_maintenance.submodule_status.invalid
        tests/repository_maintenance/test_submodule_status_invalid.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-remote-status-contract-test
        framework.repository_maintenance.remote_status.contract
        tests/repository_maintenance/test_remote_status_contract.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-remote-status-invalid-test
        framework.repository_maintenance.remote_status.invalid
        tests/repository_maintenance/test_remote_status_invalid.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-status-summary-contract-test
        framework.repository_maintenance.status_summary.contract
        tests/repository_maintenance/test_status_summary_contract.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-status-summary-invalid-test
        framework.repository_maintenance.status_summary.invalid
        tests/repository_maintenance/test_status_summary_invalid.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-doctor-policy-contract-test
        framework.repository_maintenance.doctor_policy.contract
        tests/repository_maintenance/test_doctor_policy_contract.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-doctor-policy-invalid-test
        framework.repository_maintenance.doctor_policy.invalid
        tests/repository_maintenance/test_doctor_policy_invalid.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-doctor-issue-contract-test
        framework.repository_maintenance.doctor_issue.contract
        tests/repository_maintenance/test_doctor_issue_contract.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-doctor-issue-invalid-test
        framework.repository_maintenance.doctor_issue.invalid
        tests/repository_maintenance/test_doctor_issue_invalid.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-doctor-report-contract-test
        framework.repository_maintenance.doctor_report.contract
        tests/repository_maintenance/test_doctor_report_contract.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-doctor-report-invalid-test
        framework.repository_maintenance.doctor_report.invalid
        tests/repository_maintenance/test_doctor_report_invalid.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-doctor-contract-test
        framework.repository_maintenance.doctor.contract
        tests/repository_maintenance/test_doctor_contract.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-doctor-invalid-test
        framework.repository_maintenance.doctor.invalid
        tests/repository_maintenance/test_doctor_invalid.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-maintenance-service-contract-test
        framework.repository_maintenance.maintenance_service.contract
        tests/repository_maintenance/test_maintenance_service_contract.c
    )
    umicom_add_repository_maintenance_test(
        umicom-repository-maintenance-maintenance-service-invalid-test
        framework.repository_maintenance.maintenance_service.invalid
        tests/repository_maintenance/test_maintenance_service_invalid.c
    )
endif()

message(STATUS "Umicom repository maintenance and doctor models enabled")
