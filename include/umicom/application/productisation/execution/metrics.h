/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/metrics.h
 *
 * PURPOSE:
 *   Summarise live completion execution progress and retry/blocked counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_METRICS_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_METRICS_H
#include "umicom/application/productisation/execution/work_queue.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductExecutionMetrics {
    size_t total;
    size_t pending;
    size_t ready;
    size_t running;
    size_t succeeded;
    size_t failed;
    size_t blocked;
    size_t cancelled;
    size_t rolled_back;
    size_t attempts;
    unsigned completion_percent;
} UmiProductExecutionMetrics;
/** Calculate current queue, outcome, retry and progress counters. */
UmiProductExecutionMetrics umi_product_execution_metrics_collect(
    const UmiProductExecutionWorkQueue *queue);
#ifdef __cplusplus
}
#endif
#endif
