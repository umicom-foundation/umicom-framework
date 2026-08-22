#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWorkbenchSelectionProviderPlatform.cmake
#
# PURPOSE:
#   Register authoritative project/problem/source-control/test/trading model
#   adapters above the structured selection platform.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)

set(UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/..")

if(TARGET umicom_workbench_selection_provider)
    return()
endif()

if(NOT TARGET umicom_workbench_selection)
    message(FATAL_ERROR
        "Workbench Selection Provider requires Umicom::workbench_selection")
endif()

add_library(umicom_workbench_selection_provider STATIC
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/account_mapping.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/account_row_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/action_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/application_binding.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/audit_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/backpressure_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/bookmark_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/cache_key.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/cache_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/capability_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/causation_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/cell_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/coalesce_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/column_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/command_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/compatibility_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/conflict_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/context_binding.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/controller.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/conversion_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/correlation_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/counter_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/cursor_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/dedupe_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/descriptor.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/diagnostic_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/edge_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/enrichment_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/error_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/event_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/evidence_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/export_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/field_mapping.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/field_requirement.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/field_rule.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/filter_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/freshness_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/group_binding.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/health.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/health_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/hierarchy_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/history.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/history_entry.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/identity.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/import_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/instrument_mapping.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/instrument_row_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/latency_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/mapping_state.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/merge_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/metric_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/migration_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/node_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/normalization_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/page_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/paging_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/panel_binding.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/permission_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/pipeline_state.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/policy_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/presentation_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/problem.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/problem_mapping.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/problem_row_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/project.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/project_mapping.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/project_node_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/projection_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/provider_profile.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/provider_state.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/query_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/rate_limit_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/registry.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/remediation_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/replay_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/resolver_state.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/result_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/retry_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/revision_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/row_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/schema_compatibility.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/schema_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/search_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/selection_binding.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/selection_state.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/selection_token.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/service.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/snapshot.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/snapshot_item.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/sort_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/source_binding.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/source_branch_mapping.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/source_change_mapping.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/source_commit_mapping.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/source_control.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/source_row_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/staleness_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/studio_bridge.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/studio_profile.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/studio_profile_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/synchronization_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/telemetry_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/test.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/test_mapping.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/test_row_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/throughput_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/trace_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/trade_mapping.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/trade_row_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/trading_profile_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/trading_workbench.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/tree_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/types.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/validation_record.c"
    "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/src/workbench_selection_provider/warning_record.c"
)

add_library(Umicom::workbench_selection_provider ALIAS
    umicom_workbench_selection_provider)
set_target_properties(umicom_workbench_selection_provider PROPERTIES
    EXPORT_NAME workbench_selection_provider
)

target_include_directories(umicom_workbench_selection_provider PUBLIC
    $<BUILD_INTERFACE:${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# The provider converts authoritative Framework model snapshots to structured
# workbench selections. These dependencies are direct public contracts and must
# be present on the final static-link line for tests and application consumers.
target_link_libraries(umicom_workbench_selection_provider PUBLIC
    Umicom::workbench_selection
    Umicom::project
    Umicom::ui
    Umicom::source_control
    Umicom::test_platform
)

if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_workbench_selection_provider)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_workbench_selection_provider)
endif()

if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE
        Umicom::workbench_selection_provider
    )
endif()

install(
    TARGETS umicom_workbench_selection_provider
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY
        "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/include/umicom/workbench_selection_provider"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom"
)

install(
    FILES
        "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/resources/workbench-selection-provider-defaults.json"
        "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/resources/schemas/workbench-selection-provider.schema.json"
    DESTINATION
        "${CMAKE_INSTALL_DATADIR}/umicom/resources/workbench-selection-provider"
)

