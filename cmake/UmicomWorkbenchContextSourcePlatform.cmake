#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWorkbenchContextSourcePlatform.cmake
#
# PURPOSE:
#   Register live editor/view interaction source contracts, source policy,
#   Studio/Trader profiles, regression tests and GTK4 native interaction hooks.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)

set(UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/..")

if(TARGET umicom_workbench_context_source)
    return()
endif()

if(NOT TARGET umicom_workbench_context_event)
    message(FATAL_ERROR
        "Workbench Context Source requires Umicom::workbench_context_event")
endif()

add_library(umicom_workbench_context_source STATIC
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/account_selection.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/activation_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/adapter_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/ai_selection.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/application_selection.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/application_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/audit_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/binding_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/capability.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/caret_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/coalesce_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/command.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/connection_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/context_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/controller.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/debug_location.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/definition.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/delivery_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/diagnostic.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/document_activation.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/document_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/duplicate_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/editor_location.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/editor_selection.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/event.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/event_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/failure_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/focus_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/group_hint.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/health.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/health_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/history_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/instrument_selection.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/media_selection.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/metric.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/model_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/panel_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/permission.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/policy.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/problem_selection.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/profile.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/profile_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/project_selection.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/projection_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/query.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/range_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/registry.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/resolver_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/route_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/sample.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/selection_metadata.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/service.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/session_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/signal_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/snapshot.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/source_control_selection.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/source_descriptor.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/source_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/studio_profile.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/subscription.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/terminal_selection.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/test_selection.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/throttle_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/trace.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/trade_selection.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/trading_profile.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/translator.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/trigger_policy.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/types.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/view_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/workspace_selection.c"
    "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/src/workbench_context_source/workspace_state.c"
)

add_library(Umicom::workbench_context_source ALIAS
    umicom_workbench_context_source)
set_target_properties(umicom_workbench_context_source PROPERTIES
    EXPORT_NAME workbench_context_source
)

target_include_directories(umicom_workbench_context_source PUBLIC
    $<BUILD_INTERFACE:${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(umicom_workbench_context_source PUBLIC
    Umicom::workbench_context_event
)

if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_workbench_context_source)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_workbench_context_source)
endif()

if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE
        Umicom::workbench_context_source
    )
endif()

if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/adapters/gtk4/context_interaction_gtk4.c"
    )
    target_link_libraries(umicom_ui_gtk4 PUBLIC
        Umicom::workbench_context_source
    )
endif()

install(
    TARGETS umicom_workbench_context_source
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY
        "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/include/umicom/workbench_context_source"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom"
)

install(
    FILES
        "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/resources/workbench-context-source-defaults.json"
        "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/resources/schemas/workbench-context-source.schema.json"
    DESTINATION
        "${CMAKE_INSTALL_DATADIR}/umicom/resources/workbench-context-source"
)

