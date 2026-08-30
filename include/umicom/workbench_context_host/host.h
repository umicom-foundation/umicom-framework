/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/host.h
 *
 * PURPOSE:
 *   Own endpoint/inbox state around a canonical context-link service and expose application-facing publication/consumption.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_HOST_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_HOST_H
#include "umicom/workbench_context_host/dispatcher.h"
#include "umicom/workbench_context_host/observer.h"
#include "umicom/workbench_context_host/profile_apply.h"
#include "umicom/workbench_context_host/payload.h"
#include "umicom/workbench_context_host/workbench_adapter.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostConfig {
    uint32_t structure_size;
    const char *host_id;
    const char *application_id;
    const char *observer_panel_id;
} UmiWorkbenchContextHostConfig;
typedef struct UmiWorkbenchContextHost {
    char host_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char observer_panel_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char active_group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextLinkService *link_service;
    UmiWorkbenchContextHostEndpointRegistry endpoints;
    UmiWorkbenchContextHostInboxRegistry inboxes;
    UmiWorkbenchContextHostObserver observer;
    UmiWorkbenchContextHostMetrics metrics;
    uint64_t revision;
    bool suspended;
} UmiWorkbenchContextHost;
UmiWorkbenchContextHostConfig umi_workbench_context_host_config_default(void);
UmiStatus umi_workbench_context_host_create(
    const UmiWorkbenchContextHostConfig *config,
    UmiWorkbenchContextLinkService *link_service,
    UmiWorkbenchContextHost **out_host);
void umi_workbench_context_host_destroy(UmiWorkbenchContextHost *host);
UmiStatus umi_workbench_context_host_apply_profile(
    UmiWorkbenchContextHost *host,
    const UmiWorkbenchContextHostProfile *profile);
UmiStatus umi_workbench_context_host_set_active_group(
    UmiWorkbenchContextHost *host,const char *group_id);
UmiStatus umi_workbench_context_host_assign_endpoint_group(
    UmiWorkbenchContextHost *host,
    const char *endpoint_id,
    const char *group_id,
    UmiWorkbenchContextLinkMode mode);
const char *umi_workbench_context_host_active_group(
    const UmiWorkbenchContextHost *host);
UmiStatus umi_workbench_context_host_publish(
    UmiWorkbenchContextHost *host,
    const char *group_id,
    const char *source_panel_id,
    const UmiContextPayload *payload,
    uint64_t now_ms);
UmiStatus umi_workbench_context_host_pop_delivery(
    UmiWorkbenchContextHost *host,
    const char *endpoint_id,
    UmiWorkbenchContextHostDelivery *out_delivery);
UmiStatus umi_workbench_context_host_clear_inbox(
    UmiWorkbenchContextHost *host,const char *endpoint_id);
UmiStatus umi_workbench_context_host_observe(
    UmiWorkbenchContextHost *host,
    const UmiWorkbenchContextHostObservation *observation);
UmiStatus umi_workbench_context_host_observe_workbench(
    UmiWorkbenchContextHost *host,
    const UmiUiWorkbench *workbench,
    uint64_t now_ms);
UmiStatus umi_workbench_context_host_back(
    UmiWorkbenchContextHost *host,uint64_t now_ms);
UmiStatus umi_workbench_context_host_forward(
    UmiWorkbenchContextHost *host,uint64_t now_ms);
UmiStatus umi_workbench_context_host_pin(
    UmiWorkbenchContextHost *host,const char *pin_id,uint64_t now_ms);
void umi_workbench_context_host_set_suspended(
    UmiWorkbenchContextHost *host,bool suspended);
#ifdef __cplusplus
}
#endif
#endif