if(BUILD_TESTING)
    function(umicom_add_workbench_selection_provider_test target test_name source)
        add_executable("${target}"
            "${UMICOM_WORKBENCH_SELECTION_PROVIDER_FRAMEWORK_ROOT}/${source}"
        )
        target_link_libraries("${target}" PRIVATE
            Umicom::workbench_selection_provider
        )
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES
            LABELS "framework;workbench-selection-provider"
        )
    endfunction()

    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-account-mapping-test framework.workbench_selection_provider.account_mapping tests/workbench_selection_provider/test_account_mapping.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-account-row-record-test framework.workbench_selection_provider.account_row_record tests/workbench_selection_provider/test_account_row_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-action-record-test framework.workbench_selection_provider.action_record tests/workbench_selection_provider/test_action_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-application-binding-test framework.workbench_selection_provider.application_binding tests/workbench_selection_provider/test_application_binding.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-audit-record-test framework.workbench_selection_provider.audit_record tests/workbench_selection_provider/test_audit_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-backpressure-record-test framework.workbench_selection_provider.backpressure_record tests/workbench_selection_provider/test_backpressure_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-bookmark-record-test framework.workbench_selection_provider.bookmark_record tests/workbench_selection_provider/test_bookmark_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-cache-key-test framework.workbench_selection_provider.cache_key tests/workbench_selection_provider/test_cache_key.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-cache-record-test framework.workbench_selection_provider.cache_record tests/workbench_selection_provider/test_cache_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-capability-record-test framework.workbench_selection_provider.capability_record tests/workbench_selection_provider/test_capability_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-causation-record-test framework.workbench_selection_provider.causation_record tests/workbench_selection_provider/test_causation_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-cell-record-test framework.workbench_selection_provider.cell_record tests/workbench_selection_provider/test_cell_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-coalesce-record-test framework.workbench_selection_provider.coalesce_record tests/workbench_selection_provider/test_coalesce_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-column-record-test framework.workbench_selection_provider.column_record tests/workbench_selection_provider/test_column_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-command-record-test framework.workbench_selection_provider.command_record tests/workbench_selection_provider/test_command_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-compatibility-record-test framework.workbench_selection_provider.compatibility_record tests/workbench_selection_provider/test_compatibility_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-conflict-record-test framework.workbench_selection_provider.conflict_record tests/workbench_selection_provider/test_conflict_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-context-binding-test framework.workbench_selection_provider.context_binding tests/workbench_selection_provider/test_context_binding.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-conversion-record-test framework.workbench_selection_provider.conversion_record tests/workbench_selection_provider/test_conversion_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-correlation-record-test framework.workbench_selection_provider.correlation_record tests/workbench_selection_provider/test_correlation_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-counter-record-test framework.workbench_selection_provider.counter_record tests/workbench_selection_provider/test_counter_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-cursor-record-test framework.workbench_selection_provider.cursor_record tests/workbench_selection_provider/test_cursor_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-dedupe-record-test framework.workbench_selection_provider.dedupe_record tests/workbench_selection_provider/test_dedupe_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-descriptor-test framework.workbench_selection_provider.descriptor tests/workbench_selection_provider/test_descriptor.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-diagnostic-record-test framework.workbench_selection_provider.diagnostic_record tests/workbench_selection_provider/test_diagnostic_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-edge-record-test framework.workbench_selection_provider.edge_record tests/workbench_selection_provider/test_edge_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-enrichment-record-test framework.workbench_selection_provider.enrichment_record tests/workbench_selection_provider/test_enrichment_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-error-record-test framework.workbench_selection_provider.error_record tests/workbench_selection_provider/test_error_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-event-record-test framework.workbench_selection_provider.event_record tests/workbench_selection_provider/test_event_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-evidence-record-test framework.workbench_selection_provider.evidence_record tests/workbench_selection_provider/test_evidence_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-export-record-test framework.workbench_selection_provider.export_record tests/workbench_selection_provider/test_export_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-field-mapping-test framework.workbench_selection_provider.field_mapping tests/workbench_selection_provider/test_field_mapping.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-field-requirement-test framework.workbench_selection_provider.field_requirement tests/workbench_selection_provider/test_field_requirement.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-field-rule-test framework.workbench_selection_provider.field_rule tests/workbench_selection_provider/test_field_rule.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-filter-record-test framework.workbench_selection_provider.filter_record tests/workbench_selection_provider/test_filter_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-freshness-record-test framework.workbench_selection_provider.freshness_record tests/workbench_selection_provider/test_freshness_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-group-binding-test framework.workbench_selection_provider.group_binding tests/workbench_selection_provider/test_group_binding.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-health-record-test framework.workbench_selection_provider.health_record tests/workbench_selection_provider/test_health_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-hierarchy-record-test framework.workbench_selection_provider.hierarchy_record tests/workbench_selection_provider/test_hierarchy_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-history-entry-test framework.workbench_selection_provider.history_entry tests/workbench_selection_provider/test_history_entry.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-identity-test framework.workbench_selection_provider.identity tests/workbench_selection_provider/test_identity.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-import-record-test framework.workbench_selection_provider.import_record tests/workbench_selection_provider/test_import_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-instrument-mapping-test framework.workbench_selection_provider.instrument_mapping tests/workbench_selection_provider/test_instrument_mapping.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-instrument-row-record-test framework.workbench_selection_provider.instrument_row_record tests/workbench_selection_provider/test_instrument_row_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-latency-record-test framework.workbench_selection_provider.latency_record tests/workbench_selection_provider/test_latency_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-mapping-state-test framework.workbench_selection_provider.mapping_state tests/workbench_selection_provider/test_mapping_state.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-merge-record-test framework.workbench_selection_provider.merge_record tests/workbench_selection_provider/test_merge_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-metric-record-test framework.workbench_selection_provider.metric_record tests/workbench_selection_provider/test_metric_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-migration-record-test framework.workbench_selection_provider.migration_record tests/workbench_selection_provider/test_migration_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-node-record-test framework.workbench_selection_provider.node_record tests/workbench_selection_provider/test_node_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-normalization-record-test framework.workbench_selection_provider.normalization_record tests/workbench_selection_provider/test_normalization_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-page-record-test framework.workbench_selection_provider.page_record tests/workbench_selection_provider/test_page_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-paging-record-test framework.workbench_selection_provider.paging_record tests/workbench_selection_provider/test_paging_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-panel-binding-test framework.workbench_selection_provider.panel_binding tests/workbench_selection_provider/test_panel_binding.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-permission-record-test framework.workbench_selection_provider.permission_record tests/workbench_selection_provider/test_permission_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-pipeline-state-test framework.workbench_selection_provider.pipeline_state tests/workbench_selection_provider/test_pipeline_state.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-policy-record-test framework.workbench_selection_provider.policy_record tests/workbench_selection_provider/test_policy_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-presentation-record-test framework.workbench_selection_provider.presentation_record tests/workbench_selection_provider/test_presentation_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-problem-mapping-test framework.workbench_selection_provider.problem_mapping tests/workbench_selection_provider/test_problem_mapping.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-problem-provider-test framework.workbench_selection_provider.problem_provider tests/workbench_selection_provider/test_problem_provider.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-problem-row-record-test framework.workbench_selection_provider.problem_row_record tests/workbench_selection_provider/test_problem_row_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-project-mapping-test framework.workbench_selection_provider.project_mapping tests/workbench_selection_provider/test_project_mapping.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-project-node-record-test framework.workbench_selection_provider.project_node_record tests/workbench_selection_provider/test_project_node_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-project-provider-test framework.workbench_selection_provider.project_provider tests/workbench_selection_provider/test_project_provider.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-projection-record-test framework.workbench_selection_provider.projection_record tests/workbench_selection_provider/test_projection_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-provider-profile-test framework.workbench_selection_provider.provider_profile tests/workbench_selection_provider/test_provider_profile.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-provider-state-test framework.workbench_selection_provider.provider_state tests/workbench_selection_provider/test_provider_state.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-query-record-test framework.workbench_selection_provider.query_record tests/workbench_selection_provider/test_query_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-rate-limit-record-test framework.workbench_selection_provider.rate_limit_record tests/workbench_selection_provider/test_rate_limit_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-registry-test framework.workbench_selection_provider.registry tests/workbench_selection_provider/test_registry.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-remediation-record-test framework.workbench_selection_provider.remediation_record tests/workbench_selection_provider/test_remediation_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-replay-record-test framework.workbench_selection_provider.replay_record tests/workbench_selection_provider/test_replay_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-resolver-state-test framework.workbench_selection_provider.resolver_state tests/workbench_selection_provider/test_resolver_state.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-result-record-test framework.workbench_selection_provider.result_record tests/workbench_selection_provider/test_result_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-retry-record-test framework.workbench_selection_provider.retry_record tests/workbench_selection_provider/test_retry_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-revision-record-test framework.workbench_selection_provider.revision_record tests/workbench_selection_provider/test_revision_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-row-record-test framework.workbench_selection_provider.row_record tests/workbench_selection_provider/test_row_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-schema-compatibility-test framework.workbench_selection_provider.schema_compatibility tests/workbench_selection_provider/test_schema_compatibility.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-schema-record-test framework.workbench_selection_provider.schema_record tests/workbench_selection_provider/test_schema_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-search-record-test framework.workbench_selection_provider.search_record tests/workbench_selection_provider/test_search_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-selection-binding-test framework.workbench_selection_provider.selection_binding tests/workbench_selection_provider/test_selection_binding.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-selection-state-test framework.workbench_selection_provider.selection_state tests/workbench_selection_provider/test_selection_state.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-selection-token-test framework.workbench_selection_provider.selection_token tests/workbench_selection_provider/test_selection_token.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-snapshot-item-test framework.workbench_selection_provider.snapshot_item tests/workbench_selection_provider/test_snapshot_item.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-sort-record-test framework.workbench_selection_provider.sort_record tests/workbench_selection_provider/test_sort_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-source-binding-test framework.workbench_selection_provider.source_binding tests/workbench_selection_provider/test_source_binding.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-source-branch-mapping-test framework.workbench_selection_provider.source_branch_mapping tests/workbench_selection_provider/test_source_branch_mapping.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-source-change-mapping-test framework.workbench_selection_provider.source_change_mapping tests/workbench_selection_provider/test_source_change_mapping.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-source-commit-mapping-test framework.workbench_selection_provider.source_commit_mapping tests/workbench_selection_provider/test_source_commit_mapping.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-source-control-provider-test framework.workbench_selection_provider.source_control_provider tests/workbench_selection_provider/test_source_control_provider.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-source-row-record-test framework.workbench_selection_provider.source_row_record tests/workbench_selection_provider/test_source_row_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-staleness-record-test framework.workbench_selection_provider.staleness_record tests/workbench_selection_provider/test_staleness_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-studio-profile-test framework.workbench_selection_provider.studio_profile tests/workbench_selection_provider/test_studio_profile.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-studio-profile-record-test framework.workbench_selection_provider.studio_profile_record tests/workbench_selection_provider/test_studio_profile_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-synchronization-record-test framework.workbench_selection_provider.synchronization_record tests/workbench_selection_provider/test_synchronization_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-telemetry-record-test framework.workbench_selection_provider.telemetry_record tests/workbench_selection_provider/test_telemetry_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-test-mapping-test framework.workbench_selection_provider.test_mapping tests/workbench_selection_provider/test_test_mapping.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-test-provider-test framework.workbench_selection_provider.test_provider tests/workbench_selection_provider/test_test_provider.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-test-row-record-test framework.workbench_selection_provider.test_row_record tests/workbench_selection_provider/test_test_row_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-throughput-record-test framework.workbench_selection_provider.throughput_record tests/workbench_selection_provider/test_throughput_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-trace-record-test framework.workbench_selection_provider.trace_record tests/workbench_selection_provider/test_trace_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-trade-mapping-test framework.workbench_selection_provider.trade_mapping tests/workbench_selection_provider/test_trade_mapping.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-trade-row-record-test framework.workbench_selection_provider.trade_row_record tests/workbench_selection_provider/test_trade_row_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-trading-profile-record-test framework.workbench_selection_provider.trading_profile_record tests/workbench_selection_provider/test_trading_profile_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-trading-workbench-test framework.workbench_selection_provider.trading_workbench tests/workbench_selection_provider/test_trading_workbench.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-tree-record-test framework.workbench_selection_provider.tree_record tests/workbench_selection_provider/test_tree_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-types-test framework.workbench_selection_provider.types tests/workbench_selection_provider/test_types.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-validation-record-test framework.workbench_selection_provider.validation_record tests/workbench_selection_provider/test_validation_record.c)
    umicom_add_workbench_selection_provider_test(umicom-workbench-selection-provider-warning-record-test framework.workbench_selection_provider.warning_record tests/workbench_selection_provider/test_warning_record.c)
endif()

message(STATUS "Umicom structured model selection provider platform enabled")