if(BUILD_TESTING)
    function(umicom_configure_workbench_context_source_test target test_name)
        target_link_libraries("${target}" PRIVATE
            Umicom::workbench_context_source
        )
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES
            LABELS "framework;workbench-context-source"
        )
    endfunction()

    function(umicom_add_workbench_context_source_test target test_name source)
        add_executable("${target}"
            "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/${source}"
        )
        umicom_configure_workbench_context_source_test(
            "${target}" "${test_name}")
    endfunction()

    function(umicom_add_workbench_context_source_integration_test
             target test_name source)
        add_executable("${target}"
            "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/${source}"
            "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/tests/workbench_context_host/test_support.c"
        )
        target_include_directories("${target}" PRIVATE
            "${UMICOM_WORKBENCH_CONTEXT_SOURCE_FRAMEWORK_ROOT}/tests/workbench_context_host"
        )
        umicom_configure_workbench_context_source_test(
            "${target}" "${test_name}")
    endfunction()

    umicom_add_workbench_context_source_test(umicom-workbench-context-source-account-selection-test framework.workbench_context_source.account_selection tests/workbench_context_source/test_account_selection.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-activation-state-test framework.workbench_context_source.activation_state tests/workbench_context_source/test_activation_state.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-adapter-binding-test framework.workbench_context_source.adapter_binding tests/workbench_context_source/test_adapter_binding.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-ai-selection-test framework.workbench_context_source.ai_selection tests/workbench_context_source/test_ai_selection.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-application-selection-test framework.workbench_context_source.application_selection tests/workbench_context_source/test_application_selection.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-application-state-test framework.workbench_context_source.application_state tests/workbench_context_source/test_application_state.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-audit-record-test framework.workbench_context_source.audit_record tests/workbench_context_source/test_audit_record.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-binding-test framework.workbench_context_source.binding tests/workbench_context_source/test_binding.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-binding-state-test framework.workbench_context_source.binding_state tests/workbench_context_source/test_binding_state.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-capability-test framework.workbench_context_source.capability tests/workbench_context_source/test_capability.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-caret-state-test framework.workbench_context_source.caret_state tests/workbench_context_source/test_caret_state.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-coalesce-record-test framework.workbench_context_source.coalesce_record tests/workbench_context_source/test_coalesce_record.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-command-test framework.workbench_context_source.command tests/workbench_context_source/test_command.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-connection-state-test framework.workbench_context_source.connection_state tests/workbench_context_source/test_connection_state.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-context-record-test framework.workbench_context_source.context_record tests/workbench_context_source/test_context_record.c)
    umicom_add_workbench_context_source_integration_test(umicom-workbench-context-source-controller-test framework.workbench_context_source.controller tests/workbench_context_source/test_controller.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-debug-location-test framework.workbench_context_source.debug_location tests/workbench_context_source/test_debug_location.c)
    umicom_add_workbench_context_source_integration_test(umicom-workbench-context-source-deduplication-test framework.workbench_context_source.deduplication tests/workbench_context_source/test_deduplication.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-definition-test framework.workbench_context_source.definition tests/workbench_context_source/test_definition.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-delivery-state-test framework.workbench_context_source.delivery_state tests/workbench_context_source/test_delivery_state.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-diagnostic-test framework.workbench_context_source.diagnostic tests/workbench_context_source/test_diagnostic.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-document-activation-test framework.workbench_context_source.document_activation tests/workbench_context_source/test_document_activation.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-document-state-test framework.workbench_context_source.document_state tests/workbench_context_source/test_document_state.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-duplicate-record-test framework.workbench_context_source.duplicate_record tests/workbench_context_source/test_duplicate_record.c)
    umicom_add_workbench_context_source_integration_test(umicom-workbench-context-source-dynamic-group-test framework.workbench_context_source.dynamic_group tests/workbench_context_source/test_dynamic_group.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-editor-location-test framework.workbench_context_source.editor_location tests/workbench_context_source/test_editor_location.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-editor-selection-test framework.workbench_context_source.editor_selection tests/workbench_context_source/test_editor_selection.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-event-test framework.workbench_context_source.event tests/workbench_context_source/test_event.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-event-position-hash-test framework.workbench_context_source.event_position_hash tests/workbench_context_source/test_event_position_hash.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-event-record-test framework.workbench_context_source.event_record tests/workbench_context_source/test_event_record.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-failure-record-test framework.workbench_context_source.failure_record tests/workbench_context_source/test_failure_record.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-focus-state-test framework.workbench_context_source.focus_state tests/workbench_context_source/test_focus_state.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-group-hint-test framework.workbench_context_source.group_hint tests/workbench_context_source/test_group_hint.c)
    umicom_add_workbench_context_source_integration_test(umicom-workbench-context-source-health-test framework.workbench_context_source.health tests/workbench_context_source/test_health.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-health-record-test framework.workbench_context_source.health_record tests/workbench_context_source/test_health_record.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-history-record-test framework.workbench_context_source.history_record tests/workbench_context_source/test_history_record.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-instrument-selection-test framework.workbench_context_source.instrument_selection tests/workbench_context_source/test_instrument_selection.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-media-selection-test framework.workbench_context_source.media_selection tests/workbench_context_source/test_media_selection.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-metric-test framework.workbench_context_source.metric tests/workbench_context_source/test_metric.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-model-binding-test framework.workbench_context_source.model_binding tests/workbench_context_source/test_model_binding.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-panel-state-test framework.workbench_context_source.panel_state tests/workbench_context_source/test_panel_state.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-permission-test framework.workbench_context_source.permission tests/workbench_context_source/test_permission.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-policy-test framework.workbench_context_source.policy tests/workbench_context_source/test_policy.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-problem-selection-test framework.workbench_context_source.problem_selection tests/workbench_context_source/test_problem_selection.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-profile-test framework.workbench_context_source.profile tests/workbench_context_source/test_profile.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-profile-binding-test framework.workbench_context_source.profile_binding tests/workbench_context_source/test_profile_binding.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-project-selection-test framework.workbench_context_source.project_selection tests/workbench_context_source/test_project_selection.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-projection-record-test framework.workbench_context_source.projection_record tests/workbench_context_source/test_projection_record.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-query-test framework.workbench_context_source.query tests/workbench_context_source/test_query.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-range-state-test framework.workbench_context_source.range_state tests/workbench_context_source/test_range_state.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-registry-test framework.workbench_context_source.registry tests/workbench_context_source/test_registry.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-resolver-record-test framework.workbench_context_source.resolver_record tests/workbench_context_source/test_resolver_record.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-route-record-test framework.workbench_context_source.route_record tests/workbench_context_source/test_route_record.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-sample-test framework.workbench_context_source.sample tests/workbench_context_source/test_sample.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-selection-metadata-test framework.workbench_context_source.selection_metadata tests/workbench_context_source/test_selection_metadata.c)
    umicom_add_workbench_context_source_integration_test(umicom-workbench-context-source-service-test framework.workbench_context_source.service tests/workbench_context_source/test_service.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-session-record-test framework.workbench_context_source.session_record tests/workbench_context_source/test_session_record.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-signal-binding-test framework.workbench_context_source.signal_binding tests/workbench_context_source/test_signal_binding.c)
    umicom_add_workbench_context_source_integration_test(umicom-workbench-context-source-snapshot-test framework.workbench_context_source.snapshot tests/workbench_context_source/test_snapshot.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-source-control-selection-test framework.workbench_context_source.source_control_selection tests/workbench_context_source/test_source_control_selection.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-source-descriptor-test framework.workbench_context_source.source_descriptor tests/workbench_context_source/test_source_descriptor.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-source-state-test framework.workbench_context_source.source_state tests/workbench_context_source/test_source_state.c)
    umicom_add_workbench_context_source_integration_test(umicom-workbench-context-source-studio-profile-test framework.workbench_context_source.studio_profile tests/workbench_context_source/test_studio_profile.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-subscription-test framework.workbench_context_source.subscription tests/workbench_context_source/test_subscription.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-terminal-selection-test framework.workbench_context_source.terminal_selection tests/workbench_context_source/test_terminal_selection.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-test-selection-test framework.workbench_context_source.test_selection tests/workbench_context_source/test_test_selection.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-throttle-record-test framework.workbench_context_source.throttle_record tests/workbench_context_source/test_throttle_record.c)
    umicom_add_workbench_context_source_integration_test(umicom-workbench-context-source-throttling-test framework.workbench_context_source.throttling tests/workbench_context_source/test_throttling.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-trace-test framework.workbench_context_source.trace tests/workbench_context_source/test_trace.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-trade-selection-test framework.workbench_context_source.trade_selection tests/workbench_context_source/test_trade_selection.c)
    umicom_add_workbench_context_source_integration_test(umicom-workbench-context-source-trading-profile-test framework.workbench_context_source.trading_profile tests/workbench_context_source/test_trading_profile.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-translator-editor-test framework.workbench_context_source.translator_editor tests/workbench_context_source/test_translator_editor.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-translator-instrument-test framework.workbench_context_source.translator_instrument tests/workbench_context_source/test_translator_instrument.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-trigger-policy-test framework.workbench_context_source.trigger_policy tests/workbench_context_source/test_trigger_policy.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-types-test framework.workbench_context_source.types tests/workbench_context_source/test_types.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-view-state-test framework.workbench_context_source.view_state tests/workbench_context_source/test_view_state.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-workspace-selection-test framework.workbench_context_source.workspace_selection tests/workbench_context_source/test_workspace_selection.c)
    umicom_add_workbench_context_source_test(umicom-workbench-context-source-workspace-state-test framework.workbench_context_source.workspace_state tests/workbench_context_source/test_workspace_state.c)
endif()

message(STATUS "Umicom live workbench interaction source platform enabled")
