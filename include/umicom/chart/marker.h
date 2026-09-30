/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/marker.h
 *
 * PURPOSE:
 *   Define event, signal and trade markers on time-series charts.
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
#ifndef UMICOM_CHART_MARKER_H
#define UMICOM_CHART_MARKER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_MARKER_CAPACITY 4096U

/**
 * Represent the chart marker snapshot data shared with callers of this public contract.
 */
typedef struct UmiChartMarkerSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char series_id[128];
    int64_t time;
    double value;
    char text[256];
    char shape[64];
    char position[64];
    int32_t order;
    uint64_t revision;
} UmiChartMarkerSnapshot;

/**
 * Represent the chart marker registry data shared with callers of this public contract.
 */
typedef struct UmiChartMarkerRegistry UmiChartMarkerRegistry;

/**
 * Initialise chart marker registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_chart_marker_registry_create(UmiChartMarkerRegistry **out_registry);
/**
 * Release or reset state held by chart marker registry so the same storage can be reused
 * safely.
 */
void umi_chart_marker_registry_destroy(UmiChartMarkerRegistry *registry);
/**
 * Provide the chart marker registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_marker_registry_upsert(UmiChartMarkerRegistry *registry, const UmiChartMarkerSnapshot *item);
/**
 * Remove chart marker registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_marker_registry_remove(UmiChartMarkerRegistry *registry, const char *id);
/**
 * Find chart marker registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_marker_registry_find(const UmiChartMarkerRegistry *registry, const char *id, UmiChartMarkerSnapshot *out_item);
/**
 * Find chart marker registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_marker_registry_at(const UmiChartMarkerRegistry *registry, size_t index, UmiChartMarkerSnapshot *out_item);
/**
 * Return the number of records represented by chart marker registry without changing their
 * state.
 */
size_t umi_chart_marker_registry_count(const UmiChartMarkerRegistry *registry);
/**
 * Provide the chart marker registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_chart_marker_registry_revision(const UmiChartMarkerRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_chart_marker_snapshot_validate(const UmiChartMarkerSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_CHART_MARKER_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_chart_marker_registry_upsert_many(UmiChartMarkerRegistry *registry,
    const UmiChartMarkerSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
