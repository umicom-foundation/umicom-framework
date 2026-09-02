#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWorkbenchContextEventPlatform.cmake
#
# PURPOSE:
#   Register real workbench interaction normalisation, event queueing,
#   coalescing, typed-context mapping and Studio/Trader-ready publication.
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

set(UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/..")

# Configure the optional target only when its feature has created it.
if(TARGET umicom_workbench_context_event)
    return()
endif()

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_workbench_context_host)
    message(FATAL_ERROR
        "Workbench Context Event requires Umicom::workbench_context_host")
endif()

add_library(umicom_workbench_context_event STATIC
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/account_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/ai_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/application_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/application_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/audit_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/backpressure_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/bookmark.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/builders.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/capability.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/caret_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/causation_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/coalescer.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/coalescing_policy.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/command.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/consumer_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/context_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/controller_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/correlation_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/counter.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/debug_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/dedupe_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/delivery_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/delivery_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/diagnostic.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/diagnostic_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/duplicate_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/editor_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/endpoint_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/envelope_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/event.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/event_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/event_group_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/event_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/failure_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/filter.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/group_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/health_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/history_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/instrument_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/journal_entry.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/latency_sample.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/media_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/metric.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/observer_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/panel_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/payload_mapper.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/permission.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/policy.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/processing_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/producer_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/profile.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/profile_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/project_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/query.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/queue.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/queue_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/rate_sample.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/rejection_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/replay_request.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/retry_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/route.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/route_hop.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/selection_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/service.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/session_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/snapshot_item.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/source_control_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/source_descriptor.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/source_registry.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/source_state.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/subscription.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/terminal_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/test_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/throttle_policy.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/trace.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/trade_record.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/types.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/workspace_binding.c"
    "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/src/workbench_context_event/workspace_record.c"
)

add_library(Umicom::workbench_context_event ALIAS
    umicom_workbench_context_event)
set_target_properties(umicom_workbench_context_event PROPERTIES
    EXPORT_NAME workbench_context_event
)

target_include_directories(umicom_workbench_context_event PUBLIC
    $<BUILD_INTERFACE:${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(umicom_workbench_context_event PUBLIC
    Umicom::workbench_context_host
)

# Use the shared build helper when it is available from the parent composition.
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_workbench_context_event)
endif()
# Use the shared build helper when it is available from the parent composition.
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_workbench_context_event)
endif()

# Configure the optional target only when its feature has created it.
if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE
        Umicom::workbench_context_event
    )
endif()

