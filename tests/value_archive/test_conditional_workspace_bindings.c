/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/test_conditional_workspace_bindings.c
 *
 * PURPOSE:
 *   Link every bounded conditional value owner and exercise real web and SDK state restoration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/compiler/native/declaration.h"
#include "umicom/compiler/native/symbol.h"
#include "umicom/data/enterprise/backup_manifest.h"
#include "umicom/data/enterprise/backup_plan.h"
#include "umicom/data/enterprise/connection_slot.h"
#include "umicom/data/enterprise/data_audit_event.h"
#include "umicom/data/enterprise/data_operation.h"
#include "umicom/data/enterprise/data_server_profile.h"
#include "umicom/data/enterprise/data_session.h"
#include "umicom/data/enterprise/entity_descriptor.h"
#include "umicom/data/enterprise/field_mapping.h"
#include "umicom/data/enterprise/migration_checkpoint.h"
#include "umicom/data/enterprise/migration_step.h"
#include "umicom/data/enterprise/query_expression.h"
#include "umicom/data/enterprise/query_join.h"
#include "umicom/data/enterprise/query_order.h"
#include "umicom/data/enterprise/query_parameter.h"
#include "umicom/data/enterprise/query_plan.h"
#include "umicom/data/enterprise/query_projection.h"
#include "umicom/data/enterprise/relation_mapping.h"
#include "umicom/data/enterprise/replica_descriptor.h"
#include "umicom/data/enterprise/restore_plan.h"
#include "umicom/data/enterprise/result_mapping.h"
#include "umicom/data/enterprise/savepoint_plan.h"
#include "umicom/data/enterprise/schema_column.h"
#include "umicom/data/enterprise/schema_foreign_key.h"
#include "umicom/data/enterprise/schema_identifier.h"
#include "umicom/data/enterprise/schema_index.h"
#include "umicom/data/enterprise/statement_plan.h"
#include "umicom/delivery/installer.h"
#include "umicom/delivery/manifest.h"
#include "umicom/delivery/package.h"
#include "umicom/delivery/promotion.h"
#include "umicom/delivery/provenance.h"
#include "umicom/delivery/rollback.h"
#include "umicom/delivery/update_channel.h"
#include "umicom/delivery/upgrade_plan.h"
#include "umicom/observability/performance/allocation_event.h"
#include "umicom/observability/performance/allocation_site.h"
#include "umicom/observability/performance/allocation_summary.h"
#include "umicom/observability/performance/benchmark_baseline.h"
#include "umicom/observability/performance/benchmark_case.h"
#include "umicom/observability/performance/benchmark_comparison.h"
#include "umicom/observability/performance/benchmark_run.h"
#include "umicom/observability/performance/benchmark_sample.h"
#include "umicom/observability/performance/benchmark_statistics.h"
#include "umicom/observability/performance/correlation_context.h"
#include "umicom/observability/performance/counter.h"
#include "umicom/observability/performance/coverage_baseline.h"
#include "umicom/observability/performance/coverage_file.h"
#include "umicom/observability/performance/coverage_region.h"
#include "umicom/observability/performance/coverage_regression.h"
#include "umicom/observability/performance/coverage_summary.h"
#include "umicom/observability/performance/crash_cluster.h"
#include "umicom/observability/performance/crash_correlation.h"
#include "umicom/observability/performance/crash_signature.h"
#include "umicom/observability/performance/dashboard_card.h"
#include "umicom/observability/performance/dashboard_layout.h"
#include "umicom/observability/performance/gauge.h"
#include "umicom/observability/performance/heap_snapshot.h"
#include "umicom/observability/performance/histogram.h"
#include "umicom/observability/performance/io_telemetry.h"
#include "umicom/observability/performance/leak_candidate.h"
#include "umicom/observability/performance/memory_budget.h"
#include "umicom/observability/performance/memory_pressure.h"
#include "umicom/observability/performance/memory_regression.h"
#include "umicom/observability/performance/metric_descriptor.h"
#include "umicom/observability/performance/metric_sample.h"
#include "umicom/observability/performance/metric_series.h"
#include "umicom/observability/performance/network_telemetry.h"
#include "umicom/observability/performance/observability_snapshot.h"
#include "umicom/observability/performance/operations_dashboard.h"
#include "umicom/observability/performance/performance_budget.h"
#include "umicom/observability/performance/performance_regression.h"
#include "umicom/observability/performance/performance_session.h"
#include "umicom/observability/performance/process_telemetry.h"
#include "umicom/observability/performance/profiler_aggregation.h"
#include "umicom/observability/performance/profiler_call_tree.h"
#include "umicom/observability/performance/profiler_capability.h"
#include "umicom/observability/performance/profiler_configuration.h"
#include "umicom/observability/performance/profiler_filter.h"
#include "umicom/observability/performance/profiler_flamegraph.h"
#include "umicom/observability/performance/profiler_frame.h"
#include "umicom/observability/performance/profiler_hotpath.h"
#include "umicom/observability/performance/profiler_hotspot.h"
#include "umicom/observability/performance/profiler_process.h"
#include "umicom/observability/performance/profiler_sample.h"
#include "umicom/observability/performance/profiler_sample_buffer.h"
#include "umicom/observability/performance/profiler_session.h"
#include "umicom/observability/performance/profiler_stack.h"
#include "umicom/observability/performance/profiler_symbol.h"
#include "umicom/observability/performance/profiler_target.h"
#include "umicom/observability/performance/profiler_thread.h"
#include "umicom/observability/performance/resource_budget.h"
#include "umicom/observability/performance/resource_sample.h"
#include "umicom/observability/performance/runtime_health.h"
#include "umicom/observability/performance/threshold_policy.h"
#include "umicom/observability/performance/timeline.h"
#include "umicom/observability/performance/trace_event.h"
#include "umicom/observability/performance/trace_filter.h"
#include "umicom/observability/performance/trace_session.h"
#include "umicom/observability/performance/trace_span.h"
#include "umicom/platform/bookmarks.h"
#include "umicom/platform/cross_target/abi_descriptor.h"
#include "umicom/platform/cross_target/boot_service.h"
#include "umicom/platform/cross_target/clock_semantics.h"
#include "umicom/platform/cross_target/cpu_profile.h"
#include "umicom/platform/cross_target/cross_build_contract.h"
#include "umicom/platform/cross_target/cross_target_snapshot.h"
#include "umicom/platform/cross_target/hardware_bus.h"
#include "umicom/platform/cross_target/hardware_device.h"
#include "umicom/platform/cross_target/kernel_boundary.h"
#include "umicom/platform/cross_target/memory_semantics.h"
#include "umicom/platform/cross_target/os_architecture_decision.h"
#include "umicom/platform/cross_target/page_policy.h"
#include "umicom/platform/cross_target/riscv_platform.h"
#include "umicom/platform/cross_target/scheduler_profile.h"
#include "umicom/platform/cross_target/target_probe.h"
#include "umicom/platform/cross_target/timer_descriptor.h"
#include "umicom/platform/file_operation_queue.h"
#include "umicom/platform/resource_location.h"
#include "umicom/platform/workspace_history.h"
#include "umicom/runtime/bootstrap/types.h"
#include "umicom/sdk_runtime/abi_requirement.h"
#include "umicom/sdk_runtime/adapter_root.h"
#include "umicom/sdk_runtime/architecture.h"
#include "umicom/sdk_runtime/binary_evidence.h"
#include "umicom/sdk_runtime/binary_manifest.h"
#include "umicom/sdk_runtime/clean_machine.h"
#include "umicom/sdk_runtime/cmake_package.h"
#include "umicom/sdk_runtime/cmake_target_map.h"
#include "umicom/sdk_runtime/command.h"
#include "umicom/sdk_runtime/compatibility.h"
#include "umicom/sdk_runtime/compatibility_matrix.h"
#include "umicom/sdk_runtime/compiler_runtime.h"
#include "umicom/sdk_runtime/component.h"
#include "umicom/sdk_runtime/component_catalogue.h"
#include "umicom/sdk_runtime/consumer_evidence.h"
#include "umicom/sdk_runtime/consumer_plan.h"
#include "umicom/sdk_runtime/controller.h"
#include "umicom/sdk_runtime/dependency_evidence.h"
#include "umicom/sdk_runtime/dependency_graph.h"
#include "umicom/sdk_runtime/dependency_manifest.h"
#include "umicom/sdk_runtime/deployment_layout.h"
#include "umicom/sdk_runtime/deployment_probe.h"
#include "umicom/sdk_runtime/diagnostic.h"
#include "umicom/sdk_runtime/environment_probe.h"
#include "umicom/sdk_runtime/event.h"
#include "umicom/sdk_runtime/export_target.h"
#include "umicom/sdk_runtime/header_root.h"
#include "umicom/sdk_runtime/health.h"
#include "umicom/sdk_runtime/install_probe.h"
#include "umicom/sdk_runtime/install_receipt.h"
#include "umicom/sdk_runtime/installation_layout.h"
#include "umicom/sdk_runtime/library_root.h"
#include "umicom/sdk_runtime/loader_evidence.h"
#include "umicom/sdk_runtime/loader_plan.h"
#include "umicom/sdk_runtime/loader_search.h"
#include "umicom/sdk_runtime/metrics.h"
#include "umicom/sdk_runtime/package_channel.h"
#include "umicom/sdk_runtime/package_evidence.h"
#include "umicom/sdk_runtime/package_generation.h"
#include "umicom/sdk_runtime/package_manifest.h"
#include "umicom/sdk_runtime/package_root.h"
#include "umicom/sdk_runtime/package_search.h"
#include "umicom/sdk_runtime/package_validator.h"
#include "umicom/sdk_runtime/path_normalisation.h"
#include "umicom/sdk_runtime/platform_profile.h"
#include "umicom/sdk_runtime/plugin_root.h"
#include "umicom/sdk_runtime/query.h"
#include "umicom/sdk_runtime/relocation.h"
#include "umicom/sdk_runtime/repair_plan.h"
#include "umicom/sdk_runtime/resource_root.h"
#include "umicom/sdk_runtime/rollback_plan.h"
#include "umicom/sdk_runtime/runtime_bundle.h"
#include "umicom/sdk_runtime/runtime_environment.h"
#include "umicom/sdk_runtime/runtime_file.h"
#include "umicom/sdk_runtime/runtime_file_catalogue.h"
#include "umicom/sdk_runtime/runtime_policy.h"
#include "umicom/sdk_runtime/runtime_resolver.h"
#include "umicom/sdk_runtime/sdk_bundle.h"
#include "umicom/sdk_runtime/sdk_catalogue.h"
#include "umicom/sdk_runtime/sdk_profile.h"
#include "umicom/sdk_runtime/search_path.h"
#include "umicom/sdk_runtime/service.h"
#include "umicom/sdk_runtime/ucrt_runtime.h"
#include "umicom/sdk_runtime/uninstall_plan.h"
#include "umicom/sdk_runtime/upgrade_plan.h"
#include "umicom/sdk_runtime/version_contract.h"
#include "umicom/web/server_config.h"
#include "umicom/web/workbench/cloud_object.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    if (umi_web_server_config_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_web_workbench_cloud_profile_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_backup_manifest_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_backup_plan_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_connection_slot_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_data_audit_event_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_data_operation_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_data_server_profile_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_data_session_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_entity_descriptor_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_field_mapping_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_migration_checkpoint_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_migration_step_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_query_expression_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_query_join_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_query_order_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_query_parameter_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_query_plan_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_query_projection_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_relation_mapping_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_replica_descriptor_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_restore_plan_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_result_mapping_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_savepoint_plan_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_schema_column_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_schema_foreign_key_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_schema_identifier_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_schema_index_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_data_statement_plan_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_platform_bookmarks_snapshot_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_abi_descriptor_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_boot_service_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_clock_semantics_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_cpu_profile_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_cross_build_contract_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_cross_target_snapshot_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_hardware_bus_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_hardware_device_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_kernel_boundary_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_memory_semantics_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_umicom_os_architecture_decision_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_page_policy_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_riscv_platform_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_scheduler_profile_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_target_probe_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_ct_timer_descriptor_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_platform_file_operation_queue_snapshot_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_platform_resource_location_snapshot_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_platform_workspace_history_snapshot_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_nc_declaration_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_nc_symbol_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_abi_requirement_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_adapter_root_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_architecture_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_binary_evidence_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_binary_manifest_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_clean_machine_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_cmake_package_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_cmake_target_map_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_command_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_compatibility_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_compatibility_matrix_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_compiler_runtime_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_component_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_component_catalogue_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_consumer_evidence_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_consumer_plan_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_controller_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_dependency_evidence_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_dependency_graph_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_dependency_manifest_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_deployment_layout_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_deployment_probe_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_diagnostic_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_environment_probe_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_event_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_export_target_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_header_root_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_health_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_installation_layout_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_install_probe_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_install_receipt_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_library_root_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_loader_evidence_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_loader_plan_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_loader_search_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_metrics_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_package_channel_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_package_evidence_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_package_generation_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_package_manifest_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_package_root_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_package_search_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_package_validator_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_path_normalisation_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_platform_profile_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_plugin_root_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_query_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_relocation_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_repair_plan_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_resource_root_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_rollback_plan_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_runtime_bundle_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_runtime_environment_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_runtime_file_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_runtime_file_catalogue_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_runtime_policy_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_runtime_resolver_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_sdk_bundle_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_sdk_catalogue_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_sdk_profile_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_search_path_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_service_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_ucrt_runtime_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_uninstall_plan_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_upgrade_plan_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_sdk_runtime_version_contract_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_bootstrap_factory_descriptor_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_bootstrap_provider_descriptor_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_bootstrap_service_descriptor_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_installer_plan_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_delivery_manifest_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_package_spec_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_promotion_direction_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_provenance_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_rollback_plan_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_update_channel_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_upgrade_plan_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_allocation_event_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_allocation_site_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_allocation_summary_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_benchmark_baseline_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_benchmark_case_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_benchmark_comparison_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_benchmark_run_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_benchmark_sample_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_benchmark_statistics_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_correlation_context_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_counter_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_coverage_baseline_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_coverage_file_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_coverage_region_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_coverage_regression_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_coverage_summary_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_crash_cluster_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_crash_correlation_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_crash_signature_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_dashboard_card_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_dashboard_layout_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_gauge_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_heap_snapshot_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_histogram_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_io_telemetry_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_leak_candidate_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_memory_budget_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_memory_pressure_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_memory_regression_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_metric_descriptor_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_metric_sample_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_metric_series_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_network_telemetry_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_observability_snapshot_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_operations_dashboard_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_performance_budget_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_performance_regression_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_performance_session_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_process_telemetry_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_aggregation_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_call_tree_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_capability_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_configuration_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_filter_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_flamegraph_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_frame_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_hotpath_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_hotspot_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_process_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_sample_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_sample_buffer_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_session_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_stack_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_symbol_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_target_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_profiler_thread_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_resource_budget_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_resource_sample_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_runtime_health_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_threshold_policy_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_timeline_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_trace_event_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_trace_filter_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_trace_session_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    if (umi_performance_trace_span_archive_decode_if_unchanged_with_workspace(NULL, 0U, NULL, NULL, NULL, 0U, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;

    UmiWebServerConfig expected=umi_web_server_config_default(),value=expected,workspace,incoming=expected;
    unsigned char archive[512],scratch[1024];size_t length=0U,required=0U;
    incoming.port=8123U;
    if(umi_web_server_config_archive_encode(&incoming,archive,sizeof(archive),&length)!=UMI_STATUS_OK)return 1;
    if(umi_web_server_config_archive_encode(&expected,NULL,0U,&required)!=UMI_STATUS_OK)return 1;
    if(umi_web_server_config_archive_decode_if_unchanged_with_workspace(archive,length,&expected,
        &workspace,scratch,required*2U,&value)!=UMI_STATUS_OK||value.port!=8123U)return 1;
    if(umi_web_server_config_archive_decode_if_unchanged_with_workspace(archive,length,&expected,
        &workspace,scratch,sizeof(scratch),&value)!=UMI_STATUS_INVALID_STATE)return 1;
    puts("All typed entries linked; real web configuration restore and stale review checked.");
    return 0;
}
