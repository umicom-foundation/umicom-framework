#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomServiceCppConsumer.cmake
#
# PURPOSE:
#   Qualify direct C++ inclusion and C symbol resolution for selected reusable Framework service components.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------


include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomCAbiConsumerChecks.cmake")

# Component arguments are explicit. Missing or unknown components must fail
# configuration instead of silently converting absent coverage into a green run.
function(umicom_add_service_cpp_checks target_prefix include_root)
    set(selected ${ARGN})
    set(supported codeguard data desktop developer finance frontend integration observability security
        ibkr_connection ibkr_connection_gtk4)
    if(NOT selected)
        message(FATAL_ERROR "Select at least one service component for C++ qualification")
    endif()
    list(REMOVE_DUPLICATES selected)
    foreach(component IN LISTS selected)
        if(NOT component IN_LIST supported)
            message(FATAL_ERROR "No service C++ contract group named ${component}")
        endif()
        if(NOT TARGET "Umicom::${component}")
            message(FATAL_ERROR "Service consumer requires exported Umicom::${component}")
        endif()
    endforeach()
    set(contracts
        umicom/broker_connectivity/news_providers.h|UmiIbkrNewsProvidersCopy|ibkr_connection
        umicom/broker_connectivity/gateway_gtk4.h|UmiIbkrGatewayGtkWidget|ibkr_connection_gtk4
        umicom/codeguard/api_baseline.h|umi_codeguard_api_baseline_init|codeguard
        umicom/codeguard/architecture_policy.h|umi_codeguard_architecture_policy_default|codeguard
        umicom/codeguard/duplicate_evidence.h|umi_codeguard_duplicate_candidate_build|codeguard
        umicom/codeguard/evidence.h|umi_codeguard_evidence_add|codeguard
        umicom/codeguard/memory_audit.h|umi_codeguard_memory_audit_line|codeguard
        umicom/codeguard/portability_audit.h|umi_codeguard_portability_is_adapter_path|codeguard
        umicom/codeguard/quality_catalogue.h|umi_codeguard_quality_catalogue_count|codeguard
        umicom/codeguard/quality_evaluation.h|umi_codeguard_quality_policy_default|codeguard
        umicom/codeguard/quality_session.h|umi_codeguard_quality_session_init|codeguard
        umicom/codeguard/quality_types.h|umi_codeguard_quality_copy|codeguard
        umicom/codeguard/remediation_plan.h|umi_codeguard_remediation_plan_build|codeguard
        umicom/codeguard/unsafe_api.h|umi_codeguard_unsafe_api_rule_count|codeguard
        umicom/data/backup_catalog.h|umi_database_backup_catalog_init|data
        umicom/data/connection_profile.h|umi_database_connection_registry_init|data
        umicom/data/database_explorer.h|umi_database_explorer_init|data
        umicom/data/explorer_transaction.h|umi_database_explorer_transaction_init|data
        umicom/data/explorer_types.h|umi_database_copy_text|data
        umicom/data/migration_catalog.h|umi_database_migration_catalog_init|data
        umicom/data/query_history.h|umi_database_query_history_init|data
        umicom/data/query_policy.h|umi_database_query_policy_default|data
        umicom/data/query_session.h|umi_database_query_session_init|data
        umicom/data/schema_catalog.h|umi_database_schema_catalog_init|data
        umicom/data/transfer_plan.h|umi_database_transfer_plan_init|data
        umicom/desktop/context_links.h|umi_desktop_context_links_init|desktop
        umicom/desktop/layout_catalogue.h|umi_desktop_layout_catalogue_init|desktop
        umicom/desktop/layout_designer.h|umi_desktop_layout_designer_create|desktop
        umicom/desktop/layout_tabs.h|umi_desktop_layout_tabs_init|desktop
        umicom/desktop/master_authority.h|umi_master_controller_install_desktop_authority|desktop
        umicom/desktop/monitor_canvas.h|umi_desktop_monitor_canvas_build|desktop
        umicom/desktop/monitor_topology.h|umi_desktop_monitor_topology_init|desktop
        umicom/desktop/persistence.h|umi_desktop_persistence_encode|desktop
        umicom/desktop/runtime.h|umi_desktop_runtime_create|desktop
        umicom/desktop/shell_actions.h|umi_desktop_shell_action_count|desktop
        umicom/desktop/snap_engine.h|umi_desktop_snap_policy_init|desktop
        umicom/desktop/ui_bridge.h|umi_desktop_seed_workspace_customisation|desktop
        umicom/desktop/views.h|umi_desktop_overview_view_create|desktop
        umicom/desktop/window_manager.h|umi_desktop_window_manager_init|desktop
        umicom/finance/treasury/liquidity_profile.h|umi_treasury_liquidity_profile_init|finance
        umicom/finance/treasury/treasury_service.h|umi_treasury_treasury_service_init|finance
        umicom/frontend/build_profile.h|umi_frontend_build_profiles_init|frontend
        umicom/frontend/console_log.h|umi_frontend_console_log_init|frontend
        umicom/frontend/device_registry.h|umi_frontend_device_registry_init|frontend
        umicom/frontend/dev_server.h|umi_frontend_dev_server_config_default|frontend
        umicom/frontend/document_store.h|umi_frontend_document_store_init|frontend
        umicom/frontend/dom_inspector.h|umi_frontend_dom_inspector_init|frontend
        umicom/frontend/frontend_studio.h|umi_frontend_studio_init|frontend
        umicom/frontend/hot_reload.h|umi_frontend_hot_reload_init|frontend
        umicom/frontend/network_log.h|umi_frontend_network_log_init|frontend
        umicom/frontend/preview_session.h|umi_frontend_preview_session_init|frontend
        umicom/frontend/quality_audit.h|umi_frontend_quality_budget_default|frontend
        umicom/integration/api_contract.h|umi_integration_api_contract_init|integration
        umicom/integration/application.h|umi_integration_application_init|integration
        umicom/integration/bridge.h|umi_integration_bridge_valid|integration
        umicom/integration/catalogue.h|umi_integration_catalogue_count|integration
        umicom/integration/connection_profile.h|umi_integration_connection_profile_validate|integration
        umicom/integration/contract.h|umi_integration_contract_compatible|integration
        umicom/integration/designer_fabric.h|umi_integration_designer_fabric_init|integration
        umicom/integration/designer_types.h|umi_integration_designer_copy|integration
        umicom/integration/dispatcher.h|umi_integration_dispatcher_init|integration
        umicom/integration/endpoint.h|umi_integration_endpoint_validate|integration
        umicom/integration/execution_policy.h|umi_integration_execution_policy_default|integration
        umicom/integration/field_mapping.h|umi_integration_field_mapping_init|integration
        umicom/integration/health.h|umi_integration_health_from_plan|integration
        umicom/integration/launch_plan.h|umi_integration_launch_plan_build|integration
        umicom/integration/presence.h|umi_integration_presence_init|integration
        umicom/integration/registry.h|umi_integration_registry_init|integration
        umicom/integration/request_plan.h|umi_integration_request_plan_build|integration
        umicom/integration/resolver.h|umi_integration_resolve|integration
        umicom/integration/route.h|umi_integration_route_matches|integration
        umicom/integration/router.h|umi_integration_router_init|integration
        umicom/integration/run_history.h|umi_integration_run_history_append|integration
        umicom/integration/schema_catalogue.h|umi_integration_schema_catalogue_register|integration
        umicom/integration/session.h|umi_integration_session_init|integration
        umicom/integration/simulator.h|umi_integration_simulate|integration
        umicom/integration/suite.h|umi_integration_suite_init|integration
        umicom/integration/suite_runtime.h|umi_integration_suite_runtime_prepare|integration
        umicom/integration/topology.h|umi_integration_topology_init|integration
        umicom/integration/types.h|umi_integration_application_state_text|integration
        umicom/integration/workflow.h|umi_integration_workflow_init|integration
        umicom/observability/benchmark_history.h|umi_operations_benchmark_history_add|observability
        umicom/observability/correlation.h|umi_operations_correlate|observability
        umicom/observability/crash_registry.h|umi_operations_crash_registry_add|observability
        umicom/observability/health_summary.h|umi_operations_health_summarise|observability
        umicom/observability/operations_catalogue.h|umi_operations_catalogue_count|observability
        umicom/observability/operations_centre.h|umi_operations_centre_init|observability
        umicom/observability/resilience_telemetry.h|umi_operations_resilience_capture|observability
        umicom/observability/resource_history.h|umi_operations_resource_history_add|observability
        umicom/security/approval.h|umi_security_approval_request|security
        umicom/security/identity_directory.h|umi_security_identity_directory_add|security
        umicom/security/package_trust.h|umi_security_signer_store_add|security
        umicom/security/permission_catalogue.h|umi_security_permission_catalogue_register|security
        umicom/security/redaction_policy.h|umi_security_redaction_policy_default|security
        umicom/security/role_assignment.h|umi_security_role_assignment_add|security
        umicom/security/role_policy.h|umi_security_role_policy_define|security
        umicom/security/secret_inventory.h|umi_security_secret_reference_parse|security
        umicom/security/security_audit.h|umi_security_audit_append|security
        umicom/security/security_catalogue.h|umi_security_catalogue_count|security
        umicom/security/security_centre.h|umi_security_governance_centre_init|security
        umicom/security/workspace_policy.h|umi_security_workspace_policy_evaluate|security
        umicom/studio_runtime/activation_profiles/live_preview.h|umi_studio_activation_profile_live_preview|developer
        umicom/studio_runtime/surface_profiles/live_preview.h|umi_studio_surface_profile_live_preview|developer

    )
    set(check_targets)
    foreach(contract IN LISTS contracts)
        string(REPLACE "|" ";" parts "${contract}")
        list(GET parts 0 header)
        list(GET parts 1 symbol)
        list(GET parts 2 component)
        if(NOT component IN_LIST selected)
            continue()
        endif()
        string(REGEX REPLACE "^umicom/|[.]h$" "" key "${header}")
        string(REPLACE "/" "." key "${key}")
        set(target "${target_prefix}-header-${key}")
        umicom_add_c_abi_header_check("${target}" "framework.sdk.cpp_service.${key}"
            "${header}" "${symbol}" "Umicom::${component}" "${include_root}")
        list(APPEND check_targets "${target}")
    endforeach()
    list(LENGTH check_targets count)
    message(STATUS "Registered ${count} C++ service header/link consumers for: ${selected}")
    add_custom_target("${target_prefix}-checks" DEPENDS ${check_targets})
endfunction()
