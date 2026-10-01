/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/source.h
 *
 * PURPOSE:
 *   Define a DAP-friendly but adapter-neutral debugger record for native and future Umicom runtimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_DEBUG_SOURCE_H
#define UMICOM_DEBUG_SOURCE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_SOURCE_CAPACITY 2048U
#define UMI_DEBUG_SOURCE_API_VERSION 1U

/**
 * Represent the debug source snapshot data shared with callers of this public contract.
 */
typedef struct UmiDebugSourceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char uri[1024];
    char name[256];
    uint64_t source_reference;
    int available;
    uint64_t revision;
} UmiDebugSourceSnapshot;

/**
 * Represent the debug source registry data shared with callers of this public contract.
 */
typedef struct UmiDebugSourceRegistry UmiDebugSourceRegistry;

/**
 * Initialise debug source registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_debug_source_registry_create(UmiDebugSourceRegistry **out_registry);
/**
 * Release or reset state held by debug source registry so the same storage can be reused
 * safely.
 */
void umi_debug_source_registry_destroy(UmiDebugSourceRegistry *registry);
/**
 * Provide the debug source registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_source_registry_upsert(UmiDebugSourceRegistry *registry, const UmiDebugSourceSnapshot *item);
/**
 * Remove debug source registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_source_registry_remove(UmiDebugSourceRegistry *registry, const char *id);
/**
 * Find debug source registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_source_registry_find(const UmiDebugSourceRegistry *registry, const char *id, UmiDebugSourceSnapshot *out_item);
/**
 * Find debug source registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_source_registry_at(const UmiDebugSourceRegistry *registry, size_t index, UmiDebugSourceSnapshot *out_item);
/**
 * Return the number of records represented by debug source registry without changing their
 * state.
 */
size_t umi_debug_source_registry_count(const UmiDebugSourceRegistry *registry);
/**
 * Provide the debug source registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_debug_source_registry_revision(const UmiDebugSourceRegistry *registry);
/**
 * Release or reset state held by debug source registry so the same storage can be reused
 * safely.
 */
void umi_debug_source_registry_clear(UmiDebugSourceRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_debug_source_snapshot_validate(const UmiDebugSourceSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_DEBUG_SOURCE_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_debug_source_registry_upsert_many(UmiDebugSourceRegistry *registry,
    const UmiDebugSourceSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_debug_source_registry_capture(const UmiDebugSourceRegistry *registry,
    UmiDebugSourceSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_debug_source_registry_replace_if_current(UmiDebugSourceRegistry *registry,
    uint64_t expected_revision, const UmiDebugSourceSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);


/** One reviewed change. For REMOVE, initialise item.id; its other fields are
 * ignored. For UPSERT, supply a complete record under the usual domain rules.
 * The edit remains caller-owned and is never changed by publication. */
typedef struct UmiDebugSourceEdit {
    UmiSnapshotEditKind kind;
    UmiDebugSourceSnapshot item;
} UmiDebugSourceEdit;

/** Apply this ordered list only if expected_revision still matches this owner.
 * A stale review returns INVALID_STATE, including for an empty list. Each ID
 * may appear once; duplicates return ALREADY_EXISTS. A missing removal returns
 * NOT_FOUND. Upserts retain normal field normalisation and capacity checks;
 * when full, remove an existing row before adding its replacement.
 * At most twice UMI_DEBUG_SOURCE_CAPACITY edits are accepted. Success publishes
 * all changes together, advances once, and stamps every remaining row with
 * that revision. An empty current list is a no-op. Every refusal preserves
 * the previous collection. out_result is optional and identifies a rejected
 * edit where possible; validation describes text and duplicate-ID errors.
 * Inputs/result must not overlap each other or the owner. Serialize access;
 * staging allocates one registry and performs no I/O or external callbacks. */
UmiStatus umi_debug_source_registry_edit_if_current(UmiDebugSourceRegistry *registry,
    uint64_t expected_revision, const UmiDebugSourceEdit *edits, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
