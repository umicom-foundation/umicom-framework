/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/metrics.c
 *
 * PURPOSE:
 *   Implement persistence and synchronisation metrics without global state or frontend dependencies.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/metrics.h"
#include <string.h>



static void record_duration(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    uint64_t duration_us)
{
    metrics->snapshot.last_operation_duration_us = duration_us;
    if (duration_us >
        metrics->snapshot.maximum_operation_duration_us) {
        metrics->snapshot.maximum_operation_duration_us = duration_us;
    }
    metrics->snapshot.revision += 1U;
}

void umi_workbench_layout_metrics_init(
    UmiWorkbenchLayoutMetricsCollector *metrics)
{
    if (metrics == NULL) return;
    (void)memset(metrics, 0, sizeof(*metrics));
    metrics->structure_size = sizeof(*metrics);
    metrics->snapshot.structure_size =
        sizeof(metrics->snapshot);
    metrics->snapshot.revision = 1U;
}

void umi_workbench_layout_metrics_record_save(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    size_t bytes,
    uint64_t duration_us,
    UmiStatus status)
{
    if (metrics == NULL) return;
    metrics->snapshot.save_count += 1U;
    metrics->snapshot.bytes_written += (uint64_t)bytes;
    if (status != UMI_STATUS_OK) {
        metrics->snapshot.sync_failure_count += 1U;
    }
    record_duration(metrics, duration_us);
}

void umi_workbench_layout_metrics_record_load(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    size_t bytes,
    uint64_t duration_us,
    UmiStatus status)
{
    if (metrics == NULL) return;
    metrics->snapshot.load_count += 1U;
    metrics->snapshot.bytes_read += (uint64_t)bytes;
    if (status != UMI_STATUS_OK) {
        metrics->snapshot.sync_failure_count += 1U;
    }
    record_duration(metrics, duration_us);
}

void umi_workbench_layout_metrics_record_delete(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    uint64_t duration_us,
    UmiStatus status)
{
    if (metrics == NULL) return;
    metrics->snapshot.delete_count += 1U;
    if (status != UMI_STATUS_OK) {
        metrics->snapshot.sync_failure_count += 1U;
    }
    record_duration(metrics, duration_us);
}

void umi_workbench_layout_metrics_record_sync(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    bool success,
    size_t conflicts,
    uint64_t duration_us)
{
    if (metrics == NULL) return;
    metrics->snapshot.sync_plan_count += 1U;
    if (success) {
        metrics->snapshot.sync_success_count += 1U;
    } else {
        metrics->snapshot.sync_failure_count += 1U;
    }
    metrics->snapshot.conflict_count += (uint64_t)conflicts;
    record_duration(metrics, duration_us);
}

void umi_workbench_layout_metrics_record_outbox(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    bool delivered,
    bool retry)
{
    if (metrics == NULL) return;
    if (delivered) {
        metrics->snapshot.outbox_delivery_count += 1U;
    }
    if (retry) {
        metrics->snapshot.outbox_retry_count += 1U;
    }
    metrics->snapshot.revision += 1U;
}

void umi_workbench_layout_metrics_record_offline_replay(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    bool success)
{
    if (metrics == NULL) return;
    metrics->snapshot.offline_replay_count += 1U;
    if (!success) metrics->snapshot.sync_failure_count += 1U;
    metrics->snapshot.revision += 1U;
}

UmiStatus umi_workbench_layout_metrics_snapshot(
    const UmiWorkbenchLayoutMetricsCollector *metrics,
    UmiWorkbenchLayoutDataMetrics *out_snapshot)
{
    if (metrics == NULL || out_snapshot == NULL ||
        metrics->structure_size < sizeof(*metrics)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_snapshot = metrics->snapshot;
    return UMI_STATUS_OK;
}

void umi_workbench_layout_metrics_reset(
    UmiWorkbenchLayoutMetricsCollector *metrics)
{
    if (metrics == NULL) return;
    umi_workbench_layout_metrics_init(metrics);
}
