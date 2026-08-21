/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/metrics.h
 *
 * PURPOSE:
 *   Collect bounded operational counters and latency evidence for layout persistence and synchronisation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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


typedef struct UmiWorkbenchLayoutMetricsCollector {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataMetrics snapshot;
} UmiWorkbenchLayoutMetricsCollector;

void umi_workbench_layout_metrics_init(
    UmiWorkbenchLayoutMetricsCollector *metrics);

void umi_workbench_layout_metrics_record_save(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    size_t bytes,
    uint64_t duration_us,
    UmiStatus status);

void umi_workbench_layout_metrics_record_load(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    size_t bytes,
    uint64_t duration_us,
    UmiStatus status);

void umi_workbench_layout_metrics_record_delete(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    uint64_t duration_us,
    UmiStatus status);

void umi_workbench_layout_metrics_record_sync(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    bool success,
    size_t conflicts,
    uint64_t duration_us);

void umi_workbench_layout_metrics_record_outbox(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    bool delivered,
    bool retry);

void umi_workbench_layout_metrics_record_offline_replay(
    UmiWorkbenchLayoutMetricsCollector *metrics,
    bool success);

UmiStatus umi_workbench_layout_metrics_snapshot(
    const UmiWorkbenchLayoutMetricsCollector *metrics,
    UmiWorkbenchLayoutDataMetrics *out_snapshot);

void umi_workbench_layout_metrics_reset(
    UmiWorkbenchLayoutMetricsCollector *metrics);

#ifdef __cplusplus
}
#endif

#endif