install(
    TARGETS umicom_workbench_context_event
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY
        "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/include/umicom/workbench_context_event"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom"
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add workbench context event test build helper so parent and application
    # projects apply one consistent rule.
    function(umicom_add_workbench_context_event_test target test_name source)
        add_executable("${target}"
            "${UMICOM_WORKBENCH_CONTEXT_EVENT_FRAMEWORK_ROOT}/${source}"
        )
        target_link_libraries("${target}" PRIVATE
            Umicom::workbench_context_event
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
            LABELS "framework;workbench-context-event"
        )
    endfunction()

    umicom_add_workbench_context_event_test(umicom-workbench-context-event-account-record-test framework.workbench_context_event.account_record tests/workbench_context_event/test_account_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-ai-record-test framework.workbench_context_event.ai_record tests/workbench_context_event/test_ai_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-application-binding-test framework.workbench_context_event.application_binding tests/workbench_context_event/test_application_binding.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-application-record-test framework.workbench_context_event.application_record tests/workbench_context_event/test_application_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-audit-record-test framework.workbench_context_event.audit_record tests/workbench_context_event/test_audit_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-backpressure-record-test framework.workbench_context_event.backpressure_record tests/workbench_context_event/test_backpressure_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-bookmark-test framework.workbench_context_event.bookmark tests/workbench_context_event/test_bookmark.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-builders-test framework.workbench_context_event.builders tests/workbench_context_event/test_builders.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-capability-test framework.workbench_context_event.capability tests/workbench_context_event/test_capability.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-caret-record-test framework.workbench_context_event.caret_record tests/workbench_context_event/test_caret_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-causation-record-test framework.workbench_context_event.causation_record tests/workbench_context_event/test_causation_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-coalescer-test framework.workbench_context_event.coalescer tests/workbench_context_event/test_coalescer.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-coalescing-policy-test framework.workbench_context_event.coalescing_policy tests/workbench_context_event/test_coalescing_policy.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-command-test framework.workbench_context_event.command tests/workbench_context_event/test_command.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-consumer-record-test framework.workbench_context_event.consumer_record tests/workbench_context_event/test_consumer_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-context-record-test framework.workbench_context_event.context_record tests/workbench_context_event/test_context_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-controller-state-test framework.workbench_context_event.controller_state tests/workbench_context_event/test_controller_state.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-correlation-record-test framework.workbench_context_event.correlation_record tests/workbench_context_event/test_correlation_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-counter-test framework.workbench_context_event.counter tests/workbench_context_event/test_counter.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-debug-record-test framework.workbench_context_event.debug_record tests/workbench_context_event/test_debug_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-dedupe-record-test framework.workbench_context_event.dedupe_record tests/workbench_context_event/test_dedupe_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-delivery-record-test framework.workbench_context_event.delivery_record tests/workbench_context_event/test_delivery_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-delivery-state-test framework.workbench_context_event.delivery_state tests/workbench_context_event/test_delivery_state.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-diagnostic-test framework.workbench_context_event.diagnostic tests/workbench_context_event/test_diagnostic.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-diagnostic-record-test framework.workbench_context_event.diagnostic_record tests/workbench_context_event/test_diagnostic_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-duplicate-record-test framework.workbench_context_event.duplicate_record tests/workbench_context_event/test_duplicate_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-editor-record-test framework.workbench_context_event.editor_record tests/workbench_context_event/test_editor_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-endpoint-binding-test framework.workbench_context_event.endpoint_binding tests/workbench_context_event/test_endpoint_binding.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-envelope-record-test framework.workbench_context_event.envelope_record tests/workbench_context_event/test_envelope_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-event-test framework.workbench_context_event.event tests/workbench_context_event/test_event.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-event-binding-test framework.workbench_context_event.event_binding tests/workbench_context_event/test_event_binding.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-event-group-record-test framework.workbench_context_event.event_group_record tests/workbench_context_event/test_event_group_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-event-record-test framework.workbench_context_event.event_record tests/workbench_context_event/test_event_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-failure-record-test framework.workbench_context_event.failure_record tests/workbench_context_event/test_failure_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-filter-test framework.workbench_context_event.filter tests/workbench_context_event/test_filter.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-group-binding-test framework.workbench_context_event.group_binding tests/workbench_context_event/test_group_binding.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-health-record-test framework.workbench_context_event.health_record tests/workbench_context_event/test_health_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-history-record-test framework.workbench_context_event.history_record tests/workbench_context_event/test_history_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-instrument-record-test framework.workbench_context_event.instrument_record tests/workbench_context_event/test_instrument_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-journal-entry-test framework.workbench_context_event.journal_entry tests/workbench_context_event/test_journal_entry.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-latency-sample-test framework.workbench_context_event.latency_sample tests/workbench_context_event/test_latency_sample.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-media-record-test framework.workbench_context_event.media_record tests/workbench_context_event/test_media_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-metric-test framework.workbench_context_event.metric tests/workbench_context_event/test_metric.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-observer-record-test framework.workbench_context_event.observer_record tests/workbench_context_event/test_observer_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-panel-binding-test framework.workbench_context_event.panel_binding tests/workbench_context_event/test_panel_binding.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-permission-test framework.workbench_context_event.permission tests/workbench_context_event/test_permission.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-policy-test framework.workbench_context_event.policy tests/workbench_context_event/test_policy.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-processing-state-test framework.workbench_context_event.processing_state tests/workbench_context_event/test_processing_state.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-producer-record-test framework.workbench_context_event.producer_record tests/workbench_context_event/test_producer_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-profile-test framework.workbench_context_event.profile tests/workbench_context_event/test_profile.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-profile-binding-test framework.workbench_context_event.profile_binding tests/workbench_context_event/test_profile_binding.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-project-record-test framework.workbench_context_event.project_record tests/workbench_context_event/test_project_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-query-test framework.workbench_context_event.query tests/workbench_context_event/test_query.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-queue-test framework.workbench_context_event.queue tests/workbench_context_event/test_queue.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-queue-state-test framework.workbench_context_event.queue_state tests/workbench_context_event/test_queue_state.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-rate-sample-test framework.workbench_context_event.rate_sample tests/workbench_context_event/test_rate_sample.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-rejection-record-test framework.workbench_context_event.rejection_record tests/workbench_context_event/test_rejection_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-replay-request-test framework.workbench_context_event.replay_request tests/workbench_context_event/test_replay_request.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-retry-record-test framework.workbench_context_event.retry_record tests/workbench_context_event/test_retry_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-route-test framework.workbench_context_event.route tests/workbench_context_event/test_route.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-route-hop-test framework.workbench_context_event.route_hop tests/workbench_context_event/test_route_hop.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-selection-record-test framework.workbench_context_event.selection_record tests/workbench_context_event/test_selection_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-session-record-test framework.workbench_context_event.session_record tests/workbench_context_event/test_session_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-snapshot-item-test framework.workbench_context_event.snapshot_item tests/workbench_context_event/test_snapshot_item.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-source-control-record-test framework.workbench_context_event.source_control_record tests/workbench_context_event/test_source_control_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-source-descriptor-test framework.workbench_context_event.source_descriptor tests/workbench_context_event/test_source_descriptor.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-source-registry-test framework.workbench_context_event.source_registry tests/workbench_context_event/test_source_registry.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-source-state-test framework.workbench_context_event.source_state tests/workbench_context_event/test_source_state.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-subscription-test framework.workbench_context_event.subscription tests/workbench_context_event/test_subscription.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-terminal-record-test framework.workbench_context_event.terminal_record tests/workbench_context_event/test_terminal_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-test-record-test framework.workbench_context_event.test_record tests/workbench_context_event/test_test_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-throttle-policy-test framework.workbench_context_event.throttle_policy tests/workbench_context_event/test_throttle_policy.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-trace-test framework.workbench_context_event.trace tests/workbench_context_event/test_trace.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-trade-record-test framework.workbench_context_event.trade_record tests/workbench_context_event/test_trade_record.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-types-test framework.workbench_context_event.types tests/workbench_context_event/test_types.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-workspace-binding-test framework.workbench_context_event.workspace_binding tests/workbench_context_event/test_workspace_binding.c)
    umicom_add_workbench_context_event_test(umicom-workbench-context-event-workspace-record-test framework.workbench_context_event.workspace_record tests/workbench_context_event/test_workspace_record.c)
endif()

message(STATUS "Umicom real workbench context event pipeline enabled")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkbenchContextSourcePlatform.cmake")
