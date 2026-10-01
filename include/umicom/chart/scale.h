/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/scale.h
 *
 * PURPOSE:
 *   Define independent price/value scales for multi-pane financial and scientific charts.
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
#ifndef UMICOM_CHART_SCALE_H
#define UMICOM_CHART_SCALE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_SCALE_CAPACITY 256U

/**
 * Represent the chart scale snapshot data shared with callers of this public contract.
 */
typedef struct UmiChartScaleSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char pane_id[128];
    char side[32];
    double minimum;
    double maximum;
    double margin_top;
    double margin_bottom;
    int auto_scale;
    int logarithmic;
    int inverted;
    uint64_t revision;
} UmiChartScaleSnapshot;

/**
 * Represent the chart scale registry data shared with callers of this public contract.
 */
typedef struct UmiChartScaleRegistry UmiChartScaleRegistry;

/**
 * Initialise chart scale registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_chart_scale_registry_create(UmiChartScaleRegistry **out_registry);
/**
 * Release or reset state held by chart scale registry so the same storage can be reused
 * safely.
 */
void umi_chart_scale_registry_destroy(UmiChartScaleRegistry *registry);
/**
 * Provide the chart scale registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_scale_registry_upsert(UmiChartScaleRegistry *registry, const UmiChartScaleSnapshot *item);
/**
 * Remove chart scale registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_scale_registry_remove(UmiChartScaleRegistry *registry, const char *id);
/**
 * Find chart scale registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_scale_registry_find(const UmiChartScaleRegistry *registry, const char *id, UmiChartScaleSnapshot *out_item);
/**
 * Find chart scale registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_scale_registry_at(const UmiChartScaleRegistry *registry, size_t index, UmiChartScaleSnapshot *out_item);
/**
 * Return the number of records represented by chart scale registry without changing their
 * state.
 */
size_t umi_chart_scale_registry_count(const UmiChartScaleRegistry *registry);
/**
 * Provide the chart scale registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_chart_scale_registry_revision(const UmiChartScaleRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_chart_scale_snapshot_validate(const UmiChartScaleSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_CHART_SCALE_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_chart_scale_registry_upsert_many(UmiChartScaleRegistry *registry,
    const UmiChartScaleSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_chart_scale_registry_capture(const UmiChartScaleRegistry *registry,
    UmiChartScaleSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

/** Replace the complete collection only while expected_revision still matches
 * this registry. Use a revision from capture, not a different registry. A stale
 * proposal returns INVALID_STATE without changing records. IDs must be unique;
 * an empty proposal clears the collection, except an already-empty collection
 * needs no change. Successful publication advances the revision once and gives
 * every stored row that revision. Revision exhaustion returns CAPACITY_EXCEEDED.
 * Existing upsert normalisation remains in effect. Any validation/allocation
 * failure retains the complete previous collection. Inputs and optional result
 * must not overlap each other or registry storage; serialize owner access.
 * This is an in-memory publication, not a thread lock or a durable disk save. */
UmiStatus umi_chart_scale_registry_replace_if_current(UmiChartScaleRegistry *registry,
    uint64_t expected_revision, const UmiChartScaleSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
