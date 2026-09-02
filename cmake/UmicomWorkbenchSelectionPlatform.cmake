#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWorkbenchSelectionPlatform.cmake
#
# PURPOSE:
#   Register structured project/problem/VCS/test/AI/trading selections above
#   the live interaction source and typed-context event pipeline.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)

set(UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/..")

# Configure the optional target only when its feature has created it.
if(TARGET umicom_workbench_selection)
    return()
endif()

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_workbench_context_source)
    message(FATAL_ERROR
        "Workbench Selection requires Umicom::workbench_context_source")
endif()

add_library(umicom_workbench_selection STATIC
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/account_item.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/activation_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/adapter_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/ai_item.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/application_binding.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/application_item.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/audit_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/binding_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/bookmark_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/builders.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/capability_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/causation_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/command_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/compare_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/consumer_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/consumer_state.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/correlation_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/debug_item.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/diagnostic_item.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/duplicate_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/event_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/evidence_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/export_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/field.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/field_mapping.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/field_requirement.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/file_node.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/filter_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/focus_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/group_binding.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/health_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/history_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/import_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/instrument_item.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/latency_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/media_item.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/merge_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/metric_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/multi_selection.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/navigation_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/node_capability.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/node_identity.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/node_parent.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/node_path.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/open_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/panel_binding.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/permission_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/policy_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/preview_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/producer_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/producer_state.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/profile_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/project_node.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/provider_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/provider_state.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/publication_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/query_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/rate_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/refresh_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/rejection_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/remediation_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/resolver.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/resolver_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/result_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/reveal_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/schema_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/search_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/selection.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/selection_origin.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/selection_set.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/selection_state.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/service.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/session_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/snapshot_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/source_branch.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/source_change.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/source_commit.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/stale_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/statistics_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/terminal_item.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/test_item.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/test_result.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/trace_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/trade_item.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/types.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/validation_record.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/workspace_binding.c"
    "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/src/workbench_selection/workspace_item.c"
)
add_library(Umicom::workbench_selection ALIAS
    umicom_workbench_selection)
set_target_properties(umicom_workbench_selection PROPERTIES
    EXPORT_NAME workbench_selection
)

target_include_directories(umicom_workbench_selection PUBLIC
    $<BUILD_INTERFACE:${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(umicom_workbench_selection PUBLIC
    Umicom::workbench_context_source
)

# Use the shared build helper when it is available from the parent composition.
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_workbench_selection)
endif()
# Use the shared build helper when it is available from the parent composition.
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_workbench_selection)
endif()

# Configure the optional target only when its feature has created it.
if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE
        Umicom::workbench_selection
    )
endif()

install(
    TARGETS umicom_workbench_selection
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY
        "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/include/umicom/workbench_selection"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom"
)

