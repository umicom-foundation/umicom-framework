/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/metrics.c
 *
 * PURPOSE:
 *   Compute bounded completion execution metrics without side effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/metrics.h"
UmiProductExecutionMetrics umi_product_execution_metrics_collect(
    const UmiProductExecutionWorkQueue *queue)
{
    size_t i;
    UmiProductExecutionMetrics m = {0};
    if (queue == NULL) return m;
    m.total = queue->count;
    for (i = 0U; i < queue->count; ++i) {
        m.attempts += queue->items[i].attempts;
        switch (queue->items[i].state) {
        case UMI_PRODUCT_EXECUTION_PENDING: m.pending++; break;
        case UMI_PRODUCT_EXECUTION_READY: m.ready++; break;
        case UMI_PRODUCT_EXECUTION_RUNNING: m.running++; break;
        case UMI_PRODUCT_EXECUTION_SUCCEEDED: m.succeeded++; break;
        case UMI_PRODUCT_EXECUTION_FAILED: m.failed++; break;
        case UMI_PRODUCT_EXECUTION_BLOCKED: m.blocked++; break;
        case UMI_PRODUCT_EXECUTION_CANCELLED: m.cancelled++; break;
        case UMI_PRODUCT_EXECUTION_ROLLED_BACK: m.rolled_back++; break;
        default: break;
        }
    }
    if (m.total != 0U) {
        m.completion_percent = (unsigned)(((m.succeeded + m.rolled_back) * 100U) / m.total);
    }
    return m;
}
