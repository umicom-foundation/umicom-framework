#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomRepositoryInspectionRuntimePlatform.cmake
#
# PURPOSE:
#   Framework repository inspection runtime enabled
#
# ARCHITECTURE:
#   Extend the established umicom_repository target; no competing repository
#   library, VCS subsystem or maintenance model is created.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_FRAMEWORK_COMPLETION_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_repository)
    message(FATAL_ERROR "UmicomRepositoryInspectionRuntimePlatform.cmake requires umicom_repository.")
endif()

target_sources(umicom_repository PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/attributes_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/branch_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/divergence_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/fetch_state_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/git_command.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/git_output.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/gitlink_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/gitmodules_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/head_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/ignore_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/index_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/inspection_audit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/inspection_graph.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/inspection_health.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/inspection_issue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/inspection_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/inspection_report.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/inspection_revision.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/inspection_service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/inspection_text.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/inspection_types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/nested_repository_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/porcelain_parser.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/remote_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/repository_identity.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/repository_snapshot.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/status_record.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/status_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/submodule_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/tag_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/upstream_probe.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/inspection/worktree_probe.c"
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the local fallback only when the parent build did not provide the shared helper.
    if(NOT COMMAND umicom_add_framework_completion_test)
        # Define the add framework completion test build helper so parent and application projects
        # apply one consistent rule.
        function(umicom_add_framework_completion_test target test_name source)
            # Configure the optional target only when its feature has created it.
            if(TARGET "${target}")
                return()
            endif()
            add_executable("${target}" "${UMICOM_FRAMEWORK_COMPLETION_ROOT}/${source}")
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
    endif()

umicom_add_framework_completion_test(
    umicom-repository-inspection-attributes-probe-test
    framework.repository_inspection.attributes.probe
    tests/repository_inspection/test_attributes_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-branch-probe-test
    framework.repository_inspection.branch.probe
    tests/repository_inspection/test_branch_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-divergence-probe-test
    framework.repository_inspection.divergence.probe
    tests/repository_inspection/test_divergence_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-fetch-state-probe-test
    framework.repository_inspection.fetch.state.probe
    tests/repository_inspection/test_fetch_state_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-git-command-test
    framework.repository_inspection.git.command
    tests/repository_inspection/test_git_command.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-git-output-test
    framework.repository_inspection.git.output
    tests/repository_inspection/test_git_output.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-gitlink-probe-test
    framework.repository_inspection.gitlink.probe
    tests/repository_inspection/test_gitlink_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-gitmodules-probe-test
    framework.repository_inspection.gitmodules.probe
    tests/repository_inspection/test_gitmodules_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-head-probe-test
    framework.repository_inspection.head.probe
    tests/repository_inspection/test_head_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-ignore-probe-test
    framework.repository_inspection.ignore.probe
    tests/repository_inspection/test_ignore_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-index-probe-test
    framework.repository_inspection.index.probe
    tests/repository_inspection/test_index_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-inspection-audit-test
    framework.repository_inspection.inspection.audit
    tests/repository_inspection/test_inspection_audit.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-inspection-graph-test
    framework.repository_inspection.inspection.graph
    tests/repository_inspection/test_inspection_graph.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-inspection-health-test
    framework.repository_inspection.inspection.health
    tests/repository_inspection/test_inspection_health.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-inspection-issue-test
    framework.repository_inspection.inspection.issue
    tests/repository_inspection/test_inspection_issue.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-inspection-policy-test
    framework.repository_inspection.inspection.policy
    tests/repository_inspection/test_inspection_policy.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-inspection-report-test
    framework.repository_inspection.inspection.report
    tests/repository_inspection/test_inspection_report.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-inspection-revision-test
    framework.repository_inspection.inspection.revision
    tests/repository_inspection/test_inspection_revision.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-inspection-service-test
    framework.repository_inspection.inspection.service
    tests/repository_inspection/test_inspection_service.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-inspection-text-test
    framework.repository_inspection.inspection.text
    tests/repository_inspection/test_inspection_text.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-inspection-types-test
    framework.repository_inspection.inspection.types
    tests/repository_inspection/test_inspection_types.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-nested-repository-probe-test
    framework.repository_inspection.nested.repository.probe
    tests/repository_inspection/test_nested_repository_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-porcelain-parser-test
    framework.repository_inspection.porcelain.parser
    tests/repository_inspection/test_porcelain_parser.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-remote-probe-test
    framework.repository_inspection.remote.probe
    tests/repository_inspection/test_remote_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-repository-identity-test
    framework.repository_inspection.repository.identity
    tests/repository_inspection/test_repository_identity.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-repository-snapshot-test
    framework.repository_inspection.repository.snapshot
    tests/repository_inspection/test_repository_snapshot.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-status-record-test
    framework.repository_inspection.status.record
    tests/repository_inspection/test_status_record.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-status-probe-test
    framework.repository_inspection.status.probe
    tests/repository_inspection/test_status_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-submodule-probe-test
    framework.repository_inspection.submodule.probe
    tests/repository_inspection/test_submodule_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-tag-probe-test
    framework.repository_inspection.tag.probe
    tests/repository_inspection/test_tag_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-upstream-probe-test
    framework.repository_inspection.upstream.probe
    tests/repository_inspection/test_upstream_probe.c)
umicom_add_framework_completion_test(
    umicom-repository-inspection-worktree-probe-test
    framework.repository_inspection.worktree.probe
    tests/repository_inspection/test_worktree_probe.c)
endif()

message(STATUS "Framework repository inspection runtime enabled")
