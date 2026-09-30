/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/extension.h
 *
 * PURPOSE:
 *   Define chart extension points for indicators, series renderers, overlays and tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_CHART_EXTENSION_H
#define UMICOM_CHART_EXTENSION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_EXTENSION_CAPACITY 512U

/**
 * Represent the chart extension snapshot data shared with callers of this public contract.
 */
typedef struct UmiChartExtensionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char kind[64];
    char provider_id[128];
    char entry_point[256];
    int enabled;
    int trusted;
    int32_t order;
    uint64_t revision;
} UmiChartExtensionSnapshot;

/**
 * Represent the chart extension registry data shared with callers of this public contract.
 */
typedef struct UmiChartExtensionRegistry UmiChartExtensionRegistry;

/**
 * Initialise chart extension registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_chart_extension_registry_create(UmiChartExtensionRegistry **out_registry);
/**
 * Release or reset state held by chart extension registry so the same storage can be
 * reused safely.
 */
void umi_chart_extension_registry_destroy(UmiChartExtensionRegistry *registry);
/**
 * Provide the chart extension registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_extension_registry_upsert(UmiChartExtensionRegistry *registry, const UmiChartExtensionSnapshot *item);
/**
 * Remove chart extension registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_extension_registry_remove(UmiChartExtensionRegistry *registry, const char *id);
/**
 * Find chart extension registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_extension_registry_find(const UmiChartExtensionRegistry *registry, const char *id, UmiChartExtensionSnapshot *out_item);
/**
 * Find chart extension registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_extension_registry_at(const UmiChartExtensionRegistry *registry, size_t index, UmiChartExtensionSnapshot *out_item);
/**
 * Return the number of records represented by chart extension registry without changing
 * their state.
 */
size_t umi_chart_extension_registry_count(const UmiChartExtensionRegistry *registry);
/**
 * Provide the chart extension registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_chart_extension_registry_revision(const UmiChartExtensionRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_chart_extension_snapshot_validate(const UmiChartExtensionSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_CHART_EXTENSION_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_chart_extension_registry_upsert_many(UmiChartExtensionRegistry *registry,
    const UmiChartExtensionSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
