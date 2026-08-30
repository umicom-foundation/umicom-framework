# Generated source inventory for Framework developer capabilities.
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
set(UMICOM_DEVELOPER_CAPABILITY_SOURCES
    src/developer/capabilities/workspace_discovery.c
    src/developer/capabilities/profile_selection.c
    src/developer/capabilities/preset_import.c
    src/developer/capabilities/environment_probe.c
    src/developer/capabilities/toolchain_detection.c
    src/developer/capabilities/dependency_graph.c
    src/developer/capabilities/cache_control.c
    src/developer/capabilities/cross_compile_profile.c
    src/developer/capabilities/feature_option_matrix.c
    src/developer/capabilities/configure_diagnostics.c
    src/developer/capabilities/incremental_build.c
    src/developer/capabilities/parallel_build.c
    src/developer/capabilities/target_selection.c
    src/developer/capabilities/multi_configuration.c
    src/developer/capabilities/compile_commands.c
    src/developer/capabilities/artifact_indexing.c
    src/developer/capabilities/diagnostic_parsing.c
    src/developer/capabilities/warning_policy.c
    src/developer/capabilities/sanitizer_profile.c
    src/developer/capabilities/build_cancellation.c
    src/developer/capabilities/test_discovery.c
    src/developer/capabilities/test_filtering.c
    src/developer/capabilities/test_sharding.c
    src/developer/capabilities/parallel_testing.c
    src/developer/capabilities/test_timeout.c
    src/developer/capabilities/test_retry.c
    src/developer/capabilities/test_fixtures.c
    src/developer/capabilities/coverage_capture.c
    src/developer/capabilities/machine_reports.c
    src/developer/capabilities/continuous_testing.c
    src/developer/capabilities/launch_configuration.c
    src/developer/capabilities/process_supervision.c
    src/developer/capabilities/stdio_capture.c
    src/developer/capabilities/run_history.c
    src/developer/capabilities/restart_policy.c
    src/developer/capabilities/attach_process.c
    src/developer/capabilities/elevation_policy.c
    src/developer/capabilities/environment_overlay.c
    src/developer/capabilities/argument_quoting.c
    src/developer/capabilities/run_readiness.c
    src/developer/capabilities/terminal_session.c
    src/developer/capabilities/terminal_transcript.c
    src/developer/capabilities/terminal_history.c
    src/developer/capabilities/terminal_tabs.c
    src/developer/capabilities/terminal_split.c
    src/developer/capabilities/terminal_search.c
    src/developer/capabilities/terminal_links.c
    src/developer/capabilities/shell_profiles.c
    src/developer/capabilities/working_directory_sync.c
    src/developer/capabilities/terminal_clear.c
    src/developer/capabilities/install_prefix.c
    src/developer/capabilities/install_plan.c
    src/developer/capabilities/staging_directory.c
    src/developer/capabilities/package_manifest.c
    src/developer/capabilities/artifact_signing.c
    src/developer/capabilities/package_verification.c
    src/developer/capabilities/smoke_testing.c
    src/developer/capabilities/generation_promotion.c
    src/developer/capabilities/rollback_generation.c
    src/developer/capabilities/update_feed.c
    src/developer/capabilities/problem_registry.c
    src/developer/capabilities/output_channels.c
    src/developer/capabilities/progress_reporting.c
    src/developer/capabilities/structured_logging.c
    src/developer/capabilities/runtime_metrics.c
    src/developer/capabilities/distributed_tracing.c
    src/developer/capabilities/crash_reporting.c
    src/developer/capabilities/readiness_reporting.c
    src/developer/capabilities/health_snapshot.c
    src/developer/capabilities/diagnostic_export.c
    src/developer/capabilities/workspace_trust.c
    src/developer/capabilities/command_policy.c
    src/developer/capabilities/secret_redaction.c
    src/developer/capabilities/process_allowlist.c
    src/developer/capabilities/artifact_integrity.c
    src/developer/capabilities/plugin_trust.c
    src/developer/capabilities/audit_trail.c
    src/developer/capabilities/path_policy.c
    src/developer/capabilities/environment_filtering.c
    src/developer/capabilities/privilege_boundary.c
    src/developer/capabilities/build_cache.c
    src/developer/capabilities/dependency_invalidation.c
    src/developer/capabilities/watch_debounce.c
    src/developer/capabilities/output_backpressure.c
    src/developer/capabilities/terminal_ring_buffer.c
    src/developer/capabilities/parallel_job_budget.c
    src/developer/capabilities/memory_budget.c
    src/developer/capabilities/latency_metrics.c
    src/developer/capabilities/cold_start.c
    src/developer/capabilities/index_scheduling.c
    src/developer/capabilities/cmake_provider.c
    src/developer/capabilities/ninja_provider.c
    src/developer/capabilities/ctest_provider.c
    src/developer/capabilities/meson_provider.c
    src/developer/capabilities/make_provider.c
    src/developer/capabilities/msbuild_provider.c
    src/developer/capabilities/clang_toolchain.c
    src/developer/capabilities/gcc_toolchain.c
    src/developer/capabilities/msvc_toolchain.c
    src/developer/capabilities/container_runner.c
    src/developer/capabilities/command_palette.c
    src/developer/capabilities/task_status.c
    src/developer/capabilities/quick_pick.c
    src/developer/capabilities/notifications.c
    src/developer/capabilities/build_dashboard.c
    src/developer/capabilities/test_explorer.c
    src/developer/capabilities/terminal_panel.c
    src/developer/capabilities/run_configurations.c
    src/developer/capabilities/deployment_centre.c
    src/developer/capabilities/keyboard_navigation.c
    src/developer/capabilities/windows_ucrt.c
    src/developer/capabilities/linux_glibc.c
    src/developer/capabilities/macos.c
    src/developer/capabilities/x86_64.c
    src/developer/capabilities/aarch64.c
    src/developer/capabilities/riscv64.c
    src/developer/capabilities/path_normalisation.c
    src/developer/capabilities/unicode_process.c
    src/developer/capabilities/shell_independence.c
    src/developer/capabilities/capability_negotiation.c
)

