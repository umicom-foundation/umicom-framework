/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/metrics.h
 *
 * PURPOSE:
 *   Expose operational counters for publication, routing, delivery and failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_METRICS_H
#define UMICOM_CONTEXT_CHANNEL_METRICS_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context metrics data shared with callers of this public contract.
 */
typedef struct UmiContextMetrics { uint64_t published; uint64_t routed; uint64_t delivered; uint64_t rejected; uint64_t duplicates; uint64_t expired; uint64_t policy_denials; uint64_t subscriber_failures; uint64_t last_publish_ms; uint64_t last_delivery_ms; uint64_t revision; } UmiContextMetrics;
/**
 * Initialise context metrics from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_metrics_init(UmiContextMetrics *metrics);
/**
 * Provide the context metrics record publish operation used by this module and its client
 * applications.
 */
void umi_context_metrics_record_publish(UmiContextMetrics *metrics,uint64_t now_ms);
/**
 * Provide the context metrics record route operation used by this module and its client
 * applications.
 */
void umi_context_metrics_record_route(UmiContextMetrics *metrics,size_t count);
/**
 * Provide the context metrics record delivery operation used by this module and its client
 * applications.
 */
void umi_context_metrics_record_delivery(UmiContextMetrics *metrics,uint64_t now_ms);
/**
 * Provide the context metrics record rejection operation used by this module and its
 * client applications.
 */
void umi_context_metrics_record_rejection(UmiContextMetrics *metrics);
/**
 * Provide the context metrics record duplicate operation used by this module and its
 * client applications.
 */
void umi_context_metrics_record_duplicate(UmiContextMetrics *metrics);
/**
 * Provide the context metrics record expired operation used by this module and its client
 * applications.
 */
void umi_context_metrics_record_expired(UmiContextMetrics *metrics);
/**
 * Provide the context metrics record policy denial operation used by this module and its
 * client applications.
 */
void umi_context_metrics_record_policy_denial(UmiContextMetrics *metrics);
/**
 * Provide the context metrics record subscriber failure operation used by this module and
 * its client applications.
 */
void umi_context_metrics_record_subscriber_failure(UmiContextMetrics *metrics);
#ifdef __cplusplus
}
#endif
#endif
