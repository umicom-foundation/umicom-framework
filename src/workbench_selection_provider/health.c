/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/health.c
 *
 * PURPOSE:
 *   Evaluate structured selection provider availability and error/rejection pressure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/health.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the workbench selection provider health evaluate operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_provider_health_evaluate(
    const UmiWorkbenchSelectionProviderService *service,
    UmiWorkbenchSelectionProviderHealth *out_health)
{
    UmiWorkbenchSelectionProviderSnapshot snapshot;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_health == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(out_health, 0, sizeof(*out_health));
    status = umi_workbench_selection_provider_snapshot_build(
        service, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    out_health->provider_count = snapshot.provider_count;
    out_health->published_count =
        snapshot.metrics.selection_publish_count;
    out_health->rejection_count =
        snapshot.metrics.rejection_count;
    out_health->error_count =
        snapshot.metrics.error_count;

    /* Apply this branch only when its contract condition is satisfied. */
    if (snapshot.provider_count == 0U) {
        out_health->state =
            UMI_WORKBENCH_SELECTION_PROVIDER_HEALTH_UNAVAILABLE;
        (void)snprintf(
            out_health->message,
            sizeof(out_health->message),
            "No structured selection providers are registered");
    } else /* Apply this branch only when its contract condition is satisfied. */ if (snapshot.suspended) {
        out_health->state =
            UMI_WORKBENCH_SELECTION_PROVIDER_HEALTH_DEGRADED;
        (void)snprintf(
            out_health->message,
            sizeof(out_health->message),
            "Structured selection provider routing is suspended");
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (snapshot.metrics.error_count > 0U &&
               snapshot.metrics.error_count >=
                   snapshot.metrics.selection_publish_count) {
        out_health->state =
            UMI_WORKBENCH_SELECTION_PROVIDER_HEALTH_DEGRADED;
        (void)snprintf(
            out_health->message,
            sizeof(out_health->message),
            "Structured selection errors require attention");
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        out_health->state =
            UMI_WORKBENCH_SELECTION_PROVIDER_HEALTH_HEALTHY;
        (void)snprintf(
            out_health->message,
            sizeof(out_health->message),
            "%zu structured selection providers ready",
            snapshot.provider_count);
    }
    return UMI_STATUS_OK;
}
