/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/metrics.h
 *
 * PURPOSE:
 *   Collect bounded operational counters for Layout Browser and visual designer
 *   behaviour without frontend-specific telemetry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_METRICS_H
#define UMICOM_WORKBENCH_DESIGNER_METRICS_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer metrics data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerMetrics {
    uint64_t sessions_opened;
    uint64_t sessions_closed;
    uint64_t commands_executed;
    uint64_t commands_failed;
    uint64_t undo_count;
    uint64_t redo_count;
    uint64_t saves_started;
    uint64_t saves_completed;
    uint64_t saves_failed;
    uint64_t imports;
    uint64_t exports;
    uint64_t drag_operations;
    uint64_t drop_rejections;
    uint64_t conflicts_detected;
    uint64_t collaboration_updates;
    uint64_t canvas_rebuilds;
    uint64_t browser_queries;
    uint64_t total_command_duration_us;
    uint64_t maximum_command_duration_us;
    uint64_t revision;
} UmiWorkbenchDesignerMetrics;

/**
 * Initialise workbench designer metrics from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_metrics_init(UmiWorkbenchDesignerMetrics *metrics);
/**
 * Provide the workbench designer metrics record command operation used by this module and
 * its client applications.
 */
void umi_workbench_designer_metrics_record_command(
    UmiWorkbenchDesignerMetrics *metrics,
    UmiStatus status,
    uint64_t duration_us,
    bool undo,
    bool redo);
/**
 * Write workbench designer metrics record in its stable representation and report capacity
 * or input failures to the caller.
 */
void umi_workbench_designer_metrics_record_save(
    UmiWorkbenchDesignerMetrics *metrics,
    UmiStatus status);
/**
 * Provide the workbench designer metrics command success rate operation used by this
 * module and its client applications.
 */
double umi_workbench_designer_metrics_command_success_rate(
    const UmiWorkbenchDesignerMetrics *metrics);
/**
 * Provide the workbench designer metrics average command duration us operation used by
 * this module and its client applications.
 */
double umi_workbench_designer_metrics_average_command_duration_us(
    const UmiWorkbenchDesignerMetrics *metrics);

#ifdef __cplusplus
}
#endif

#endif
