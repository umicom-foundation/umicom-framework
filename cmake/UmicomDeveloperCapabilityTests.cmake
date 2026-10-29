#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDeveloperCapabilityTests.cmake
#
# PURPOSE:
#   Configure the umicom developer capability tests build rules without
#   duplicating product logic.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

# Creator: Sammy Hegab | Umicom Foundation | MIT
function(umicom_add_developer_capability_tests)
    umicom_add_framework_test(umicom-developer-capabilities-01 framework.developer.capabilities.01 tests/developer_capabilities/test_developer_capabilities_01.c)
    umicom_add_framework_test(umicom-developer-capabilities-02 framework.developer.capabilities.02 tests/developer_capabilities/test_developer_capabilities_02.c)
    umicom_add_framework_test(umicom-developer-capabilities-03 framework.developer.capabilities.03 tests/developer_capabilities/test_developer_capabilities_03.c)
    umicom_add_framework_test(umicom-developer-capabilities-04 framework.developer.capabilities.04 tests/developer_capabilities/test_developer_capabilities_04.c)
    umicom_add_framework_test(umicom-developer-capabilities-05 framework.developer.capabilities.05 tests/developer_capabilities/test_developer_capabilities_05.c)
    umicom_add_framework_test(umicom-developer-capabilities-06 framework.developer.capabilities.06 tests/developer_capabilities/test_developer_capabilities_06.c)
    umicom_add_framework_test(umicom-developer-capabilities-07 framework.developer.capabilities.07 tests/developer_capabilities/test_developer_capabilities_07.c)
    umicom_add_framework_test(umicom-developer-capabilities-08 framework.developer.capabilities.08 tests/developer_capabilities/test_developer_capabilities_08.c)
    umicom_add_framework_test(umicom-developer-capabilities-09 framework.developer.capabilities.09 tests/developer_capabilities/test_developer_capabilities_09.c)
    umicom_add_framework_test(umicom-developer-capabilities-10 framework.developer.capabilities.10 tests/developer_capabilities/test_developer_capabilities_10.c)
    umicom_add_framework_test(umicom-developer-capabilities-11 framework.developer.capabilities.11 tests/developer_capabilities/test_developer_capabilities_11.c)
    umicom_add_framework_test(umicom-developer-capabilities-12 framework.developer.capabilities.12 tests/developer_capabilities/test_developer_capabilities_12.c)
    umicom_add_framework_test(umicom-developer-capabilities-13 framework.developer.capabilities.13 tests/developer_capabilities/test_developer_capabilities_13.c)
    umicom_add_framework_test(umicom-developer-capabilities-14 framework.developer.capabilities.14 tests/developer_capabilities/test_developer_capabilities_14.c)
    umicom_add_framework_test(umicom-developer-capabilities-15 framework.developer.capabilities.15 tests/developer_capabilities/test_developer_capabilities_15.c)
    umicom_add_framework_test(umicom-developer-capabilities-16 framework.developer.capabilities.16 tests/developer_capabilities/test_developer_capabilities_16.c)
    umicom_add_framework_test(umicom-developer-capabilities-17 framework.developer.capabilities.17 tests/developer_capabilities/test_developer_capabilities_17.c)
    umicom_add_framework_test(umicom-developer-capabilities-18 framework.developer.capabilities.18 tests/developer_capabilities/test_developer_capabilities_18.c)
    umicom_add_framework_test(umicom-developer-capabilities-19 framework.developer.capabilities.19 tests/developer_capabilities/test_developer_capabilities_19.c)
    umicom_add_framework_test(umicom-developer-capabilities-20 framework.developer.capabilities.20 tests/developer_capabilities/test_developer_capabilities_20.c)
    umicom_add_framework_test(umicom-developer-capabilities-21 framework.developer.capabilities.21 tests/developer_capabilities/test_developer_capabilities_21.c)
    umicom_add_framework_test(umicom-developer-capabilities-22 framework.developer.capabilities.22 tests/developer_capabilities/test_developer_capabilities_22.c)
    umicom_add_framework_test(umicom-developer-capabilities-23 framework.developer.capabilities.23 tests/developer_capabilities/test_developer_capabilities_23.c)
    umicom_add_framework_test(umicom-developer-capabilities-24 framework.developer.capabilities.24 tests/developer_capabilities/test_developer_capabilities_24.c)

    # Run choices use all retained records, independently of evidence display limits.
    add_executable(umicom-test-evidence-runs-test tests/test_evidence_runs.c)
    target_link_libraries(umicom-test-evidence-runs-test PRIVATE Umicom::test_platform)
    set_target_properties(umicom-test-evidence-runs-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-test-evidence-runs-test)
    umicom_apply_sanitizers(umicom-test-evidence-runs-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-test-evidence-runs-test)
    endif()
    foreach(case empty catalogue all exact source-links output-only unknown-run stale-result stale-output identity lifetime older-run outcomes arguments)
        add_test(NAME framework.test.evidence_runs.${case} COMMAND umicom-test-evidence-runs-test "${case}")
        set_tests_properties(framework.test.evidence_runs.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;test-platform;evidence;regression")
    endforeach()

    # Bounded immutable pages preserve canonical ordering and source revisions.
    add_executable(umicom-test-explorer-page-test tests/test_explorer_page.c)
    target_link_libraries(umicom-test-explorer-page-test PRIVATE Umicom::test_platform)
    set_target_properties(umicom-test-explorer-page-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-test-explorer-page-test)
    umicom_apply_sanitizers(umicom-test-explorer-page-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-test-explorer-page-test)
    endif()
    foreach(case empty first middle last aligned oversized single exact-boundary order filter hierarchy outcomes immutable stale-sort stale-selection stale-rebuild wrong-model arguments)
        add_test(NAME framework.test.explorer_page.${case} COMMAND umicom-test-explorer-page-test "${case}")
        set_tests_properties(framework.test.explorer_page.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;test-platform;paging;regression")
    endforeach()

    # Full projection capacity is independent of page and selection limits.
    add_executable(umicom-test-explorer-scale-test tests/test_explorer_scale.c)
    target_link_libraries(umicom-test-explorer-scale-test PRIVATE Umicom::test_platform)
    set_target_properties(umicom-test-explorer-scale-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-test-explorer-scale-test)
    umicom_apply_sanitizers(umicom-test-explorer-scale-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-test-explorer-scale-test)
    endif()
    foreach(case empty large-flat catalogue-limit deep-chain cycle orphan ancestor-filter source-sort outcome-sort duration-sort framework-sort group-suite group-framework group-outcome result-sequence result-revision snapshot-fields state-retained state-removed-return empty-return collapse-expand missing-selection missing-expansion selection-limit invalid-filter-text invalid-filter-suite invalid-filter-label page-tail page-stale owner-lifetime)
        add_test(NAME framework.test.explorer_scale.${case} COMMAND umicom-test-explorer-scale-test "${case}")
        set_tests_properties(framework.test.explorer_scale.${case} PROPERTIES TIMEOUT 120 LABELS "framework;test-platform;projection;regression")
    endforeach()
    # The full item registry uses substantial memory; avoid concurrent stress fixtures.
    set_tests_properties(framework.test.explorer_scale.catalogue-limit PROPERTIES RUN_SERIAL TRUE)
    add_executable(umicom-test-explorer-allocation-test tests/test_explorer_allocation.c)
    target_link_libraries(umicom-test-explorer-allocation-test PRIVATE Umicom::test_platform)
    set_target_properties(umicom-test-explorer-allocation-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-test-explorer-allocation-test)
    umicom_apply_sanitizers(umicom-test-explorer-allocation-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-test-explorer-allocation-test)
    endif()
    add_test(NAME framework.test.explorer_scale.allocation COMMAND umicom-test-explorer-allocation-test)
    set_tests_properties(framework.test.explorer_scale.allocation PROPERTIES TIMEOUT 30 LABELS "framework;test-platform;projection;regression")


    # Shared filters admit complete bounded selections and retain exact outcomes.
    add_executable(umicom-test-filter-contract-test tests/test_filter_contract.c)
    target_link_libraries(umicom-test-filter-contract-test PRIVATE Umicom::test_platform)
    set_target_properties(umicom-test-filter-contract-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-test-filter-contract-test)
    umicom_apply_sanitizers(umicom-test-filter-contract-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-test-filter-contract-test)
    endif()
    foreach(case arguments text-bound suite-bound label-bound item-id-bound item-kind-bound item-name-bound item-suite-bound item-labels-bound disabled case-insensitive suite-exact failed timed-out cancelled skipped not-run exact-outcome selection-empty selection-order selection-limit selection-exact-limit selection-invalid selection-no-results latest-sequence latest-revision indexed-history)
        add_test(NAME framework.test.filter_contract.${case} COMMAND umicom-test-filter-contract-test "${case}")
        set_tests_properties(framework.test.filter_contract.${case} PROPERTIES TIMEOUT 30 LABELS "framework;test-platform;filter;regression")
    endforeach()

    add_executable(umicom-test-filter-allocation-test tests/test_filter_allocation.c)
    target_link_libraries(umicom-test-filter-allocation-test PRIVATE Umicom::test_platform)
    set_target_properties(umicom-test-filter-allocation-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-test-filter-allocation-test)
    umicom_apply_sanitizers(umicom-test-filter-allocation-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-test-filter-allocation-test)
    endif()
    add_test(NAME framework.test.filter_contract.allocation COMMAND umicom-test-filter-allocation-test)
    set_tests_properties(framework.test.filter_contract.allocation PROPERTIES TIMEOUT 30 LABELS "framework;test-platform;filter;regression")

    # Presentation and single-test plans support catalogues beyond execution batch size.
    add_executable(umicom-test-workspace-catalogue-test tests/test_workspace_catalogue.c)
    target_link_libraries(umicom-test-workspace-catalogue-test PRIVATE Umicom::test_platform)
    set_target_properties(umicom-test-workspace-catalogue-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-test-workspace-catalogue-test)
    umicom_apply_sanitizers(umicom-test-workspace-catalogue-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-test-workspace-catalogue-test)
    endif()
    foreach(case large-snapshot large-select large-visible large-plan-selected large-plan-all filter-membership filter-refusal selected-removed empty timeout-only run-capacity result-outcomes missing-selection inspect-selected inspect-unknown inspect-invalid inspect-row inspect-out-of-range inspect-latest-sequence inspect-latest-revision inspect-result-metadata rerun-filtered rerun-passed rerun-timeout)
        add_test(NAME framework.test.workspace_catalogue.${case} COMMAND umicom-test-workspace-catalogue-test "${case}")
        set_tests_properties(framework.test.workspace_catalogue.${case} PROPERTIES TIMEOUT 120 LABELS "framework;test-platform;workspace;regression")
        if(case MATCHES "^large-")
            set_tests_properties(framework.test.workspace_catalogue.${case} PROPERTIES RUN_SERIAL TRUE)
        endif()
    endforeach()

    # Keep each refusal observable without starting external tests or a GUI.
    add_executable(umicom-test-workspace-filter tests/test_workspace_filter_transaction.c)
    target_link_libraries(umicom-test-workspace-filter PRIVATE Umicom::test_platform)
    set_target_properties(umicom-test-workspace-filter PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-test-workspace-filter)
    umicom_apply_sanitizers(umicom-test-workspace-filter)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-test-workspace-filter)
    endif()
    foreach(case counts large selection-retained selection-replaced empty text-bound suite-bound label-bound invalid-outcome null-query null-output normalize failed-only)
        add_test(NAME framework.test.workspace_filter_transaction.${case} COMMAND umicom-test-workspace-filter "${case}")
        set_tests_properties(framework.test.workspace_filter_transaction.${case} PROPERTIES TIMEOUT 120 LABELS "framework;test-platform;transaction;regression")
        if(case STREQUAL "large")
            set_tests_properties(framework.test.workspace_filter_transaction.${case} PROPERTIES RUN_SERIAL TRUE)
        endif()
    endforeach()

    # Keep each refusal observable without starting external tests or a GUI.
    add_executable(umicom-test-selection-transaction tests/test_operation_selection_transaction.c)
    target_link_libraries(umicom-test-selection-transaction PRIVATE Umicom::test_platform)
    set_target_properties(umicom-test-selection-transaction PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-test-selection-transaction)
    umicom_apply_sanitizers(umicom-test-selection-transaction)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-test-selection-transaction)
    endif()
    foreach(case order missing-later duplicate-later null-later empty-id long-id over-capacity empty-request generation-full generation-near-full alias repeat invalid-prior)
        add_test(NAME framework.test.operation_selection_transaction.${case} COMMAND umicom-test-selection-transaction "${case}")
        set_tests_properties(framework.test.operation_selection_transaction.${case} PROPERTIES TIMEOUT 120 LABELS "framework;test-platform;transaction;regression")
        if(case STREQUAL "large")
            set_tests_properties(framework.test.operation_selection_transaction.${case} PROPERTIES RUN_SERIAL TRUE)
        endif()
    endforeach()

    # Keep each refusal observable without starting external tests or a GUI.
    add_executable(umicom-test-workspace-filter-failure tests/test_workspace_filter_failure.c)
    target_link_libraries(umicom-test-workspace-filter-failure PRIVATE Umicom::test_platform)
    set_target_properties(umicom-test-workspace-filter-failure PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-test-workspace-filter-failure)
    umicom_apply_sanitizers(umicom-test-workspace-filter-failure)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-test-workspace-filter-failure)
    endif()
    foreach(case allocation stale revision-overflow retry)
        add_test(NAME framework.test.workspace_filter_failure.${case} COMMAND umicom-test-workspace-filter-failure "${case}")
        set_tests_properties(framework.test.workspace_filter_failure.${case} PROPERTIES TIMEOUT 120 LABELS "framework;test-platform;transaction;regression")
        if(case STREQUAL "large")
            set_tests_properties(framework.test.workspace_filter_failure.${case} PROPERTIES RUN_SERIAL TRUE)
        endif()
    endforeach()

    include("${CMAKE_CURRENT_SOURCE_DIR}/tests/test_explorer_query.cmake")

    # Professional Test Explorer, rerun, coverage, benchmark and history layer.
    umicom_add_framework_test(umicom-selected-test-evidence-tests framework.test.selected_evidence tests/test_selected_test_evidence.c)
    umicom_add_framework_test(umicom-evidence-text-test framework.test.evidence_text tests/test_evidence_text.c)
    umicom_add_framework_test(umicom-test-provider-registry-tests framework.test.provider_registry tests/test_test_provider_registry.c)
    umicom_add_framework_test(umicom-test-explorer-model-tests framework.test.explorer_model tests/test_test_explorer_model.c)
    umicom_add_framework_test(umicom-test-rerun-policy-tests framework.test.rerun_policy tests/test_test_rerun_policy.c)
    umicom_add_framework_test(umicom-test-coverage-analysis-tests framework.test.coverage_analysis tests/test_test_coverage_analysis.c)
    foreach(_coverage_case IN ITEMS
        ratio-boundaries
        null-lookup
        long-session
        bad-policy
        bad-baseline
        arguments
        overflow-lines-file
        overflow-lines-aggregate
        overflow-branches-file
        overflow-branches-aggregate
        invalid-lines
        invalid-branches
        maximum-sum
        large-ratio
        empty-unknown
        empty-allowed
        mixed-unknown
        unknown-warning
        unknown-failed
        session-filter)
        add_test(NAME "framework.test.coverage_analysis.${_coverage_case}"
            COMMAND umicom-test-coverage-analysis-tests "${_coverage_case}")
        set_tests_properties("framework.test.coverage_analysis.${_coverage_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;testing;coverage;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.test.coverage_analysis.${_coverage_case}"
                TARGETS umicom-test-coverage-analysis-tests)
        endif()
    endforeach()

    umicom_add_framework_test(umicom-test-benchmark-analysis-tests framework.test.benchmark_analysis tests/test_test_benchmark_analysis.c)
    foreach(_benchmark_case IN ITEMS
        nan-value
        infinite-value
        nan-baseline
        infinite-baseline
        policy-nan-regression
        policy-infinite-improvement
        maximum-constant
        small-scale
        large-scale
        extreme-signed
        cancelled-baseline
        opposite-baseline
        delta-overflow
        baseline-absent
        provider-regression
        provider-disabled
        negative-values
        higher-direction
        short-samples
        late-invalid-metric
        arguments)
        add_test(NAME "framework.test.benchmark_analysis.${_benchmark_case}"
            COMMAND umicom-test-benchmark-analysis-tests "${_benchmark_case}")
        set_tests_properties("framework.test.benchmark_analysis.${_benchmark_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;testing;benchmark;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.test.benchmark_analysis.${_benchmark_case}"
                TARGETS umicom-test-benchmark-analysis-tests)
        endif()
    endforeach()

    umicom_add_framework_test(umicom-test-history-analysis-tests framework.test.history_analysis tests/test_test_history_analysis.c)
    foreach(_history_case IN ITEMS
        nan-duration
        infinite-duration
        negative-duration
        unknown-outcome
        policy-nan-slow
        policy-infinite-slow
        policy-nan-trend
        policy-infinite-trend
        skipped-only
        cancelled-only
        not-run-only
        latest-skipped
        minimum-completed
        completed-timing
        maximum-duration
        trend-overflow
        trend-drop
        trend-rise
        timed-out-streak
        zero-minimum-empty
        unrelated-invalid
        late-invalid-item
        arguments)
        add_test(NAME "framework.test.history_analysis.${_history_case}"
            COMMAND umicom-test-history-analysis-tests "${_history_case}")
        set_tests_properties("framework.test.history_analysis.${_history_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;testing;history;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.test.history_analysis.${_history_case}"
                TARGETS umicom-test-history-analysis-tests)
        endif()
    endforeach()

    umicom_add_framework_test(umicom-test-explorer-session-tests framework.test.explorer_session tests/test_test_explorer_session.c)
    umicom_add_framework_test(umicom-test-platform-command-tests framework.test.commands tests/test_test_platform_command.c)


    # Incremental build graph, professional tasks, artifacts and deployment.
    umicom_add_framework_test(umicom-build-automation-tests framework.build.automation tests/test_build_automation.c)
    umicom_add_framework_test(umicom-build-automation-schedule-tests framework.build.automation_schedule tests/test_build_automation_schedule.c)
    umicom_add_framework_test(umicom-build-continuous-integration-tests framework.build.continuous_integration tests/test_build_continuous_integration.c)
    umicom_add_framework_test(umicom-build-incremental-tests framework.build.incremental tests/test_build_incremental.c)
    umicom_add_framework_test(umicom-build-task-tests framework.build.task tests/test_build_task.c)
    umicom_add_framework_test(umicom-build-task-graph-tests framework.build.task_graph tests/test_build_task_graph.c)
    umicom_add_framework_test(umicom-build-task-orchestrator-tests framework.build.task_orchestrator tests/test_build_task_orchestrator.c)
    umicom_add_framework_test(umicom-build-artifact-manifest-tests framework.build.artifact_manifest tests/test_build_artifact_manifest.c)
    umicom_add_framework_test(umicom-build-deployment-target-tests framework.build.deployment_target tests/test_build_deployment_target.c)
    umicom_add_framework_test(umicom-build-deployment-plan-tests framework.build.deployment_plan tests/test_build_deployment_plan.c)
    umicom_add_framework_test(umicom-build-deployment-controller-tests framework.build.deployment_controller tests/test_build_deployment_controller.c)
    umicom_add_framework_test(umicom-build-delivery-pipeline-tests framework.build.delivery_pipeline tests/test_build_pipeline.c)
    umicom_add_framework_test(umicom-build-delivery-catalogue-tests framework.build.delivery_catalogue tests/test_build_delivery_catalogue.c)
endfunction()