install(
    FILES
        "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/resources/workbench-selection-defaults.json"
        "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/resources/schemas/workbench-selection.schema.json"
    DESTINATION
        "${CMAKE_INSTALL_DATADIR}/umicom/resources/workbench-selection"
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add workbench selection test build helper so parent and application projects
    # apply one consistent rule.
    function(umicom_add_workbench_selection_test target test_name source)
        add_executable("${target}"
            "${UMICOM_WORKBENCH_SELECTION_FRAMEWORK_ROOT}/${source}"
        )
        target_link_libraries("${target}" PRIVATE
            Umicom::workbench_selection
        )
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES
            LABELS "framework;workbench-selection"
        )
    endfunction()

    umicom_add_workbench_selection_test(umicom-workbench-selection-account-item-test framework.workbench_selection.account_item tests/workbench_selection/test_account_item.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-activation-record-test framework.workbench_selection.activation_record tests/workbench_selection/test_activation_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-adapter-record-test framework.workbench_selection.adapter_record tests/workbench_selection/test_adapter_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-ai-item-test framework.workbench_selection.ai_item tests/workbench_selection/test_ai_item.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-application-binding-test framework.workbench_selection.application_binding tests/workbench_selection/test_application_binding.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-application-item-test framework.workbench_selection.application_item tests/workbench_selection/test_application_item.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-audit-record-test framework.workbench_selection.audit_record tests/workbench_selection/test_audit_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-binding-record-test framework.workbench_selection.binding_record tests/workbench_selection/test_binding_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-bookmark-record-test framework.workbench_selection.bookmark_record tests/workbench_selection/test_bookmark_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-builders-test framework.workbench_selection.builders tests/workbench_selection/test_builders.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-capability-record-test framework.workbench_selection.capability_record tests/workbench_selection/test_capability_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-causation-record-test framework.workbench_selection.causation_record tests/workbench_selection/test_causation_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-command-record-test framework.workbench_selection.command_record tests/workbench_selection/test_command_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-compare-record-test framework.workbench_selection.compare_record tests/workbench_selection/test_compare_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-consumer-record-test framework.workbench_selection.consumer_record tests/workbench_selection/test_consumer_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-consumer-state-test framework.workbench_selection.consumer_state tests/workbench_selection/test_consumer_state.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-correlation-record-test framework.workbench_selection.correlation_record tests/workbench_selection/test_correlation_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-debug-item-test framework.workbench_selection.debug_item tests/workbench_selection/test_debug_item.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-diagnostic-item-test framework.workbench_selection.diagnostic_item tests/workbench_selection/test_diagnostic_item.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-duplicate-record-test framework.workbench_selection.duplicate_record tests/workbench_selection/test_duplicate_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-event-record-test framework.workbench_selection.event_record tests/workbench_selection/test_event_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-evidence-record-test framework.workbench_selection.evidence_record tests/workbench_selection/test_evidence_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-export-record-test framework.workbench_selection.export_record tests/workbench_selection/test_export_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-field-test framework.workbench_selection.field tests/workbench_selection/test_field.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-field-mapping-test framework.workbench_selection.field_mapping tests/workbench_selection/test_field_mapping.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-field-requirement-test framework.workbench_selection.field_requirement tests/workbench_selection/test_field_requirement.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-file-node-test framework.workbench_selection.file_node tests/workbench_selection/test_file_node.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-filter-record-test framework.workbench_selection.filter_record tests/workbench_selection/test_filter_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-focus-record-test framework.workbench_selection.focus_record tests/workbench_selection/test_focus_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-group-binding-test framework.workbench_selection.group_binding tests/workbench_selection/test_group_binding.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-health-record-test framework.workbench_selection.health_record tests/workbench_selection/test_health_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-history-record-test framework.workbench_selection.history_record tests/workbench_selection/test_history_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-import-record-test framework.workbench_selection.import_record tests/workbench_selection/test_import_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-instrument-item-test framework.workbench_selection.instrument_item tests/workbench_selection/test_instrument_item.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-latency-record-test framework.workbench_selection.latency_record tests/workbench_selection/test_latency_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-media-item-test framework.workbench_selection.media_item tests/workbench_selection/test_media_item.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-merge-record-test framework.workbench_selection.merge_record tests/workbench_selection/test_merge_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-metric-record-test framework.workbench_selection.metric_record tests/workbench_selection/test_metric_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-multi-selection-test framework.workbench_selection.multi_selection tests/workbench_selection/test_multi_selection.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-navigation-record-test framework.workbench_selection.navigation_record tests/workbench_selection/test_navigation_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-node-capability-test framework.workbench_selection.node_capability tests/workbench_selection/test_node_capability.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-node-identity-test framework.workbench_selection.node_identity tests/workbench_selection/test_node_identity.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-node-parent-test framework.workbench_selection.node_parent tests/workbench_selection/test_node_parent.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-node-path-test framework.workbench_selection.node_path tests/workbench_selection/test_node_path.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-open-record-test framework.workbench_selection.open_record tests/workbench_selection/test_open_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-panel-binding-test framework.workbench_selection.panel_binding tests/workbench_selection/test_panel_binding.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-permission-record-test framework.workbench_selection.permission_record tests/workbench_selection/test_permission_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-policy-record-test framework.workbench_selection.policy_record tests/workbench_selection/test_policy_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-preview-record-test framework.workbench_selection.preview_record tests/workbench_selection/test_preview_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-producer-record-test framework.workbench_selection.producer_record tests/workbench_selection/test_producer_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-producer-state-test framework.workbench_selection.producer_state tests/workbench_selection/test_producer_state.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-profile-record-test framework.workbench_selection.profile_record tests/workbench_selection/test_profile_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-project-node-test framework.workbench_selection.project_node tests/workbench_selection/test_project_node.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-provider-record-test framework.workbench_selection.provider_record tests/workbench_selection/test_provider_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-provider-state-test framework.workbench_selection.provider_state tests/workbench_selection/test_provider_state.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-publication-record-test framework.workbench_selection.publication_record tests/workbench_selection/test_publication_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-query-record-test framework.workbench_selection.query_record tests/workbench_selection/test_query_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-rate-record-test framework.workbench_selection.rate_record tests/workbench_selection/test_rate_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-refresh-record-test framework.workbench_selection.refresh_record tests/workbench_selection/test_refresh_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-rejection-record-test framework.workbench_selection.rejection_record tests/workbench_selection/test_rejection_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-remediation-record-test framework.workbench_selection.remediation_record tests/workbench_selection/test_remediation_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-resolver-test framework.workbench_selection.resolver tests/workbench_selection/test_resolver.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-resolver-record-test framework.workbench_selection.resolver_record tests/workbench_selection/test_resolver_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-result-record-test framework.workbench_selection.result_record tests/workbench_selection/test_result_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-reveal-record-test framework.workbench_selection.reveal_record tests/workbench_selection/test_reveal_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-schema-record-test framework.workbench_selection.schema_record tests/workbench_selection/test_schema_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-search-record-test framework.workbench_selection.search_record tests/workbench_selection/test_search_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-selection-test framework.workbench_selection.selection tests/workbench_selection/test_selection.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-selection-origin-test framework.workbench_selection.selection_origin tests/workbench_selection/test_selection_origin.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-selection-set-test framework.workbench_selection.selection_set tests/workbench_selection/test_selection_set.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-selection-state-test framework.workbench_selection.selection_state tests/workbench_selection/test_selection_state.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-session-record-test framework.workbench_selection.session_record tests/workbench_selection/test_session_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-snapshot-record-test framework.workbench_selection.snapshot_record tests/workbench_selection/test_snapshot_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-source-branch-test framework.workbench_selection.source_branch tests/workbench_selection/test_source_branch.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-source-change-test framework.workbench_selection.source_change tests/workbench_selection/test_source_change.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-source-commit-test framework.workbench_selection.source_commit tests/workbench_selection/test_source_commit.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-stale-record-test framework.workbench_selection.stale_record tests/workbench_selection/test_stale_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-statistics-record-test framework.workbench_selection.statistics_record tests/workbench_selection/test_statistics_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-terminal-item-test framework.workbench_selection.terminal_item tests/workbench_selection/test_terminal_item.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-test-item-test framework.workbench_selection.test_item tests/workbench_selection/test_test_item.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-test-result-test framework.workbench_selection.test_result tests/workbench_selection/test_test_result.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-trace-record-test framework.workbench_selection.trace_record tests/workbench_selection/test_trace_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-trade-item-test framework.workbench_selection.trade_item tests/workbench_selection/test_trade_item.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-types-test framework.workbench_selection.types tests/workbench_selection/test_types.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-validation-record-test framework.workbench_selection.validation_record tests/workbench_selection/test_validation_record.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-workspace-binding-test framework.workbench_selection.workspace_binding tests/workbench_selection/test_workspace_binding.c)
    umicom_add_workbench_selection_test(umicom-workbench-selection-workspace-item-test framework.workbench_selection.workspace_item tests/workbench_selection/test_workspace_item.c)
endif()

message(STATUS "Umicom structured workbench selection platform enabled")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkbenchSelectionProviderPlatform.cmake")
