#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTestRuntimePlatform.cmake
#
# PURPOSE:
#   Register Framework test-runtime facilities and normalise child-process
#   runtime environments for every registered test.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
set(UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

add_library(umicom_test_runtime STATIC
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/types.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/failure_classifier.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/environment.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/search_path.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/working_directory.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/executable_probe.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/dependency_probe.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/launch_plan.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/launch_evidence.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/failure_classification.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/bad_command_evidence.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/crash_evidence.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/timeout_evidence.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/timeout_policy.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/retry_policy.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/test_descriptor.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/catalogue.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/selection.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/profile.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/profile_catalogue.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/shard.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/execution_batch.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/result.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/result_store.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/history.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/regression_baseline.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/diagnostic.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/diagnostics.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/metrics.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/health.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/report.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/artifact_locator.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/runtime_dependency.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/process_environment.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/runtime_probe.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/clean_machine.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/ctest_bridge.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/scheduler.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/command.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/event.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/query.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/service.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/controller.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/execution_guard.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/runtime_path.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/environment_entry.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/dependency_directory.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/executable_location.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/command_line.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/process_identity.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/process_status.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/start_failure.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/exit_interpretation.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/assertion_evidence.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/sanitizer_evidence.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/stack_evidence.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/memory_evidence.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/dependency_closure.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/runtime_manifest.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/environment_snapshot.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/execution_snapshot.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/failure_group.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/failure_summary.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/baseline_comparison.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/trend.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/duration_budget.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/slow_test.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/quarantine.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/reproduction.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/diagnostic_bundle.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/result_parser.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/ctest_record.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/ctest_result.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/test_label.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/label_catalogue.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/resource_lock.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/resource_group.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/temporary_directory.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/log_capture.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/evidence_retention.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/execution_statistics.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/parallel_budget.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/suite_partition.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/test_family.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/execution_order.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/rerun_plan.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/failure_reproduction.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/runtime_capability.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/platform_requirement.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/tool_requirement.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/network_requirement.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/filesystem_requirement.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/database_requirement.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/ui_requirement.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/failure_fingerprint.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/failure_owner.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/failure_queue.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/resolution_evidence.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/execution_gate.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/preflight.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/postflight.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/run_identity.c"
    "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/src/test_runtime/run_summary.c"
)
add_library(Umicom::test_runtime ALIAS umicom_test_runtime)
set_target_properties(umicom_test_runtime PROPERTIES EXPORT_NAME test_runtime)
target_include_directories(umicom_test_runtime PUBLIC
    $<BUILD_INTERFACE:${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(umicom_test_runtime PUBLIC Umicom::base)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_test_runtime)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_test_runtime)
endif()
if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE Umicom::test_runtime)
endif()

# Every child process receives the compiler runtime directory and the active
# build bin directory explicitly. This prevents CTest from reporting unrelated
# tests as BAD_COMMAND when Windows cannot locate UCRT64 runtime DLLs.
get_filename_component(UMICOM_TEST_RUNTIME_COMPILER_BIN "${CMAKE_C_COMPILER}" DIRECTORY)
if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(UMICOM_TEST_RUNTIME_BINARY_BIN "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
else()
    set(UMICOM_TEST_RUNTIME_BINARY_BIN "${CMAKE_BINARY_DIR}/bin")
endif()

function(umicom_configure_test_runtime test_name test_directory)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.28")
        set_property(TEST "${test_name}" DIRECTORY "${test_directory}" APPEND
            PROPERTY ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:${UMICOM_TEST_RUNTIME_BINARY_BIN}"
            "PATH=path_list_prepend:${UMICOM_TEST_RUNTIME_COMPILER_BIN}"
        )
        set_property(TEST "${test_name}" DIRECTORY "${test_directory}" APPEND
            PROPERTY ENVIRONMENT
            "UMICOM_TEST_SOURCE_ROOT=${CMAKE_SOURCE_DIR}"
            "UMICOM_TEST_BUILD_ROOT=${CMAKE_BINARY_DIR}"
            "UMICOM_TEST_RUNTIME_BIN=${UMICOM_TEST_RUNTIME_BINARY_BIN}"
        )
    elseif(test_directory STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        set_property(TEST "${test_name}" APPEND PROPERTY ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:${UMICOM_TEST_RUNTIME_BINARY_BIN}"
            "PATH=path_list_prepend:${UMICOM_TEST_RUNTIME_COMPILER_BIN}"
        )
        set_property(TEST "${test_name}" APPEND PROPERTY ENVIRONMENT
            "UMICOM_TEST_SOURCE_ROOT=${CMAKE_SOURCE_DIR}"
            "UMICOM_TEST_BUILD_ROOT=${CMAKE_BINARY_DIR}"
            "UMICOM_TEST_RUNTIME_BIN=${UMICOM_TEST_RUNTIME_BINARY_BIN}"
        )
    endif()
endfunction()

function(umicom_configure_test_runtime_directory directory_path)
    get_property(_tests DIRECTORY "${directory_path}" PROPERTY TESTS)
    foreach(_test IN LISTS _tests)
        umicom_configure_test_runtime("${_test}" "${directory_path}")
    endforeach()
    get_property(_children DIRECTORY "${directory_path}" PROPERTY SUBDIRECTORIES)
    foreach(_child IN LISTS _children)
        umicom_configure_test_runtime_directory("${_child}")
    endforeach()
endfunction()

function(umicom_configure_all_registered_test_runtimes)
    umicom_configure_test_runtime_directory("${CMAKE_SOURCE_DIR}")
endfunction()

# The deferred call runs after Framework and all product subdirectories have
# registered their tests, so the same runtime policy covers Studio as well.
cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}"
    CALL umicom_configure_all_registered_test_runtimes)

