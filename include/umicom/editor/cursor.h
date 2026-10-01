/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/cursor.h
 *
 * PURPOSE:
 *   Define reusable editor cursor and caret state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This is a product-neutral C23 model. The registry owns snapshot copies by
 * value; callers own external resources and coordinate cross-thread mutation.
 */
#ifndef UMICOM_EDITOR_CURSOR_H
#define UMICOM_EDITOR_CURSOR_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_CURSOR_CAPACITY 2048U
/**
 * Represent the editor cursor snapshot data shared with callers of this public contract.
 */
typedef struct UmiEditorCursorSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    uint64_t line;
    uint64_t column;
    uint64_t preferred_column;
    int primary;
    int visible;
    uint64_t revision;
} UmiEditorCursorSnapshot;
/**
 * Represent the editor cursor registry data shared with callers of this public contract.
 */
typedef struct UmiEditorCursorRegistry UmiEditorCursorRegistry;
/**
 * Initialise editor cursor registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_cursor_registry_create(UmiEditorCursorRegistry **out_registry);
/**
 * Release or reset state held by editor cursor registry so the same storage can be reused
 * safely.
 */
void umi_editor_cursor_registry_destroy(UmiEditorCursorRegistry *registry);
/**
 * Provide the editor cursor registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_cursor_registry_upsert(UmiEditorCursorRegistry *registry,const UmiEditorCursorSnapshot *item);
/**
 * Remove editor cursor registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_cursor_registry_remove(UmiEditorCursorRegistry *registry,const char *id);
/**
 * Find editor cursor registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_cursor_registry_find(const UmiEditorCursorRegistry *registry,const char *id,UmiEditorCursorSnapshot *out_item);
/**
 * Find editor cursor registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_cursor_registry_at(const UmiEditorCursorRegistry *registry,size_t index,UmiEditorCursorSnapshot *out_item);
/**
 * Return the number of records represented by editor cursor registry without changing
 * their state.
 */
size_t umi_editor_cursor_registry_count(const UmiEditorCursorRegistry *registry);
/**
 * Provide the editor cursor registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_cursor_registry_revision(const UmiEditorCursorRegistry *registry);

/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_editor_cursor_snapshot_validate(const UmiEditorCursorSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_EDITOR_CURSOR_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_editor_cursor_registry_upsert_many(UmiEditorCursorRegistry *registry,
    const UmiEditorCursorSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_editor_cursor_registry_capture(const UmiEditorCursorRegistry *registry,
    UmiEditorCursorSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_editor_cursor_registry_replace_if_current(UmiEditorCursorRegistry *registry,
    uint64_t expected_revision, const UmiEditorCursorSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif
#endif
