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


void umi_workbench_designer_metrics_init(UmiWorkbenchDesignerMetrics *metrics)
{
    if (metrics != NULL) (void)memset(metrics, 0, sizeof(*metrics));
}

void umi_workbench_designer_metrics_record_command(
    UmiWorkbenchDesignerMetrics *metrics,
    UmiStatus status,
    uint64_t duration_us,
    bool undo,
    bool redo)
{
    if (metrics == NULL) return;
    metrics->commands_executed += 1U;
    if (status != UMI_STATUS_OK) metrics->commands_failed += 1U;
    if (undo) metrics->undo_count += 1U;
    if (redo) metrics->redo_count += 1U;
    metrics->total_command_duration_us += duration_us;
    if (duration_us > metrics->maximum_command_duration_us) {
        metrics->maximum_command_duration_us = duration_us;
    }
    metrics->revision += 1U;
}

void umi_workbench_designer_metrics_record_save(
    UmiWorkbenchDesignerMetrics *metrics,
    UmiStatus status)
{
    if (metrics == NULL) return;
    metrics->saves_started += 1U;
    if (status == UMI_STATUS_OK) metrics->saves_completed += 1U;
    else metrics->saves_failed += 1U;
    metrics->revision += 1U;
}

double umi_workbench_designer_metrics_command_success_rate(
    const UmiWorkbenchDesignerMetrics *metrics)
{
    if (metrics == NULL || metrics->commands_executed == 0U) return 1.0;
    return (double)(metrics->commands_executed - metrics->commands_failed) /
        (double)metrics->commands_executed;
}

double umi_workbench_designer_metrics_average_command_duration_us(
    const UmiWorkbenchDesignerMetrics *metrics)
{
    if (metrics == NULL || metrics->commands_executed == 0U) return 0.0;
    return (double)metrics->total_command_duration_us /
        (double)metrics->commands_executed;
}
