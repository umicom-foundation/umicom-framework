/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/run_history.c
 *
 * PURPOSE:
 *   Implement the run history behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Integration run history | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/run_history.h"
#include <string.h>

/*
 * Find integration run history while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiIntegrationRunRecord *umi_integration_run_history_find(const UmiIntegrationRunHistory *history,const char *run_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || run_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < history->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(history->items[index].run_id,run_id) == 0) return &history->items[index];
    return NULL;
}

/*
 * Add integration run history only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_integration_run_history_append(UmiIntegrationRunHistory *history,const UmiIntegrationSimulation *simulation)
{
    UmiIntegrationRunRecord record = {0};
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || simulation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_integration_run_history_find(history,simulation->run_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    status = umi_integration_designer_copy(record.run_id,sizeof(record.run_id),simulation->run_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(record.workflow_id,sizeof(record.workflow_id),simulation->workflow_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record.status = simulation->status;
    record.step_count = simulation->trace_count;
    record.attempted_requests = simulation->attempted_requests;
    history->next_ordinal += 1U;
    record.ordinal = history->next_ordinal;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count < UMI_INTEGRATION_DESIGNER_MAX_RUNS) {
        history->items[history->count] = record;
        history->count += 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)memmove(&history->items[0],&history->items[1],(UMI_INTEGRATION_DESIGNER_MAX_RUNS - 1U) * sizeof(history->items[0]));
        history->items[UMI_INTEGRATION_DESIGNER_MAX_RUNS - 1U] = record;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the integration run history metrics operation used by this module and its client
 * applications.
 */
void umi_integration_run_history_metrics(const UmiIntegrationRunHistory *history,UmiIntegrationRunMetrics *out_metrics)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_metrics == NULL) return;
    (void)memset(out_metrics,0,sizeof(*out_metrics));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return;
    out_metrics->total = history->count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < history->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (history->items[index].status == UMI_INTEGRATION_DESIGNER_RUN_SUCCEEDED) out_metrics->succeeded += 1U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (history->items[index].status == UMI_INTEGRATION_DESIGNER_RUN_FAILED) out_metrics->failed += 1U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (history->items[index].status == UMI_INTEGRATION_DESIGNER_RUN_CANCELLED) out_metrics->cancelled += 1U;
        out_metrics->attempted_requests += history->items[index].attempted_requests;
    }
}
