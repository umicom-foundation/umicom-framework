/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/application.c
 *
 * PURPOSE:
 *   Implement an owned server-driven frontend application model combining widgets, signals, bindings, sessions, transports, render nodes and styles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation owns only its documented child models. Product-specific
 * business data and toolkit objects remain outside this reusable Framework layer.
 */
#include "umicom/frontend/application.h"

#include <stdlib.h>
#include <string.h>

struct UmiFrontendApplication {
    UmiFrontendWidgetRegistry *widgets;
    UmiFrontendSignalRegistry *signals;
    UmiFrontendBindingRegistry *bindings;
    UmiFrontendSessionRegistry *sessions;
    UmiFrontendTransportRegistry *transports;
    UmiFrontendRenderNodeRegistry *render_nodes;
    UmiFrontendStyleRegistry *styles;
    uint64_t revision;
};

/*
 * Initialise frontend application from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_frontend_application_create(UmiFrontendApplication **out_service)
{
    UmiFrontendApplication *service;
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiFrontendApplication *)calloc(1U, sizeof(*service));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->revision = 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_frontend_widget_tree_registry_create(&service->widgets);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_frontend_signal_registry_create(&service->signals);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_frontend_binding_registry_create(&service->bindings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_frontend_web_session_registry_create(&service->sessions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_frontend_transport_registry_create(&service->transports);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_frontend_render_tree_registry_create(&service->render_nodes);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_frontend_web_style_registry_create(&service->styles);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_frontend_application_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by frontend application so the same storage can be reused
 * safely.
 */
void umi_frontend_application_destroy(UmiFrontendApplication *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    umi_frontend_web_style_registry_destroy(service->styles);
    umi_frontend_render_tree_registry_destroy(service->render_nodes);
    umi_frontend_transport_registry_destroy(service->transports);
    umi_frontend_web_session_registry_destroy(service->sessions);
    umi_frontend_binding_registry_destroy(service->bindings);
    umi_frontend_signal_registry_destroy(service->signals);
    umi_frontend_widget_tree_registry_destroy(service->widgets);
    free(service);
}

/*
 * Provide the frontend application snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_application_snapshot(const UmiFrontendApplication *service, UmiFrontendApplicationSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 1U;
    out_snapshot->widgets_count = umi_frontend_widget_tree_registry_count(service->widgets);
    out_snapshot->signals_count = umi_frontend_signal_registry_count(service->signals);
    out_snapshot->bindings_count = umi_frontend_binding_registry_count(service->bindings);
    out_snapshot->sessions_count = umi_frontend_web_session_registry_count(service->sessions);
    out_snapshot->transports_count = umi_frontend_transport_registry_count(service->transports);
    out_snapshot->render_nodes_count = umi_frontend_render_tree_registry_count(service->render_nodes);
    out_snapshot->styles_count = umi_frontend_web_style_registry_count(service->styles);
    out_snapshot->revision = service->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the frontend application widgets operation used by this module and its client
 * applications.
 */
UmiFrontendWidgetRegistry *umi_frontend_application_widgets(UmiFrontendApplication *service)
{
    return service != NULL ? service->widgets : NULL;
}

/*
 * Provide the frontend application signals operation used by this module and its client
 * applications.
 */
UmiFrontendSignalRegistry *umi_frontend_application_signals(UmiFrontendApplication *service)
{
    return service != NULL ? service->signals : NULL;
}

/*
 * Provide the frontend application bindings operation used by this module and its client
 * applications.
 */
UmiFrontendBindingRegistry *umi_frontend_application_bindings(UmiFrontendApplication *service)
{
    return service != NULL ? service->bindings : NULL;
}

/*
 * Provide the frontend application sessions operation used by this module and its client
 * applications.
 */
UmiFrontendSessionRegistry *umi_frontend_application_sessions(UmiFrontendApplication *service)
{
    return service != NULL ? service->sessions : NULL;
}

/*
 * Provide the frontend application transports operation used by this module and its client
 * applications.
 */
UmiFrontendTransportRegistry *umi_frontend_application_transports(UmiFrontendApplication *service)
{
    return service != NULL ? service->transports : NULL;
}

/*
 * Provide the frontend application render nodes operation used by this module and its
 * client applications.
 */
UmiFrontendRenderNodeRegistry *umi_frontend_application_render_nodes(UmiFrontendApplication *service)
{
    return service != NULL ? service->render_nodes : NULL;
}

/*
 * Provide the frontend application styles operation used by this module and its client
 * applications.
 */
UmiFrontendStyleRegistry *umi_frontend_application_styles(UmiFrontendApplication *service)
{
    return service != NULL ? service->styles : NULL;
}
