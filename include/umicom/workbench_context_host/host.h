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
/**
 * Represent the workbench context host config data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextHostConfig {
    uint32_t structure_size;
    const char *host_id;
    const char *application_id;
    const char *observer_panel_id;
} UmiWorkbenchContextHostConfig;
/**
 * Represent the workbench context host data shared with callers of this public contract.
 */
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
/**
 * Provide the workbench context host config default operation used by this module and its
 * client applications.
 */
UmiWorkbenchContextHostConfig umi_workbench_context_host_config_default(void);
/**
 * Initialise workbench context host from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_context_host_create(
    const UmiWorkbenchContextHostConfig *config,
    UmiWorkbenchContextLinkService *link_service,
    UmiWorkbenchContextHost **out_host);
/**
 * Release or reset state held by workbench context host so the same storage can be reused
 * safely.
 */
void umi_workbench_context_host_destroy(UmiWorkbenchContextHost *host);
/**
 * Provide the workbench context host apply profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_apply_profile(
    UmiWorkbenchContextHost *host,
    const UmiWorkbenchContextHostProfile *profile);
/**
 * Provide the workbench context host set active group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_set_active_group(
    UmiWorkbenchContextHost *host,const char *group_id);
/**
 * Provide the workbench context host assign endpoint group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_assign_endpoint_group(
    UmiWorkbenchContextHost *host,
    const char *endpoint_id,
    const char *group_id,
    UmiWorkbenchContextLinkMode mode);
/**
 * Provide the workbench context host active group operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_host_active_group(
    const UmiWorkbenchContextHost *host);
/**
 * Provide the workbench context host publish operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_context_host_publish(
    UmiWorkbenchContextHost *host,
    const char *group_id,
    const char *source_panel_id,
    const UmiContextPayload *payload,
    uint64_t now_ms);
/**
 * Provide the workbench context host pop delivery operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_pop_delivery(
    UmiWorkbenchContextHost *host,
    const char *endpoint_id,
    UmiWorkbenchContextHostDelivery *out_delivery);
/**
 * Provide the workbench context host clear inbox operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_clear_inbox(
    UmiWorkbenchContextHost *host,const char *endpoint_id);
/**
 * Provide the workbench context host observe operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_context_host_observe(
    UmiWorkbenchContextHost *host,
    const UmiWorkbenchContextHostObservation *observation);
/**
 * Provide the workbench context host observe workbench operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_observe_workbench(
    UmiWorkbenchContextHost *host,
    const UmiUiWorkbench *workbench,
    uint64_t now_ms);
/**
 * Provide the workbench context host back operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_context_host_back(
    UmiWorkbenchContextHost *host,uint64_t now_ms);
/**
 * Provide the workbench context host forward operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_context_host_forward(
    UmiWorkbenchContextHost *host,uint64_t now_ms);
/**
 * Provide the workbench context host pin operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_context_host_pin(
    UmiWorkbenchContextHost *host,const char *pin_id,uint64_t now_ms);
/**
 * Provide the workbench context host set suspended operation used by this module and its
 * client applications.
 */
void umi_workbench_context_host_set_suspended(
    UmiWorkbenchContextHost *host,bool suspended);
#ifdef __cplusplus
}
#endif
#endif
