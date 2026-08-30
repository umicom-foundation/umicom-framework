/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/health.c
 *
 * PURPOSE:
 *   Evaluate host health from lifecycle state, endpoint inventory and delivery pressure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/health.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_workbench_context_host_health_evaluate(
    const UmiWorkbenchContextHost *host,
    UmiWorkbenchContextHostHealth *out_health)
{
    uint64_t dropped = 0U;
    size_t queued = 0U;
    size_t index;

    if (host == NULL || out_health == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(out_health, 0, sizeof(*out_health));

    for (index = 0U; index < host->inboxes.count; ++index) {
        queued += host->inboxes.items[index].count;
        dropped += host->inboxes.items[index].dropped_count;
    }
    out_health->endpoint_count = host->endpoints.count;
    out_health->queued_delivery_count = queued;
    out_health->dropped_delivery_count = dropped;

    if (host->suspended) {
        out_health->state =
            UMI_WORKBENCH_CONTEXT_HOST_HEALTH_DEGRADED;
        (void)snprintf(
            out_health->message,
            sizeof(out_health->message),
            "Context host is suspended");
    } else if (host->endpoints.count == 0U) {
        out_health->state =
            UMI_WORKBENCH_CONTEXT_HOST_HEALTH_UNAVAILABLE;
        (void)snprintf(
            out_health->message,
            sizeof(out_health->message),
            "No context-aware endpoints are registered");
    } else if (dropped > 0U) {
        out_health->state =
            UMI_WORKBENCH_CONTEXT_HOST_HEALTH_DEGRADED;
        (void)snprintf(
            out_health->message,
            sizeof(out_health->message),
            "Delivery pressure dropped %llu contexts",
            (unsigned long long)dropped);
    } else {
        out_health->state =
            UMI_WORKBENCH_CONTEXT_HOST_HEALTH_HEALTHY;
        (void)snprintf(
            out_health->message,
            sizeof(out_health->message),
            "Context host is healthy");
    }
    return UMI_STATUS_OK;
}
