#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomContextChannelPlatform.cmake
#
# PURPOSE:
#   Register typed context channels, cross-application panel contracts and the
#   GTK4 presentation adapter as a Framework-owned capability.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)
set(UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
if(TARGET umicom_context_channel)
    return()
endif()
add_library(umicom_context_channel STATIC
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/account.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/audit_record.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/binding.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/bookmark.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/bridge.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/capability.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/capability_service.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/channel.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/colour.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/command.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/controller.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/deduplication.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/delivery_receipt.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/diagnostic.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/diagnostics_service.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/event.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/filter_engine.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/filter_rule.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/group_member.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/group_service.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/history_entry.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/history_service.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/instrument.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/link_group.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/media.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/memory_store.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/metrics.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/panel.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/panel_context.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/panel_host.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/panel_instance.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/payload.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/permission.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/policy.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/policy_rule.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/project.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/provider.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/provider_state.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/query.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/recent_context.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/registry.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/route.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/route_trace.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/router.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/schema.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/selection.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/service.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/session.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/snapshot.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/source_location.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/subscription.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/trade.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/transformer_engine.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/transformer_rule.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/types.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/value.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/workspace.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_access.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_backpressure.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_bookmark_service.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_browser.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_codec.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_conflict.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_export.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_federation.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_filter.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_group_service.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_health.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_history_query.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_import.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_inspector.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_journal.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_merge.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_migration.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_panel_service.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_preference.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_profile.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_recovery.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_replay.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_schema_compatibility.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_search.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_security.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_session_service.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_snapshot_store.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_statistics.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_subscription_service.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_sync.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_throttle.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_trace.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_transform.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_transport.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/context_channel/context_validator.c"
)
add_library(Umicom::context_channel ALIAS umicom_context_channel)
set_target_properties(umicom_context_channel PROPERTIES EXPORT_NAME context_channel)
target_include_directories(umicom_context_channel PUBLIC
    $<BUILD_INTERFACE:${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_context_channel)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_context_channel)
endif()
if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE Umicom::context_channel)
endif()

add_library(umicom_cross_application_panel STATIC
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/activation.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/activation_service.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/bookmark.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/capability.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/capability_service.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/collection.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/collection_entry.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/command.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/command_router.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/context_binding.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/context_bridge.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/contribution.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/definition.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/diagnostic.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/diagnostics.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/event.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/factory.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/factory_record.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/focus_service.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/focus_state.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/host.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/host_slot.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/instance.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/layout_binding.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/layout_bridge.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/lifecycle.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/metric.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/metrics.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/permission.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/permission_service.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/persistence.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/query.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/registry.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/session.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/snapshot.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/types.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/view.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/workspace.c"
    "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/src/cross_application_panel/workspace_binding.c"
)
add_library(Umicom::cross_application_panel ALIAS umicom_cross_application_panel)
set_target_properties(umicom_cross_application_panel PROPERTIES EXPORT_NAME cross_application_panel)
target_include_directories(umicom_cross_application_panel PUBLIC
    $<BUILD_INTERFACE:${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(umicom_cross_application_panel PUBLIC Umicom::context_channel)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_cross_application_panel)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_cross_application_panel)
endif()
if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE Umicom::cross_application_panel)
endif()

# Context channels and cross-application panels are public dependencies of
# exported Framework targets. Keep them in the same export set so installed
# consumers receive a complete and resolvable UmicomFrameworkTargets package.
install(
    TARGETS
        umicom_context_channel
        umicom_cross_application_panel
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/include/umicom/cross_application_panel" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom")
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/adapters/gtk4/context_channel_badge.c"
        "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/adapters/gtk4/context_channel_diagnostics.c"
        "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/adapters/gtk4/context_channel_group_picker.c"
        "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/adapters/gtk4/context_channel_history.c"
        "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/adapters/gtk4/context_channel_inspector.c"
        "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/adapters/gtk4/context_channel_menu.c"
        "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/adapters/gtk4/context_channel_status.c"
        "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/adapters/gtk4/context_channel_toolbar.c"
        "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/adapters/gtk4/cross_application_panel_header.c"
        "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/adapters/gtk4/cross_application_panel_host.c"
)
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::context_channel)
endif()
install(DIRECTORY "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/include/umicom/context_channel" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom")
install(DIRECTORY "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/resources/schemas/context" DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/resources/schemas")
install(FILES "${UMICOM_CONTEXT_CHANNEL_FRAMEWORK_ROOT}/resources/themes/context-channels.css" DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/resources/themes")
message(STATUS "Umicom typed context channels and cross-application panels enabled")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomContextChannelTests.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomTestRuntimePlatform.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkRuntimePlatform.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkbenchContextLinkPlatform.cmake")
