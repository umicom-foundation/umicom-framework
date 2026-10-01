/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/property_inspector.h
 *
 * PURPOSE:
 *   Define a generic property-inspector model reusable by Studio, Designer and domain applications.
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
#ifndef UMICOM_UI_PROPERTY_INSPECTOR_H
#define UMICOM_UI_PROPERTY_INSPECTOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_PROPERTY_INSPECTOR_CAPACITY 2048U

/**
 * Represent the ui inspector property snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiInspectorPropertySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char object_id[128];
    char category[128];
    char name[128];
    char value[512];
    char value_type[64];
    int editable;
    int required;
    int32_t order;
    uint64_t revision;
} UmiUiInspectorPropertySnapshot;

/**
 * Represent the ui inspector property registry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiInspectorPropertyRegistry UmiUiInspectorPropertyRegistry;

/**
 * Initialise ui property inspector registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_ui_property_inspector_registry_create(UmiUiInspectorPropertyRegistry **out_registry);
/**
 * Release or reset state held by ui property inspector registry so the same storage can be
 * reused safely.
 */
void umi_ui_property_inspector_registry_destroy(UmiUiInspectorPropertyRegistry *registry);
/**
 * Provide the ui property inspector registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_property_inspector_registry_upsert(UmiUiInspectorPropertyRegistry *registry, const UmiUiInspectorPropertySnapshot *item);
/**
 * Remove ui property inspector registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_property_inspector_registry_remove(UmiUiInspectorPropertyRegistry *registry, const char *id);
/**
 * Find ui property inspector registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_ui_property_inspector_registry_find(const UmiUiInspectorPropertyRegistry *registry, const char *id, UmiUiInspectorPropertySnapshot *out_item);
/**
 * Find ui property inspector registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_ui_property_inspector_registry_at(const UmiUiInspectorPropertyRegistry *registry, size_t index, UmiUiInspectorPropertySnapshot *out_item);
/**
 * Return the number of records represented by ui property inspector registry without
 * changing their state.
 */
size_t umi_ui_property_inspector_registry_count(const UmiUiInspectorPropertyRegistry *registry);
/**
 * Provide the ui property inspector registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_ui_property_inspector_registry_revision(const UmiUiInspectorPropertyRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_ui_property_inspector_snapshot_validate(const UmiUiInspectorPropertySnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_UI_PROPERTY_INSPECTOR_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_ui_property_inspector_registry_upsert_many(UmiUiInspectorPropertyRegistry *registry,
    const UmiUiInspectorPropertySnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_ui_property_inspector_registry_capture(const UmiUiInspectorPropertyRegistry *registry,
    UmiUiInspectorPropertySnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_ui_property_inspector_registry_replace_if_current(UmiUiInspectorPropertyRegistry *registry,
    uint64_t expected_revision, const UmiUiInspectorPropertySnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
