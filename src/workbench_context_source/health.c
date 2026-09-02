/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/health.c
 *
 * PURPOSE:
 *   Evaluate source availability, suspension and rejection pressure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/health.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the workbench context source health evaluate operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_health_evaluate(
    const UmiWorkbenchContextSourceService *service,
    UmiWorkbenchContextSourceHealth *out_health)
{
    UmiWorkbenchContextSourceSnapshot snapshot;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_health == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(out_health, 0, sizeof(*out_health));
    status = umi_workbench_context_source_snapshot_build(
        service, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    out_health->registered_sources = snapshot.source_count;
    out_health->active_sources = snapshot.active_source_count;
    out_health->accepted_samples =
        snapshot.metrics.accepted_sample_count;
    out_health->rejected_samples =
        snapshot.metrics.rejected_sample_count;
    out_health->duplicate_samples =
        snapshot.metrics.duplicate_sample_count;
    out_health->throttled_samples =
        snapshot.metrics.throttled_sample_count;

    /* Apply this branch only when its contract condition is satisfied. */
    if (snapshot.source_count == 0U) {
        out_health->state =
            UMI_WORKBENCH_CONTEXT_SOURCE_HEALTH_UNAVAILABLE;
        (void)snprintf(
            out_health->message,
            sizeof(out_health->message),
            "No live interaction sources are registered");
    } else /* Apply this branch only when its contract condition is satisfied. */ if (snapshot.suspended) {
        out_health->state =
            UMI_WORKBENCH_CONTEXT_SOURCE_HEALTH_DEGRADED;
        (void)snprintf(
            out_health->message,
            sizeof(out_health->message),
            "Live interaction source routing is suspended");
    } else /* Apply this branch only when its contract condition is satisfied. */ if (snapshot.metrics.rejected_sample_count >
               snapshot.metrics.accepted_sample_count &&
               snapshot.metrics.rejected_sample_count > 0U) {
        out_health->state =
            UMI_WORKBENCH_CONTEXT_SOURCE_HEALTH_DEGRADED;
        (void)snprintf(
            out_health->message,
            sizeof(out_health->message),
            "Interaction rejection pressure exceeds accepted traffic");
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        out_health->state =
            UMI_WORKBENCH_CONTEXT_SOURCE_HEALTH_HEALTHY;
        (void)snprintf(
            out_health->message,
            sizeof(out_health->message),
            "%zu live interaction sources ready",
            snapshot.active_source_count);
    }
    return UMI_STATUS_OK;
}
