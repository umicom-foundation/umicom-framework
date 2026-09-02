/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/metrics.h
 *
 * PURPOSE:
 *   Collect bounded operational counters and latency evidence for layout persistence and synchronisation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_METRICS_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_METRICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/workbench_layout_data/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench layout metrics collector data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutMetricsCollector {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataMetrics snapshot;
} UmiWorkbenchLayoutMetricsCollector;

/**
 * Initialise workbench layout metrics from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_metrics_init(
    UmiWorkbenchLayoutMetricsCollector *metrics);

/**
 * Write workbench layout metrics record in its stable representation and report capacity
 * or input failures to the caller.
 */
void umi_workbench_layout_metrics_record_save(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    size_t bytes,
    uint64_t duration_us,
    UmiStatus status);

/**
 * Read workbench layout metrics record into validated module state and return a status
 * when input cannot be used.
 */
void umi_workbench_layout_metrics_record_load(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    size_t bytes,
    uint64_t duration_us,
    UmiStatus status);

/**
 * Provide the workbench layout metrics record delete operation used by this module and its
 * client applications.
 */
void umi_workbench_layout_metrics_record_delete(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    uint64_t duration_us,
    UmiStatus status);

/**
 * Provide the workbench layout metrics record sync operation used by this module and its
 * client applications.
 */
void umi_workbench_layout_metrics_record_sync(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    bool success,
    size_t conflicts,
    uint64_t duration_us);

/**
 * Provide the workbench layout metrics record outbox operation used by this module and its
 * client applications.
 */
void umi_workbench_layout_metrics_record_outbox(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    bool delivered,
    bool retry);

/**
 * Provide the workbench layout metrics record offline replay operation used by this module
 * and its client applications.
 */
void umi_workbench_layout_metrics_record_offline_replay(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    bool success);

/**
 * Provide the workbench layout metrics snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_metrics_snapshot(
    const UmiWorkbenchLayoutMetricsCollector *metrics,
    UmiWorkbenchLayoutDataMetrics *out_snapshot);

/**
 * Release or reset state held by workbench layout metrics so the same storage can be
 * reused safely.
 */
void umi_workbench_layout_metrics_reset(
    UmiWorkbenchLayoutMetricsCollector *metrics);

#ifdef __cplusplus
}
#endif

#endif
