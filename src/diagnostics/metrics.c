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
/* Find metric while leaving the underlying catalogue or model owned by this module. */
static size_t metric_find(const UmiMetricsRegistry *registry, const char *name)
{ size_t i; /* Visit each bounded item once so every record receives the same rule. */ for (i = 0U; i < registry->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->items[i].name, name) == 0) return i; return SIZE_MAX; }
/*
 * Initialise metrics registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_metrics_registry_create(UmiMetricsRegistry **out_registry)
{
    UmiMetricsRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL; registry = (UmiMetricsRegistry *)calloc(1U, sizeof(*registry)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_diagnostic_lock_init(&registry->lock);
    *out_registry = registry; return UMI_STATUS_OK;
}
/*
 * Release or reset state held by metrics registry so the same storage can be reused
 * safely.
 */
void umi_metrics_registry_destroy(UmiMetricsRegistry *registry)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry != NULL) { free(registry); } }
/*
 * Provide the metrics registry define operation used by this module and its client
 * applications.
 */
UmiStatus umi_metrics_registry_define(UmiMetricsRegistry *registry, const char *name, const char *description, UmiMetricKind kind)
{
    UmiMetricSnapshot *metric;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || name == NULL || name[0] == '\0' || description == NULL || (kind != UMI_METRIC_COUNTER && kind != UMI_METRIC_GAUGE)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(name) >= UMI_METRIC_NAME_CAPACITY || strlen(description) >= UMI_METRIC_DESCRIPTION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_diagnostic_lock_acquire(&registry->lock);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (metric_find(registry, name) != SIZE_MAX) { umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_ALREADY_EXISTS; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_METRIC_REGISTRY_MAX) { umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_CAPACITY_EXCEEDED; }
    metric = &registry->items[registry->count++]; (void)memset(metric, 0, sizeof(*metric));
    (void)snprintf(metric->name, sizeof(metric->name), "%s", name); (void)snprintf(metric->description, sizeof(metric->description), "%s", description); metric->kind = kind;
    umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_OK;
}
/* Provide the metric update operation used by this module and its client applications. */
static UmiStatus metric_update(UmiMetricsRegistry *registry, const char *name, double value, uint64_t now_ns, int add, UmiMetricKind required)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&registry->lock);
    index = metric_find(registry, name); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (index == SIZE_MAX) { umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_NOT_FOUND; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->items[index].kind != required) { umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_INVALID_STATE; }
    registry->items[index].value = add ? registry->items[index].value + value : value; registry->items[index].updated_at_ns = now_ns;
    umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_OK;
}
/* Add metrics counter only after its inputs and available capacity have been checked. */
UmiStatus umi_metrics_counter_add(UmiMetricsRegistry *registry, const char *name, double amount, uint64_t now_ns)
{ return amount < 0.0 ? UMI_STATUS_INVALID_ARGUMENT : metric_update(registry, name, amount, now_ns, 1, UMI_METRIC_COUNTER); }
/*
 * Copy metrics gauge into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_metrics_gauge_set(UmiMetricsRegistry *registry, const char *name, double value, uint64_t now_ns)
{ return metric_update(registry, name, value, now_ns, 0, UMI_METRIC_GAUGE); }
/*
 * Provide the metrics registry get operation used by this module and its client
 * applications.
 */
UmiStatus umi_metrics_registry_get(const UmiMetricsRegistry *registry, const char *name, UmiMetricSnapshot *out_metric)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || name == NULL || out_metric == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&registry->lock);
    index = metric_find(registry, name); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (index == SIZE_MAX) { umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_NOT_FOUND; }
    *out_metric = registry->items[index]; umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by metrics registry without changing their
 * state.
 */
size_t umi_metrics_registry_count(const UmiMetricsRegistry *registry)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry != NULL) { umi_diagnostic_lock_acquire(&registry->lock); n = registry->count; umi_diagnostic_lock_release(&registry->lock); } return n; }
/*
 * Find metrics registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_metrics_registry_at(const UmiMetricsRegistry *registry, size_t index, UmiMetricSnapshot *out_metric)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_metric == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&registry->lock);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) { umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_NOT_FOUND; }
    *out_metric = registry->items[index]; umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_OK;
}
