/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/stream.h
 *
 * PURPOSE:
 *   Define live streaming state and counters for high-frequency chart updates.
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
#ifndef UMICOM_CHART_STREAM_H
#define UMICOM_CHART_STREAM_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_STREAM_CAPACITY 512U

/**
 * Represent the chart stream snapshot data shared with callers of this public contract.
 */
typedef struct UmiChartStreamSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char series_id[128];
    uint64_t updates;
    uint64_t dropped;
    int64_t last_time;
    double last_value;
    int connected;
    int paused;
    uint64_t revision;
} UmiChartStreamSnapshot;

/**
 * Represent the chart stream registry data shared with callers of this public contract.
 */
typedef struct UmiChartStreamRegistry UmiChartStreamRegistry;

/**
 * Initialise chart stream registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_chart_stream_registry_create(UmiChartStreamRegistry **out_registry);
/**
 * Release or reset state held by chart stream registry so the same storage can be reused
 * safely.
 */
void umi_chart_stream_registry_destroy(UmiChartStreamRegistry *registry);
/**
 * Provide the chart stream registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_stream_registry_upsert(UmiChartStreamRegistry *registry, const UmiChartStreamSnapshot *item);
/**
 * Remove chart stream registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_stream_registry_remove(UmiChartStreamRegistry *registry, const char *id);
/**
 * Find chart stream registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_stream_registry_find(const UmiChartStreamRegistry *registry, const char *id, UmiChartStreamSnapshot *out_item);
/**
 * Find chart stream registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_stream_registry_at(const UmiChartStreamRegistry *registry, size_t index, UmiChartStreamSnapshot *out_item);
/**
 * Provide the chart stream registry record operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_stream_registry_record(UmiChartStreamRegistry *registry,
                                             const char *id,
                                             int64_t time,
                                             double value,
                                             int dropped_update);
/**
 * Provide the chart stream registry set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_stream_registry_set_state(UmiChartStreamRegistry *registry,
                                              const char *id,
                                              int connected,
                                              int paused);
/**
 * Return the number of records represented by chart stream registry without changing their
 * state.
 */
size_t umi_chart_stream_registry_count(const UmiChartStreamRegistry *registry);
/**
 * Provide the chart stream registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_chart_stream_registry_revision(const UmiChartStreamRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_chart_stream_snapshot_validate(const UmiChartStreamSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_CHART_STREAM_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_chart_stream_registry_upsert_many(UmiChartStreamRegistry *registry,
    const UmiChartStreamSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_chart_stream_registry_capture(const UmiChartStreamRegistry *registry,
    UmiChartStreamSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_chart_stream_registry_replace_if_current(UmiChartStreamRegistry *registry,
    uint64_t expected_revision, const UmiChartStreamSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
