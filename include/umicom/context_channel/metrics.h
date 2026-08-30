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
typedef struct UmiContextMetrics { uint64_t published; uint64_t routed; uint64_t delivered; uint64_t rejected; uint64_t duplicates; uint64_t expired; uint64_t policy_denials; uint64_t subscriber_failures; uint64_t last_publish_ms; uint64_t last_delivery_ms; uint64_t revision; } UmiContextMetrics;
void umi_context_metrics_init(UmiContextMetrics *metrics);
void umi_context_metrics_record_publish(UmiContextMetrics *metrics,uint64_t now_ms);
void umi_context_metrics_record_route(UmiContextMetrics *metrics,size_t count);
void umi_context_metrics_record_delivery(UmiContextMetrics *metrics,uint64_t now_ms);
void umi_context_metrics_record_rejection(UmiContextMetrics *metrics);
void umi_context_metrics_record_duplicate(UmiContextMetrics *metrics);
void umi_context_metrics_record_expired(UmiContextMetrics *metrics);
void umi_context_metrics_record_policy_denial(UmiContextMetrics *metrics);
void umi_context_metrics_record_subscriber_failure(UmiContextMetrics *metrics);
#ifdef __cplusplus
}
#endif
#endif
