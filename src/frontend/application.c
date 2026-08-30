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

/* BEGINNER NOTE:
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

UmiStatus umi_frontend_application_create(UmiFrontendApplication **out_service)
{
    UmiFrontendApplication *service;
    UmiStatus status = UMI_STATUS_OK;
    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiFrontendApplication *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->revision = 1U;
    if (status == UMI_STATUS_OK) status = umi_frontend_widget_tree_registry_create(&service->widgets);
    if (status == UMI_STATUS_OK) status = umi_frontend_signal_registry_create(&service->signals);
    if (status == UMI_STATUS_OK) status = umi_frontend_binding_registry_create(&service->bindings);
    if (status == UMI_STATUS_OK) status = umi_frontend_web_session_registry_create(&service->sessions);
    if (status == UMI_STATUS_OK) status = umi_frontend_transport_registry_create(&service->transports);
    if (status == UMI_STATUS_OK) status = umi_frontend_render_tree_registry_create(&service->render_nodes);
    if (status == UMI_STATUS_OK) status = umi_frontend_web_style_registry_create(&service->styles);
    if (status != UMI_STATUS_OK) {
        umi_frontend_application_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_frontend_application_destroy(UmiFrontendApplication *service)
{
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

UmiStatus umi_frontend_application_snapshot(const UmiFrontendApplication *service, UmiFrontendApplicationSnapshot *out_snapshot)
{
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

UmiFrontendWidgetRegistry *umi_frontend_application_widgets(UmiFrontendApplication *service)
{
    return service != NULL ? service->widgets : NULL;
}

UmiFrontendSignalRegistry *umi_frontend_application_signals(UmiFrontendApplication *service)
{
    return service != NULL ? service->signals : NULL;
}

UmiFrontendBindingRegistry *umi_frontend_application_bindings(UmiFrontendApplication *service)
{
    return service != NULL ? service->bindings : NULL;
}

UmiFrontendSessionRegistry *umi_frontend_application_sessions(UmiFrontendApplication *service)
{
    return service != NULL ? service->sessions : NULL;
}

UmiFrontendTransportRegistry *umi_frontend_application_transports(UmiFrontendApplication *service)
{
    return service != NULL ? service->transports : NULL;
}

UmiFrontendRenderNodeRegistry *umi_frontend_application_render_nodes(UmiFrontendApplication *service)
{
    return service != NULL ? service->render_nodes : NULL;
}

UmiFrontendStyleRegistry *umi_frontend_application_styles(UmiFrontendApplication *service)
{
    return service != NULL ? service->styles : NULL;
}
