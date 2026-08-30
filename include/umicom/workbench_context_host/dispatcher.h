/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/dispatcher.h
 *
 * PURPOSE:
 *   Translate link-service delivery batches into endpoint inbox deliveries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_DISPATCHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_DISPATCHER_H
#include "umicom/workbench_context_host/endpoint_registry.h"
#include "umicom/workbench_context_host/inbox_registry.h"
#include "umicom/workbench_context_host/metrics.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_workbench_context_host_dispatch(
    UmiWorkbenchContextHostEndpointRegistry *endpoints,
    UmiWorkbenchContextHostInboxRegistry *inboxes,
    UmiWorkbenchContextHostMetrics *metrics,
    const UmiWorkbenchContextLinkDeliveryBatch *batch,
    const char *source_panel_id,
    const UmiContextPayload *payload,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