# -----------------------------------------------------------------------------
# Professional Test Explorer and coverage platform.
#
# This inventory is loaded after the canonical umicom_test_platform target has
# been created. Attaching the new sources here keeps the large top-level build
# definition stable while preserving one authoritative test-platform library.
# -----------------------------------------------------------------------------
if(NOT TARGET umicom_test_platform)
    message(FATAL_ERROR
        "Professional Test Explorer sources require umicom_test_platform")
endif()

target_sources(umicom_test_platform PRIVATE
    src/test_platform/provider_registry.c
    src/test_platform/explorer_model.c
    src/test_platform/rerun_policy.c
    src/test_platform/coverage_analysis.c
    src/test_platform/benchmark_analysis.c
    src/test_platform/history_analysis.c
    src/test_platform/explorer_session.c
    src/test_platform/command.c
)

# -----------------------------------------------------------------------------
# Build Graph, Tasks and Deployment platform.
#
# These sources extend the canonical umicom_build target. Applications consume
# the same incremental planner, task graph, artifact evidence and deployment
# controller; no application owns a parallel build or delivery implementation.
# -----------------------------------------------------------------------------
if(NOT TARGET umicom_build)
    message(FATAL_ERROR
        "Build Graph, Tasks and Deployment sources require umicom_build")
endif()

target_sources(umicom_build PRIVATE
    src/build/incremental.c
    src/build/task.c
    src/build/task_graph.c
    src/build/task_orchestrator.c
    src/build/artifact_manifest.c
    src/build/deployment_target.c
    src/build/deployment_plan.c
    src/build/deployment_controller.c
    src/build/pipeline.c
    src/build/delivery_catalogue.c
)

# -----------------------------------------------------------------------------
# Native developer lifecycle execution provider.
#
# The declarative build graph remains Framework-owned and provider-neutral.
# This adapter binds semantic tasks to the existing validated local toolchain,
# so Studio does not duplicate CMake/CTest/process execution.
# -----------------------------------------------------------------------------
if(NOT TARGET umicom_toolchain)
    message(FATAL_ERROR
        "Native developer lifecycle execution requires umicom_toolchain")
