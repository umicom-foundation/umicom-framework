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

/**
 * List the named message metric values accepted by this public contract.
 */
typedef enum UmiMessageMetric {
    UMI_MESSAGE_METRIC_ACCEPTED = 1,
    UMI_MESSAGE_METRIC_DELIVERED = 2,
    UMI_MESSAGE_METRIC_RETRIED = 3,
    UMI_MESSAGE_METRIC_FAILED = 4,
    UMI_MESSAGE_METRIC_DEAD_LETTERED = 5,
    UMI_MESSAGE_METRIC_REPLAYED = 6
} UmiMessageMetric;

/**
 * Represent the message metrics data shared with callers of this public contract.
 */
typedef struct UmiMessageMetrics {
    uint64_t accepted;
    uint64_t delivered;
    uint64_t retried;
    uint64_t failed;
    uint64_t dead_lettered;
    uint64_t replayed;
} UmiMessageMetrics;

/**
 * Represent the message metrics counter data shared with callers of this public contract.
 */
typedef struct UmiMessageMetricsCounter UmiMessageMetricsCounter;

/**
 * Initialise message metrics from caller-provided values so later operations receive a
 * known state.
 */
void umi_message_metrics_init(UmiMessageMetricsCounter *counter);
/**
 * Provide the message metrics increment operation used by this module and its client
 * applications.
 */
void umi_message_metrics_increment(UmiMessageMetricsCounter *counter,
                                   UmiMessageMetric metric);
/**
 * Provide the message metrics snapshot operation used by this module and its client
 * applications.
 */
UmiMessageMetrics umi_message_metrics_snapshot(
    const UmiMessageMetricsCounter *counter);
/**
 * Return the number of records represented by message metrics counter without changing
 * their state.
 */
size_t umi_message_metrics_counter_size(void);

#ifdef __cplusplus
}
#endif

#endif
