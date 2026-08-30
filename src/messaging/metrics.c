/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/metrics.c
 *
 * PURPOSE:
 *   Implement atomic message counters and immutable snapshots for diagnostics and health reporting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/metrics.h"

#include <stdatomic.h>
#include <stddef.h>
#include <string.h>

struct UmiMessageMetricsCounter {
    atomic_uint_fast64_t accepted;
    atomic_uint_fast64_t delivered;
    atomic_uint_fast64_t retried;
    atomic_uint_fast64_t failed;
    atomic_uint_fast64_t dead_lettered;
    atomic_uint_fast64_t replayed;
};

void umi_message_metrics_init(UmiMessageMetricsCounter *counter)
{
    if (counter == NULL) return;
    (void)memset(counter, 0, sizeof(*counter));
}

void umi_message_metrics_increment(UmiMessageMetricsCounter *counter,
                                   UmiMessageMetric metric)
{
    atomic_uint_fast64_t *value = NULL;
    if (counter == NULL) return;
    switch (metric) {
    case UMI_MESSAGE_METRIC_ACCEPTED: value = &counter->accepted; break;
    case UMI_MESSAGE_METRIC_DELIVERED: value = &counter->delivered; break;
    case UMI_MESSAGE_METRIC_RETRIED: value = &counter->retried; break;
    case UMI_MESSAGE_METRIC_FAILED: value = &counter->failed; break;
    case UMI_MESSAGE_METRIC_DEAD_LETTERED: value = &counter->dead_lettered; break;
    case UMI_MESSAGE_METRIC_REPLAYED: value = &counter->replayed; break;
    default: return;
    }
    (void)atomic_fetch_add_explicit(value, 1U, memory_order_relaxed);
}

UmiMessageMetrics umi_message_metrics_snapshot(
    const UmiMessageMetricsCounter *counter)
{
    UmiMessageMetrics metrics;
    (void)memset(&metrics, 0, sizeof(metrics));
    if (counter == NULL) return metrics;
    metrics.accepted = atomic_load_explicit(&counter->accepted,
                                            memory_order_relaxed);
    metrics.delivered = atomic_load_explicit(&counter->delivered,
                                             memory_order_relaxed);
    metrics.retried = atomic_load_explicit(&counter->retried,
                                           memory_order_relaxed);
    metrics.failed = atomic_load_explicit(&counter->failed,
                                          memory_order_relaxed);
    metrics.dead_lettered = atomic_load_explicit(&counter->dead_lettered,
                                                 memory_order_relaxed);
    metrics.replayed = atomic_load_explicit(&counter->replayed,
                                            memory_order_relaxed);
    return metrics;
}

size_t umi_message_metrics_counter_size(void)
{
    return sizeof(UmiMessageMetricsCounter);
}
