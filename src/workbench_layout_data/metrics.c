/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/metrics.c
 *
 * PURPOSE:
 *   Implement persistence and synchronisation metrics without global state or frontend dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/metrics.h"
#include <string.h>



/* Provide the record duration operation used by this module and its client applications. */
static void record_duration(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    uint64_t duration_us)
{
    metrics->snapshot.last_operation_duration_us = duration_us;
    /* Apply this branch only when its contract condition is satisfied. */
    if (duration_us >
        metrics->snapshot.maximum_operation_duration_us) {
        metrics->snapshot.maximum_operation_duration_us = duration_us;
    }
    metrics->snapshot.revision += 1U;
}

/*
 * Initialise workbench layout metrics from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_metrics_init(
    UmiWorkbenchLayoutMetricsCollector *metrics)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL) return;
    (void)memset(metrics, 0, sizeof(*metrics));
    metrics->structure_size = sizeof(*metrics);
    metrics->snapshot.structure_size =
        sizeof(metrics->snapshot);
    metrics->snapshot.revision = 1U;
}

/*
 * Write workbench layout metrics record in its stable representation and report capacity
 * or input failures to the caller.
 */
void umi_workbench_layout_metrics_record_save(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    size_t bytes,
    uint64_t duration_us,
    UmiStatus status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL) return;
    metrics->snapshot.save_count += 1U;
    metrics->snapshot.bytes_written += (uint64_t)bytes;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        metrics->snapshot.sync_failure_count += 1U;
    }
    record_duration(metrics, duration_us);
}

/*
 * Read workbench layout metrics record into validated module state and return a status
 * when input cannot be used.
 */
void umi_workbench_layout_metrics_record_load(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    size_t bytes,
    uint64_t duration_us,
    UmiStatus status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL) return;
    metrics->snapshot.load_count += 1U;
    metrics->snapshot.bytes_read += (uint64_t)bytes;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        metrics->snapshot.sync_failure_count += 1U;
    }
    record_duration(metrics, duration_us);
}

/*
 * Provide the workbench layout metrics record delete operation used by this module and its
 * client applications.
 */
void umi_workbench_layout_metrics_record_delete(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    uint64_t duration_us,
    UmiStatus status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL) return;
    metrics->snapshot.delete_count += 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        metrics->snapshot.sync_failure_count += 1U;
    }
    record_duration(metrics, duration_us);
}

/*
 * Provide the workbench layout metrics record sync operation used by this module and its
 * client applications.
 */
void umi_workbench_layout_metrics_record_sync(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    bool success,
    size_t conflicts,
    uint64_t duration_us)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL) return;
    metrics->snapshot.sync_plan_count += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (success) {
        metrics->snapshot.sync_success_count += 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        metrics->snapshot.sync_failure_count += 1U;
    }
    metrics->snapshot.conflict_count += (uint64_t)conflicts;
    record_duration(metrics, duration_us);
}

/*
 * Provide the workbench layout metrics record outbox operation used by this module and its
 * client applications.
 */
void umi_workbench_layout_metrics_record_outbox(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    bool delivered,
    bool retry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (delivered) {
        metrics->snapshot.outbox_delivery_count += 1U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (retry) {
        metrics->snapshot.outbox_retry_count += 1U;
    }
    metrics->snapshot.revision += 1U;
}

/*
 * Provide the workbench layout metrics record offline replay operation used by this module
 * and its client applications.
 */
void umi_workbench_layout_metrics_record_offline_replay(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    bool success)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL) return;
    metrics->snapshot.offline_replay_count += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!success) metrics->snapshot.sync_failure_count += 1U;
    metrics->snapshot.revision += 1U;
}

/*
 * Provide the workbench layout metrics snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_metrics_snapshot(
    const UmiWorkbenchLayoutMetricsCollector *metrics,
    UmiWorkbenchLayoutDataMetrics *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL || out_snapshot == NULL ||
        metrics->structure_size < sizeof(*metrics)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_snapshot = metrics->snapshot;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by workbench layout metrics so the same storage can be
 * reused safely.
 */
void umi_workbench_layout_metrics_reset(
    UmiWorkbenchLayoutMetricsCollector *metrics)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metrics == NULL) return;
    umi_workbench_layout_metrics_init(metrics);
}
