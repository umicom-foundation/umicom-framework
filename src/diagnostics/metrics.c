/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/metrics.c
 *
 * PURPOSE:
 *   Implement bounded metric definitions and atomic registry updates through the
 *   portable Framework mutex.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/metrics.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "diagnostic_lock.h"
struct UmiMetricsRegistry { UmiMetricSnapshot items[UMI_METRIC_REGISTRY_MAX]; size_t count; UmiDiagnosticLock lock; };
static size_t metric_find(const UmiMetricsRegistry *registry, const char *name)
{ size_t i; for (i = 0U; i < registry->count; ++i) if (strcmp(registry->items[i].name, name) == 0) return i; return SIZE_MAX; }
UmiStatus umi_metrics_registry_create(UmiMetricsRegistry **out_registry)
{
    UmiMetricsRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL; registry = (UmiMetricsRegistry *)calloc(1U, sizeof(*registry)); if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_diagnostic_lock_init(&registry->lock);
    *out_registry = registry; return UMI_STATUS_OK;
}
void umi_metrics_registry_destroy(UmiMetricsRegistry *registry)
{ if (registry != NULL) { free(registry); } }
UmiStatus umi_metrics_registry_define(UmiMetricsRegistry *registry, const char *name, const char *description, UmiMetricKind kind)
{
    UmiMetricSnapshot *metric;
    if (registry == NULL || name == NULL || name[0] == '\0' || description == NULL || (kind != UMI_METRIC_COUNTER && kind != UMI_METRIC_GAUGE)) return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(name) >= UMI_METRIC_NAME_CAPACITY || strlen(description) >= UMI_METRIC_DESCRIPTION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_diagnostic_lock_acquire(&registry->lock);
    if (metric_find(registry, name) != SIZE_MAX) { umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_ALREADY_EXISTS; }
    if (registry->count >= UMI_METRIC_REGISTRY_MAX) { umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_CAPACITY_EXCEEDED; }
    metric = &registry->items[registry->count++]; (void)memset(metric, 0, sizeof(*metric));
    (void)snprintf(metric->name, sizeof(metric->name), "%s", name); (void)snprintf(metric->description, sizeof(metric->description), "%s", description); metric->kind = kind;
    umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_OK;
}
static UmiStatus metric_update(UmiMetricsRegistry *registry, const char *name, double value, uint64_t now_ns, int add, UmiMetricKind required)
{
    size_t index;
    if (registry == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&registry->lock);
    index = metric_find(registry, name); if (index == SIZE_MAX) { umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_NOT_FOUND; }
    if (registry->items[index].kind != required) { umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_INVALID_STATE; }
    registry->items[index].value = add ? registry->items[index].value + value : value; registry->items[index].updated_at_ns = now_ns;
    umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_OK;
}
UmiStatus umi_metrics_counter_add(UmiMetricsRegistry *registry, const char *name, double amount, uint64_t now_ns)
{ return amount < 0.0 ? UMI_STATUS_INVALID_ARGUMENT : metric_update(registry, name, amount, now_ns, 1, UMI_METRIC_COUNTER); }
UmiStatus umi_metrics_gauge_set(UmiMetricsRegistry *registry, const char *name, double value, uint64_t now_ns)
{ return metric_update(registry, name, value, now_ns, 0, UMI_METRIC_GAUGE); }
UmiStatus umi_metrics_registry_get(const UmiMetricsRegistry *registry, const char *name, UmiMetricSnapshot *out_metric)
{
    size_t index;
    if (registry == NULL || name == NULL || out_metric == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&registry->lock);
    index = metric_find(registry, name); if (index == SIZE_MAX) { umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_NOT_FOUND; }
    *out_metric = registry->items[index]; umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_OK;
}
size_t umi_metrics_registry_count(const UmiMetricsRegistry *registry)
{ size_t n = 0U; if (registry != NULL) { umi_diagnostic_lock_acquire(&registry->lock); n = registry->count; umi_diagnostic_lock_release(&registry->lock); } return n; }
UmiStatus umi_metrics_registry_at(const UmiMetricsRegistry *registry, size_t index, UmiMetricSnapshot *out_metric)
{
    if (registry == NULL || out_metric == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&registry->lock);
    if (index >= registry->count) { umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_NOT_FOUND; }
    *out_metric = registry->items[index]; umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_OK;
}
