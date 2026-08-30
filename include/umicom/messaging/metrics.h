/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/metrics.h
 *
 * PURPOSE:
 *   Define lock-free Integration Fabric counters for accepted, delivered, retried, failed, dead-lettered and replayed messages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_METRICS_H
#define UMICOM_MESSAGING_METRICS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiMessageMetric {
    UMI_MESSAGE_METRIC_ACCEPTED = 1,
    UMI_MESSAGE_METRIC_DELIVERED = 2,
    UMI_MESSAGE_METRIC_RETRIED = 3,
    UMI_MESSAGE_METRIC_FAILED = 4,
    UMI_MESSAGE_METRIC_DEAD_LETTERED = 5,
    UMI_MESSAGE_METRIC_REPLAYED = 6
} UmiMessageMetric;

typedef struct UmiMessageMetrics {
    uint64_t accepted;
    uint64_t delivered;
    uint64_t retried;
    uint64_t failed;
    uint64_t dead_lettered;
    uint64_t replayed;
} UmiMessageMetrics;

typedef struct UmiMessageMetricsCounter UmiMessageMetricsCounter;

void umi_message_metrics_init(UmiMessageMetricsCounter *counter);
void umi_message_metrics_increment(UmiMessageMetricsCounter *counter,
                                   UmiMessageMetric metric);
UmiMessageMetrics umi_message_metrics_snapshot(
    const UmiMessageMetricsCounter *counter);
size_t umi_message_metrics_counter_size(void);

#ifdef __cplusplus
}
#endif

#endif
