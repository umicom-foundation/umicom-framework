/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/metrics.c
 *
 * PURPOSE:
 *   Maintain saturating-style operational evidence and derived success and
 *   latency statistics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/metrics.h"
#include <string.h>


/*
 * Initialise workbench designer metrics from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_metrics_init(UmiWorkbenchDesignerMetrics *metrics)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics != NULL) (void)memset(metrics, 0, sizeof(*metrics));
}

/*
 * Provide the workbench designer metrics record command operation used by this module and
 * its client applications.
 */
void umi_workbench_designer_metrics_record_command(
    UmiWorkbenchDesignerMetrics *metrics,
    UmiStatus status,
    uint64_t duration_us,
    bool undo,
    bool redo)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL) return;
    metrics->commands_executed += 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) metrics->commands_failed += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (undo) metrics->undo_count += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (redo) metrics->redo_count += 1U;
    metrics->total_command_duration_us += duration_us;
    /* Apply this branch only when its contract condition is satisfied. */
    if (duration_us > metrics->maximum_command_duration_us) {
        metrics->maximum_command_duration_us = duration_us;
    }
    metrics->revision += 1U;
}

/*
 * Write workbench designer metrics record in its stable representation and report capacity
 * or input failures to the caller.
 */
void umi_workbench_designer_metrics_record_save(
    UmiWorkbenchDesignerMetrics *metrics,
    UmiStatus status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL) return;
    metrics->saves_started += 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) metrics->saves_completed += 1U;
    /* Use this fallback path when the earlier condition does not apply. */
    else metrics->saves_failed += 1U;
    metrics->revision += 1U;
}

/*
 * Provide the workbench designer metrics command success rate operation used by this
 * module and its client applications.
 */
double umi_workbench_designer_metrics_command_success_rate(
    const UmiWorkbenchDesignerMetrics *metrics)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL || metrics->commands_executed == 0U) return 1.0;
    return (double)(metrics->commands_executed - metrics->commands_failed) /
        (double)metrics->commands_executed;
}

/*
 * Provide the workbench designer metrics average command duration us operation used by
 * this module and its client applications.
 */
double umi_workbench_designer_metrics_average_command_duration_us(
    const UmiWorkbenchDesignerMetrics *metrics)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL || metrics->commands_executed == 0U) return 0.0;
    return (double)metrics->total_command_duration_us /
        (double)metrics->commands_executed;
}
