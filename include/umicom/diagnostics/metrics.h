/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/metrics.h
 *
 * PURPOSE:
 *   Define thread-safe counters, gauges and immutable metric snapshots for
 *   Framework and product observability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_METRICS_H
#define UMICOM_DIAGNOSTICS_METRICS_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_METRIC_NAME_CAPACITY 128U
#define UMI_METRIC_DESCRIPTION_CAPACITY 256U
#define UMI_METRIC_REGISTRY_MAX 512U
/**
 * List the named metric kind values accepted by this public contract.
 */
typedef enum UmiMetricKind { UMI_METRIC_COUNTER = 1, UMI_METRIC_GAUGE = 2 } UmiMetricKind;
/**
 * Represent the metric snapshot data shared with callers of this public contract.
 */
typedef struct UmiMetricSnapshot {
    char name[UMI_METRIC_NAME_CAPACITY];
    char description[UMI_METRIC_DESCRIPTION_CAPACITY];
    UmiMetricKind kind;
    double value;
    uint64_t updated_at_ns;
} UmiMetricSnapshot;
/**
 * Represent the metrics registry data shared with callers of this public contract.
 */
typedef struct UmiMetricsRegistry UmiMetricsRegistry;
/**
 * Initialise metrics registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_metrics_registry_create(UmiMetricsRegistry **out_registry);
/**
 * Release or reset state held by metrics registry so the same storage can be reused
 * safely.
 */
void umi_metrics_registry_destroy(UmiMetricsRegistry *registry);
/**
 * Provide the metrics registry define operation used by this module and its client
 * applications.
 */
UmiStatus umi_metrics_registry_define(UmiMetricsRegistry *registry,
                                      const char *name,
                                      const char *description,
                                      UmiMetricKind kind);
/**
 * Add metrics counter only after its inputs and available capacity have been checked.
 */
UmiStatus umi_metrics_counter_add(UmiMetricsRegistry *registry,
                                  const char *name,
                                  double amount,
                                  uint64_t now_ns);
/**
 * Copy metrics gauge into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_metrics_gauge_set(UmiMetricsRegistry *registry,
                                const char *name,
                                double value,
                                uint64_t now_ns);
/**
 * Provide the metrics registry get operation used by this module and its client
 * applications.
 */
UmiStatus umi_metrics_registry_get(const UmiMetricsRegistry *registry,
                                   const char *name,
                                   UmiMetricSnapshot *out_metric);
/**
 * Return the number of records represented by metrics registry without changing their
 * state.
 */
size_t umi_metrics_registry_count(const UmiMetricsRegistry *registry);
/**
 * Find metrics registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_metrics_registry_at(const UmiMetricsRegistry *registry,
                                  size_t index,
                                  UmiMetricSnapshot *out_metric);
#ifdef __cplusplus
}
#endif

#endif