endif()

target_sources(umicom_toolchain PRIVATE
    src/toolchain/task_executor.c
    src/toolchain/discovery_defaults.c
)

target_link_libraries(umicom_toolchain PUBLIC
    Umicom::build
    Umicom::security
)

if(BUILD_TESTING)
    add_executable(umicom-toolchain-build-contract-test
        tests/toolchain_lifecycle/test_build_contract.c)
    target_link_libraries(umicom-toolchain-build-contract-test
        PRIVATE Umicom::toolchain)
    umicom_apply_warnings(umicom-toolchain-build-contract-test)
    umicom_apply_sanitizers(umicom-toolchain-build-contract-test)
    add_test(NAME framework.toolchain.lifecycle.build_contract
        COMMAND umicom-toolchain-build-contract-test)

    add_executable(umicom-toolchain-build-validation-test
        tests/toolchain_lifecycle/test_build_validation.c)
    target_link_libraries(umicom-toolchain-build-validation-test
        PRIVATE Umicom::toolchain)
    umicom_apply_warnings(umicom-toolchain-build-validation-test)
    umicom_apply_sanitizers(umicom-toolchain-build-validation-test)
    add_test(NAME framework.toolchain.lifecycle.build_validation
        COMMAND umicom-toolchain-build-validation-test)

    add_executable(umicom-toolchain-task-executor-contract-test
        tests/toolchain_lifecycle/test_task_executor_contract.c)
    target_link_libraries(umicom-toolchain-task-executor-contract-test
        PRIVATE Umicom::toolchain)
    umicom_apply_warnings(umicom-toolchain-task-executor-contract-test)
    umicom_apply_sanitizers(umicom-toolchain-task-executor-contract-test)
    add_test(NAME framework.toolchain.lifecycle.task_executor
        COMMAND umicom-toolchain-task-executor-contract-test)

    add_executable(umicom-toolchain-command-parser-test
        tests/toolchain_lifecycle/test_command_parser.c)
    target_link_libraries(umicom-toolchain-command-parser-test
        PRIVATE Umicom::toolchain)
    umicom_apply_warnings(umicom-toolchain-command-parser-test)
    umicom_apply_sanitizers(umicom-toolchain-command-parser-test)
    add_test(NAME framework.toolchain.lifecycle.command_parser
        COMMAND umicom-toolchain-command-parser-test)

    add_executable(umicom-toolchain-environment-overlay-test
        tests/toolchain_lifecycle/test_environment_overlay.c)
    target_link_libraries(umicom-toolchain-environment-overlay-test
        PRIVATE Umicom::toolchain)
    umicom_apply_warnings(umicom-toolchain-environment-overlay-test)
    umicom_apply_sanitizers(umicom-toolchain-environment-overlay-test)
    add_test(NAME framework.toolchain.lifecycle.environment_overlay
        COMMAND umicom-toolchain-environment-overlay-test)

    add_executable(umicom-toolchain-task-executor-policy-test
        tests/toolchain_lifecycle/test_task_executor_policy.c)
    target_link_libraries(umicom-toolchain-task-executor-policy-test
        PRIVATE Umicom::toolchain)
    umicom_apply_warnings(umicom-toolchain-task-executor-policy-test)
    umicom_apply_sanitizers(umicom-toolchain-task-executor-policy-test)
    add_test(NAME framework.toolchain.lifecycle.task_executor_policy
        COMMAND umicom-toolchain-task-executor-policy-test)

    add_executable(umicom-toolchain-discovery-defaults-test
        tests/toolchain_lifecycle/test_discovery_defaults.c)
    target_link_libraries(umicom-toolchain-discovery-defaults-test
        PRIVATE Umicom::toolchain)
    umicom_apply_warnings(umicom-toolchain-discovery-defaults-test)
    umicom_apply_sanitizers(umicom-toolchain-discovery-defaults-test)
    add_test(NAME framework.toolchain.lifecycle.discovery_defaults
        COMMAND umicom-toolchain-discovery-defaults-test)
endif()
