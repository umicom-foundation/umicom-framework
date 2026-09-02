/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/metrics.h
 *
 * PURPOSE:
 *   Aggregate terminal, process and task counters for status bars, dashboards
 *   and operational health checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_METRICS_H
#define UMICOM_TERMINAL_METRICS_H

#include "umicom/platform/process_supervisor.h"
#include "umicom/platform/task_queue.h"
#include "umicom/terminal/manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the terminal metrics data shared with callers of this public contract.
 */
typedef struct UmiTerminalMetrics {
    size_t sessions;
    size_t ready_sessions;
    size_t running_sessions;
    size_t failed_sessions;
    uint64_t commands_executed;
    size_t transcript_lines;
    UmiProcessSupervisorStats processes;
    UmiTaskQueueStats tasks;
} UmiTerminalMetrics;

/**
 * Provide the terminal metrics collect operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_metrics_collect(UmiTerminalManager *manager,
                                       UmiProcessSupervisor *supervisor,
                                       UmiTaskQueue *task_queue,
                                       UmiTerminalMetrics *out_metrics);

#ifdef __cplusplus
}
#endif
#endif
