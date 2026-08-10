/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/metrics.h
 *
 * PURPOSE:
 *   Define thread-safe counters, gauges and immutable metric snapshots for
 *   Framework and product observability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef enum UmiMetricKind { UMI_METRIC_COUNTER = 1, UMI_METRIC_GAUGE = 2 } UmiMetricKind;
typedef struct UmiMetricSnapshot {
    char name[UMI_METRIC_NAME_CAPACITY];
    char description[UMI_METRIC_DESCRIPTION_CAPACITY];
    UmiMetricKind kind;
    double value;
    uint64_t updated_at_ns;
} UmiMetricSnapshot;
typedef struct UmiMetricsRegistry UmiMetricsRegistry;
UmiStatus umi_metrics_registry_create(UmiMetricsRegistry **out_registry);
void umi_metrics_registry_destroy(UmiMetricsRegistry *registry);
UmiStatus umi_metrics_registry_define(UmiMetricsRegistry *registry,
                                      const char *name,
                                      const char *description,
                                      UmiMetricKind kind);
UmiStatus umi_metrics_counter_add(UmiMetricsRegistry *registry,
                                  const char *name,
                                  double amount,
                                  uint64_t now_ns);
UmiStatus umi_metrics_gauge_set(UmiMetricsRegistry *registry,
                                const char *name,
                                double value,
                                uint64_t now_ns);
UmiStatus umi_metrics_registry_get(const UmiMetricsRegistry *registry,
                                   const char *name,
                                   UmiMetricSnapshot *out_metric);
size_t umi_metrics_registry_count(const UmiMetricsRegistry *registry);
UmiStatus umi_metrics_registry_at(const UmiMetricsRegistry *registry,
                                  size_t index,
                                  UmiMetricSnapshot *out_metric);
#ifdef __cplusplus
}
#endif

#endif
