#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTestQualityWorkbenchPlatform.cmake
#
# PURPOSE:
#   Compose Test Explorer, diagnostics, coverage and quality presentation state
#   on the canonical Framework developer/test/diagnostics services.
#
# ARCHITECTURE:
#   Existing Test Runtime, diagnostics pipeline and observability services remain
#   authoritative. This layer owns toolkit-neutral production workbench state.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(NOT TARGET umicom_developer)
    message(FATAL_ERROR "UmicomTestQualityWorkbenchPlatform.cmake requires canonical umicom_developer")
endif()

target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/workbench_types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_item_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_suite_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_group_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_tree_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_tree_filter.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_tree_selection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_discovery_state.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_discovery_session.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_discovery_snapshot.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_run_request.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_run_profile.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_run_session.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_run_progress.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_run_result.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_result_item.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_result_group.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_result_history.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_result_snapshot.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_status_summary.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_duration_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_failure_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_failure_detail.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_output_entry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_output_stream.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_output_filter.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_output_session.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_retry_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_rerun_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_debug_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_parallelism_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_shard_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_environment_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_configuration_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_command_set.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_keymap_context.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_workspace_profile.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/diagnostic_item_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/diagnostic_group_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/diagnostic_filter_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/diagnostic_severity_summary.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/diagnostic_source_summary.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/diagnostic_snapshot.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/problem_navigation_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/problem_quick_fix_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/quality_gate_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/quality_gate_result.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/quality_budget_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/quality_score_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/coverage_file_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/coverage_region_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/coverage_line_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/coverage_branch_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/coverage_function_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/coverage_summary_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/coverage_filter_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/coverage_snapshot.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/coverage_delta_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/coverage_threshold_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/coverage_navigation_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/flaky_test_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/flaky_test_history.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_impact_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_impact_summary.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/quality_workbench_snapshot.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/test/workbench/test_quality_workbench_service.c"
)