if(BUILD_TESTING)
    function(umicom_add_test_runtime_test target test_name source)
        add_executable("${target}" "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::test_runtime)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES LABELS "framework;test-runtime")
    endfunction()
    umicom_add_test_runtime_test(umicom-test-runtime-failure-classifier-test framework.test_runtime.failure_classifier tests/test_runtime/test_failure_classifier.c)
    umicom_add_test_runtime_test(umicom-test-runtime-environment-test framework.test_runtime.environment tests/test_runtime/test_environment.c)
    umicom_add_test_runtime_test(umicom-test-runtime-search-path-test framework.test_runtime.search_path tests/test_runtime/test_search_path.c)
    umicom_add_test_runtime_test(umicom-test-runtime-working-directory-test framework.test_runtime.working_directory tests/test_runtime/test_working_directory.c)
    umicom_add_test_runtime_test(umicom-test-runtime-executable-probe-test framework.test_runtime.executable_probe tests/test_runtime/test_executable_probe.c)
    umicom_add_test_runtime_test(umicom-test-runtime-dependency-probe-test framework.test_runtime.dependency_probe tests/test_runtime/test_dependency_probe.c)
    umicom_add_test_runtime_test(umicom-test-runtime-launch-plan-test framework.test_runtime.launch_plan tests/test_runtime/test_launch_plan.c)
    umicom_add_test_runtime_test(umicom-test-runtime-launch-evidence-test framework.test_runtime.launch_evidence tests/test_runtime/test_launch_evidence.c)
    umicom_add_test_runtime_test(umicom-test-runtime-failure-classification-test framework.test_runtime.failure_classification tests/test_runtime/test_failure_classification.c)
    umicom_add_test_runtime_test(umicom-test-runtime-bad-command-evidence-test framework.test_runtime.bad_command_evidence tests/test_runtime/test_bad_command_evidence.c)
    umicom_add_test_runtime_test(umicom-test-runtime-crash-evidence-test framework.test_runtime.crash_evidence tests/test_runtime/test_crash_evidence.c)
    umicom_add_test_runtime_test(umicom-test-runtime-timeout-evidence-test framework.test_runtime.timeout_evidence tests/test_runtime/test_timeout_evidence.c)
    umicom_add_test_runtime_test(umicom-test-runtime-timeout-policy-test framework.test_runtime.timeout_policy tests/test_runtime/test_timeout_policy.c)
    umicom_add_test_runtime_test(umicom-test-runtime-retry-policy-test framework.test_runtime.retry_policy tests/test_runtime/test_retry_policy.c)
    umicom_add_test_runtime_test(umicom-test-runtime-test-descriptor-test framework.test_runtime.test_descriptor tests/test_runtime/test_test_descriptor.c)
    umicom_add_test_runtime_test(umicom-test-runtime-catalogue-test framework.test_runtime.catalogue tests/test_runtime/test_catalogue.c)
    umicom_add_test_runtime_test(umicom-test-runtime-selection-test framework.test_runtime.selection tests/test_runtime/test_selection.c)
    umicom_add_test_runtime_test(umicom-test-runtime-profile-test framework.test_runtime.profile tests/test_runtime/test_profile.c)
    umicom_add_test_runtime_test(umicom-test-runtime-profile-catalogue-test framework.test_runtime.profile_catalogue tests/test_runtime/test_profile_catalogue.c)
    umicom_add_test_runtime_test(umicom-test-runtime-shard-test framework.test_runtime.shard tests/test_runtime/test_shard.c)
    umicom_add_test_runtime_test(umicom-test-runtime-execution-batch-test framework.test_runtime.execution_batch tests/test_runtime/test_execution_batch.c)
    umicom_add_test_runtime_test(umicom-test-runtime-result-test framework.test_runtime.result tests/test_runtime/test_result.c)
    umicom_add_test_runtime_test(umicom-test-runtime-result-store-test framework.test_runtime.result_store tests/test_runtime/test_result_store.c)
    umicom_add_test_runtime_test(umicom-test-runtime-history-test framework.test_runtime.history tests/test_runtime/test_history.c)
    umicom_add_test_runtime_test(umicom-test-runtime-regression-baseline-test framework.test_runtime.regression_baseline tests/test_runtime/test_regression_baseline.c)
    umicom_add_test_runtime_test(umicom-test-runtime-diagnostic-test framework.test_runtime.diagnostic tests/test_runtime/test_diagnostic.c)
    umicom_add_test_runtime_test(umicom-test-runtime-diagnostics-test framework.test_runtime.diagnostics tests/test_runtime/test_diagnostics.c)
    umicom_add_test_runtime_test(umicom-test-runtime-metrics-test framework.test_runtime.metrics tests/test_runtime/test_metrics.c)
    umicom_add_test_runtime_test(umicom-test-runtime-health-test framework.test_runtime.health tests/test_runtime/test_health.c)
    umicom_add_test_runtime_test(umicom-test-runtime-report-test framework.test_runtime.report tests/test_runtime/test_report.c)
    umicom_add_test_runtime_test(umicom-test-runtime-artifact-locator-test framework.test_runtime.artifact_locator tests/test_runtime/test_artifact_locator.c)
    umicom_add_test_runtime_test(umicom-test-runtime-runtime-dependency-test framework.test_runtime.runtime_dependency tests/test_runtime/test_runtime_dependency.c)
    umicom_add_test_runtime_test(umicom-test-runtime-process-environment-test framework.test_runtime.process_environment tests/test_runtime/test_process_environment.c)
    umicom_add_test_runtime_test(umicom-test-runtime-runtime-probe-test framework.test_runtime.runtime_probe tests/test_runtime/test_runtime_probe.c)
    umicom_add_test_runtime_test(umicom-test-runtime-clean-machine-test framework.test_runtime.clean_machine tests/test_runtime/test_clean_machine.c)
    umicom_add_test_runtime_test(umicom-test-runtime-ctest-bridge-test framework.test_runtime.ctest_bridge tests/test_runtime/test_ctest_bridge.c)
    umicom_add_test_runtime_test(umicom-test-runtime-scheduler-test framework.test_runtime.scheduler tests/test_runtime/test_scheduler.c)
    umicom_add_test_runtime_test(umicom-test-runtime-command-test framework.test_runtime.command tests/test_runtime/test_command.c)
    umicom_add_test_runtime_test(umicom-test-runtime-event-test framework.test_runtime.event tests/test_runtime/test_event.c)
    umicom_add_test_runtime_test(umicom-test-runtime-query-test framework.test_runtime.query tests/test_runtime/test_query.c)
    umicom_add_test_runtime_test(umicom-test-runtime-service-test framework.test_runtime.service tests/test_runtime/test_service.c)
    umicom_add_test_runtime_test(umicom-test-runtime-controller-test framework.test_runtime.controller tests/test_runtime/test_controller.c)
    umicom_add_test_runtime_test(umicom-test-runtime-execution-guard-test framework.test_runtime.execution_guard tests/test_runtime/test_execution_guard.c)
    umicom_add_test_runtime_test(umicom-test-runtime-runtime-path-test framework.test_runtime.runtime_path tests/test_runtime/test_runtime_path.c)
    umicom_add_test_runtime_test(umicom-test-runtime-environment-entry-test framework.test_runtime.environment_entry tests/test_runtime/test_environment_entry.c)
    umicom_add_test_runtime_test(umicom-test-runtime-dependency-directory-test framework.test_runtime.dependency_directory tests/test_runtime/test_dependency_directory.c)
    umicom_add_test_runtime_test(umicom-test-runtime-executable-location-test framework.test_runtime.executable_location tests/test_runtime/test_executable_location.c)
    umicom_add_test_runtime_test(umicom-test-runtime-command-line-test framework.test_runtime.command_line tests/test_runtime/test_command_line.c)
    umicom_add_test_runtime_test(umicom-test-runtime-process-identity-test framework.test_runtime.process_identity tests/test_runtime/test_process_identity.c)
    umicom_add_test_runtime_test(umicom-test-runtime-process-status-test framework.test_runtime.process_status tests/test_runtime/test_process_status.c)
    umicom_add_test_runtime_test(umicom-test-runtime-start-failure-test framework.test_runtime.start_failure tests/test_runtime/test_start_failure.c)
    umicom_add_test_runtime_test(umicom-test-runtime-exit-interpretation-test framework.test_runtime.exit_interpretation tests/test_runtime/test_exit_interpretation.c)
    umicom_add_test_runtime_test(umicom-test-runtime-assertion-evidence-test framework.test_runtime.assertion_evidence tests/test_runtime/test_assertion_evidence.c)
    umicom_add_test_runtime_test(umicom-test-runtime-sanitizer-evidence-test framework.test_runtime.sanitizer_evidence tests/test_runtime/test_sanitizer_evidence.c)
    umicom_add_test_runtime_test(umicom-test-runtime-stack-evidence-test framework.test_runtime.stack_evidence tests/test_runtime/test_stack_evidence.c)
    umicom_add_test_runtime_test(umicom-test-runtime-memory-evidence-test framework.test_runtime.memory_evidence tests/test_runtime/test_memory_evidence.c)
    umicom_add_test_runtime_test(umicom-test-runtime-dependency-closure-test framework.test_runtime.dependency_closure tests/test_runtime/test_dependency_closure.c)
    umicom_add_test_runtime_test(umicom-test-runtime-runtime-manifest-test framework.test_runtime.runtime_manifest tests/test_runtime/test_runtime_manifest.c)
    umicom_add_test_runtime_test(umicom-test-runtime-environment-snapshot-test framework.test_runtime.environment_snapshot tests/test_runtime/test_environment_snapshot.c)
    umicom_add_test_runtime_test(umicom-test-runtime-execution-snapshot-test framework.test_runtime.execution_snapshot tests/test_runtime/test_execution_snapshot.c)
    umicom_add_test_runtime_test(umicom-test-runtime-failure-group-test framework.test_runtime.failure_group tests/test_runtime/test_failure_group.c)
    umicom_add_test_runtime_test(umicom-test-runtime-failure-summary-test framework.test_runtime.failure_summary tests/test_runtime/test_failure_summary.c)
    umicom_add_test_runtime_test(umicom-test-runtime-baseline-comparison-test framework.test_runtime.baseline_comparison tests/test_runtime/test_baseline_comparison.c)
    umicom_add_test_runtime_test(umicom-test-runtime-trend-test framework.test_runtime.trend tests/test_runtime/test_trend.c)
    umicom_add_test_runtime_test(umicom-test-runtime-duration-budget-test framework.test_runtime.duration_budget tests/test_runtime/test_duration_budget.c)
    umicom_add_test_runtime_test(umicom-test-runtime-slow-test-test framework.test_runtime.slow_test tests/test_runtime/test_slow_test.c)
    umicom_add_test_runtime_test(umicom-test-runtime-quarantine-test framework.test_runtime.quarantine tests/test_runtime/test_quarantine.c)
    umicom_add_test_runtime_test(umicom-test-runtime-reproduction-test framework.test_runtime.reproduction tests/test_runtime/test_reproduction.c)
    umicom_add_test_runtime_test(umicom-test-runtime-diagnostic-bundle-test framework.test_runtime.diagnostic_bundle tests/test_runtime/test_diagnostic_bundle.c)
    umicom_add_test_runtime_test(umicom-test-runtime-result-parser-test framework.test_runtime.result_parser tests/test_runtime/test_result_parser.c)
    umicom_add_test_runtime_test(umicom-test-runtime-ctest-record-test framework.test_runtime.ctest_record tests/test_runtime/test_ctest_record.c)
    umicom_add_test_runtime_test(umicom-test-runtime-ctest-result-test framework.test_runtime.ctest_result tests/test_runtime/test_ctest_result.c)
    umicom_add_test_runtime_test(umicom-test-runtime-test-label-test framework.test_runtime.test_label tests/test_runtime/test_test_label.c)
    umicom_add_test_runtime_test(umicom-test-runtime-label-catalogue-test framework.test_runtime.label_catalogue tests/test_runtime/test_label_catalogue.c)
    umicom_add_test_runtime_test(umicom-test-runtime-resource-lock-test framework.test_runtime.resource_lock tests/test_runtime/test_resource_lock.c)
    umicom_add_test_runtime_test(umicom-test-runtime-resource-group-test framework.test_runtime.resource_group tests/test_runtime/test_resource_group.c)
    umicom_add_test_runtime_test(umicom-test-runtime-temporary-directory-test framework.test_runtime.temporary_directory tests/test_runtime/test_temporary_directory.c)
    umicom_add_test_runtime_test(umicom-test-runtime-log-capture-test framework.test_runtime.log_capture tests/test_runtime/test_log_capture.c)
    umicom_add_test_runtime_test(umicom-test-runtime-evidence-retention-test framework.test_runtime.evidence_retention tests/test_runtime/test_evidence_retention.c)
    umicom_add_test_runtime_test(umicom-test-runtime-execution-statistics-test framework.test_runtime.execution_statistics tests/test_runtime/test_execution_statistics.c)
    umicom_add_test_runtime_test(umicom-test-runtime-parallel-budget-test framework.test_runtime.parallel_budget tests/test_runtime/test_parallel_budget.c)
    umicom_add_test_runtime_test(umicom-test-runtime-suite-partition-test framework.test_runtime.suite_partition tests/test_runtime/test_suite_partition.c)
    umicom_add_test_runtime_test(umicom-test-runtime-test-family-test framework.test_runtime.test_family tests/test_runtime/test_test_family.c)
    umicom_add_test_runtime_test(umicom-test-runtime-execution-order-test framework.test_runtime.execution_order tests/test_runtime/test_execution_order.c)
    umicom_add_test_runtime_test(umicom-test-runtime-rerun-plan-test framework.test_runtime.rerun_plan tests/test_runtime/test_rerun_plan.c)
    umicom_add_test_runtime_test(umicom-test-runtime-failure-reproduction-test framework.test_runtime.failure_reproduction tests/test_runtime/test_failure_reproduction.c)
    umicom_add_test_runtime_test(umicom-test-runtime-runtime-capability-test framework.test_runtime.runtime_capability tests/test_runtime/test_runtime_capability.c)
    umicom_add_test_runtime_test(umicom-test-runtime-platform-requirement-test framework.test_runtime.platform_requirement tests/test_runtime/test_platform_requirement.c)
    umicom_add_test_runtime_test(umicom-test-runtime-tool-requirement-test framework.test_runtime.tool_requirement tests/test_runtime/test_tool_requirement.c)
    umicom_add_test_runtime_test(umicom-test-runtime-network-requirement-test framework.test_runtime.network_requirement tests/test_runtime/test_network_requirement.c)
    umicom_add_test_runtime_test(umicom-test-runtime-filesystem-requirement-test framework.test_runtime.filesystem_requirement tests/test_runtime/test_filesystem_requirement.c)
    umicom_add_test_runtime_test(umicom-test-runtime-database-requirement-test framework.test_runtime.database_requirement tests/test_runtime/test_database_requirement.c)
    umicom_add_test_runtime_test(umicom-test-runtime-ui-requirement-test framework.test_runtime.ui_requirement tests/test_runtime/test_ui_requirement.c)
    umicom_add_test_runtime_test(umicom-test-runtime-failure-fingerprint-test framework.test_runtime.failure_fingerprint tests/test_runtime/test_failure_fingerprint.c)
    umicom_add_test_runtime_test(umicom-test-runtime-failure-owner-test framework.test_runtime.failure_owner tests/test_runtime/test_failure_owner.c)
    umicom_add_test_runtime_test(umicom-test-runtime-failure-queue-test framework.test_runtime.failure_queue tests/test_runtime/test_failure_queue.c)
    umicom_add_test_runtime_test(umicom-test-runtime-resolution-evidence-test framework.test_runtime.resolution_evidence tests/test_runtime/test_resolution_evidence.c)
    umicom_add_test_runtime_test(umicom-test-runtime-execution-gate-test framework.test_runtime.execution_gate tests/test_runtime/test_execution_gate.c)
    umicom_add_test_runtime_test(umicom-test-runtime-preflight-test framework.test_runtime.preflight tests/test_runtime/test_preflight.c)
    umicom_add_test_runtime_test(umicom-test-runtime-postflight-test framework.test_runtime.postflight tests/test_runtime/test_postflight.c)
    umicom_add_test_runtime_test(umicom-test-runtime-run-identity-test framework.test_runtime.run_identity tests/test_runtime/test_run_identity.c)
    umicom_add_test_runtime_test(umicom-test-runtime-run-summary-test framework.test_runtime.run_summary tests/test_runtime/test_run_summary.c)
endif()

install(TARGETS umicom_test_runtime EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/include/umicom/test_runtime"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom")
message(STATUS "Umicom test runtime and regression execution platform enabled")

install(FILES "${UMICOM_TEST_RUNTIME_FRAMEWORK_ROOT}/resources/testing/execution-profiles.json"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/resources/testing")
