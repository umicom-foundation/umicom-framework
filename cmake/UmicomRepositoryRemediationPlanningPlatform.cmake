#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomRepositoryRemediationPlanningPlatform.cmake
#
# PURPOSE:
#   Framework plan-only repository remediation enabled
#
# ARCHITECTURE:
#   Extend the established umicom_repository target; no competing repository
#   library, VCS subsystem or maintenance model is created.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_FRAMEWORK_COMPLETION_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
if(NOT TARGET umicom_repository)
    message(FATAL_ERROR "UmicomRepositoryRemediationPlanningPlatform.cmake requires umicom_repository.")
endif()

target_sources(umicom_repository PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_action.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_action_set.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_attributes.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_branch.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_builder.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_gitlink.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_guard.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_ignore.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_index.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_remote.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_report.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_revision.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_safety.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_submodule.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_text.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_upstream.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/remediation/remediation_worktree.c"
)

if(BUILD_TESTING)
    if(NOT COMMAND umicom_add_framework_completion_test)
        function(umicom_add_framework_completion_test target test_name source)
            if(TARGET "${target}")
                return()
            endif()
            add_executable("${target}" "${UMICOM_FRAMEWORK_COMPLETION_ROOT}/${source}")
            target_link_libraries("${target}" PRIVATE Umicom::Framework)
            if(COMMAND umicom_apply_warnings)
                umicom_apply_warnings("${target}")
            endif()
            if(COMMAND umicom_apply_sanitizers)
                umicom_apply_sanitizers("${target}")
            endif()
            add_test(NAME "${test_name}" COMMAND "${target}")
        endfunction()
    endif()

umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-action-test
    framework.repository_remediation.remediation.action
    tests/repository_remediation/test_remediation_action.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-action-set-test
    framework.repository_remediation.remediation.action.set
    tests/repository_remediation/test_remediation_action_set.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-attributes-test
    framework.repository_remediation.remediation.attributes
    tests/repository_remediation/test_remediation_attributes.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-branch-test
    framework.repository_remediation.remediation.branch
    tests/repository_remediation/test_remediation_branch.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-builder-test
    framework.repository_remediation.remediation.builder
    tests/repository_remediation/test_remediation_builder.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-gitlink-test
    framework.repository_remediation.remediation.gitlink
    tests/repository_remediation/test_remediation_gitlink.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-guard-test
    framework.repository_remediation.remediation.guard
    tests/repository_remediation/test_remediation_guard.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-ignore-test
    framework.repository_remediation.remediation.ignore
    tests/repository_remediation/test_remediation_ignore.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-index-test
    framework.repository_remediation.remediation.index
    tests/repository_remediation/test_remediation_index.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-plan-test
    framework.repository_remediation.remediation.plan
    tests/repository_remediation/test_remediation_plan.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-policy-test
    framework.repository_remediation.remediation.policy
    tests/repository_remediation/test_remediation_policy.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-remote-test
    framework.repository_remediation.remediation.remote
    tests/repository_remediation/test_remediation_remote.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-report-test
    framework.repository_remediation.remediation.report
    tests/repository_remediation/test_remediation_report.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-revision-test
    framework.repository_remediation.remediation.revision
    tests/repository_remediation/test_remediation_revision.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-safety-test
    framework.repository_remediation.remediation.safety
    tests/repository_remediation/test_remediation_safety.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-service-test
    framework.repository_remediation.remediation.service
    tests/repository_remediation/test_remediation_service.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-submodule-test
    framework.repository_remediation.remediation.submodule
    tests/repository_remediation/test_remediation_submodule.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-text-test
    framework.repository_remediation.remediation.text
    tests/repository_remediation/test_remediation_text.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-types-test
    framework.repository_remediation.remediation.types
    tests/repository_remediation/test_remediation_types.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-upstream-test
    framework.repository_remediation.remediation.upstream
    tests/repository_remediation/test_remediation_upstream.c)
umicom_add_framework_completion_test(
    umicom-repository-remediation-remediation-worktree-test
    framework.repository_remediation.remediation.worktree
    tests/repository_remediation/test_remediation_worktree.c)
endif()

message(STATUS "Framework plan-only repository remediation enabled")