if(BUILD_TESTING)
    function(umicom_add_test_quality_workbench_test target test_name source)
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${CMAKE_CURRENT_LIST_DIR}/../tests/test_workbench/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::Framework)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES LABELS "framework;test-workbench;test-explorer;diagnostics;coverage;quality")
    endfunction()
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-workbench-types-test framework.test_quality_workbench.workbench.types test_workbench_types.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-item-model-test framework.test_quality_workbench.test.item.model test_test_item_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-suite-model-test framework.test_quality_workbench.test.suite.model test_test_suite_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-group-model-test framework.test_quality_workbench.test.group.model test_test_group_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-tree-model-test framework.test_quality_workbench.test.tree.model test_test_tree_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-tree-filter-test framework.test_quality_workbench.test.tree.filter test_test_tree_filter.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-tree-selection-test framework.test_quality_workbench.test.tree.selection test_test_tree_selection.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-discovery-state-test framework.test_quality_workbench.test.discovery.state test_test_discovery_state.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-discovery-session-test framework.test_quality_workbench.test.discovery.session test_test_discovery_session.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-discovery-snapshot-test framework.test_quality_workbench.test.discovery.snapshot test_test_discovery_snapshot.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-run-request-test framework.test_quality_workbench.test.run.request test_test_run_request.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-run-profile-test framework.test_quality_workbench.test.run.profile test_test_run_profile.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-run-session-test framework.test_quality_workbench.test.run.session test_test_run_session.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-run-progress-test framework.test_quality_workbench.test.run.progress test_test_run_progress.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-run-result-test framework.test_quality_workbench.test.run.result test_test_run_result.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-result-item-test framework.test_quality_workbench.test.result.item test_test_result_item.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-result-group-test framework.test_quality_workbench.test.result.group test_test_result_group.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-result-history-test framework.test_quality_workbench.test.result.history test_test_result_history.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-result-snapshot-test framework.test_quality_workbench.test.result.snapshot test_test_result_snapshot.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-status-summary-test framework.test_quality_workbench.test.status.summary test_test_status_summary.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-duration-model-test framework.test_quality_workbench.test.duration.model test_test_duration_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-failure-model-test framework.test_quality_workbench.test.failure.model test_test_failure_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-failure-detail-test framework.test_quality_workbench.test.failure.detail test_test_failure_detail.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-output-entry-test framework.test_quality_workbench.test.output.entry test_test_output_entry.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-output-stream-test framework.test_quality_workbench.test.output.stream test_test_output_stream.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-output-filter-test framework.test_quality_workbench.test.output.filter test_test_output_filter.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-output-session-test framework.test_quality_workbench.test.output.session test_test_output_session.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-retry-policy-test framework.test_quality_workbench.test.retry.policy test_test_retry_policy.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-rerun-plan-test framework.test_quality_workbench.test.rerun.plan test_test_rerun_plan.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-debug-plan-test framework.test_quality_workbench.test.debug.plan test_test_debug_plan.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-parallelism-model-test framework.test_quality_workbench.test.parallelism.model test_test_parallelism_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-shard-model-test framework.test_quality_workbench.test.shard.model test_test_shard_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-environment-model-test framework.test_quality_workbench.test.environment.model test_test_environment_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-configuration-model-test framework.test_quality_workbench.test.configuration.model test_test_configuration_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-command-set-test framework.test_quality_workbench.test.command.set test_test_command_set.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-keymap-context-test framework.test_quality_workbench.test.keymap.context test_test_keymap_context.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-workspace-profile-test framework.test_quality_workbench.test.workspace.profile test_test_workspace_profile.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-diagnostic-item-model-test framework.test_quality_workbench.diagnostic.item.model test_diagnostic_item_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-diagnostic-group-model-test framework.test_quality_workbench.diagnostic.group.model test_diagnostic_group_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-diagnostic-filter-model-test framework.test_quality_workbench.diagnostic.filter.model test_diagnostic_filter_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-diagnostic-severity-summary-test framework.test_quality_workbench.diagnostic.severity.summary test_diagnostic_severity_summary.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-diagnostic-source-summary-test framework.test_quality_workbench.diagnostic.source.summary test_diagnostic_source_summary.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-diagnostic-snapshot-test framework.test_quality_workbench.diagnostic.snapshot test_diagnostic_snapshot.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-problem-navigation-model-test framework.test_quality_workbench.problem.navigation.model test_problem_navigation_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-problem-quick-fix-model-test framework.test_quality_workbench.problem.quick.fix.model test_problem_quick_fix_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-quality-gate-model-test framework.test_quality_workbench.quality.gate.model test_quality_gate_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-quality-gate-result-test framework.test_quality_workbench.quality.gate.result test_quality_gate_result.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-quality-budget-model-test framework.test_quality_workbench.quality.budget.model test_quality_budget_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-quality-score-model-test framework.test_quality_workbench.quality.score.model test_quality_score_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-coverage-file-model-test framework.test_quality_workbench.coverage.file.model test_coverage_file_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-coverage-region-model-test framework.test_quality_workbench.coverage.region.model test_coverage_region_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-coverage-line-model-test framework.test_quality_workbench.coverage.line.model test_coverage_line_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-coverage-branch-model-test framework.test_quality_workbench.coverage.branch.model test_coverage_branch_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-coverage-function-model-test framework.test_quality_workbench.coverage.function.model test_coverage_function_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-coverage-summary-model-test framework.test_quality_workbench.coverage.summary.model test_coverage_summary_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-coverage-filter-model-test framework.test_quality_workbench.coverage.filter.model test_coverage_filter_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-coverage-snapshot-test framework.test_quality_workbench.coverage.snapshot test_coverage_snapshot.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-coverage-delta-model-test framework.test_quality_workbench.coverage.delta.model test_coverage_delta_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-coverage-threshold-model-test framework.test_quality_workbench.coverage.threshold.model test_coverage_threshold_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-coverage-navigation-model-test framework.test_quality_workbench.coverage.navigation.model test_coverage_navigation_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-flaky-test-model-test framework.test_quality_workbench.flaky.test.model test_flaky_test_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-flaky-test-history-test framework.test_quality_workbench.flaky.test.history test_flaky_test_history.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-impact-model-test framework.test_quality_workbench.test.impact.model test_test_impact_model.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-impact-summary-test framework.test_quality_workbench.test.impact.summary test_test_impact_summary.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-quality-workbench-snapshot-test framework.test_quality_workbench.quality.workbench.snapshot test_quality_workbench_snapshot.c)
    umicom_add_test_quality_workbench_test(umicom-test-quality-workbench-test-quality-workbench-service-test framework.test_quality_workbench.test.quality.workbench.service test_test_quality_workbench_service.c)
endif()
