/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/diff_hunk.h
 *
 * PURPOSE:
 *   Define side-by-side and inline diff hunks for editors, reviews and merge tools.
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
#ifndef UMICOM_EDITOR_DIFF_HUNK_H
#define UMICOM_EDITOR_DIFF_HUNK_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_DIFF_HUNK_CAPACITY 8192U
/**
 * Represent the editor diff hunk snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorDiffHunkSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char left_uri[1024];
    char right_uri[1024];
    uint64_t old_start;
    uint64_t old_count;
    uint64_t new_start;
    uint64_t new_count;
    int state;
    uint64_t revision;
} UmiEditorDiffHunkSnapshot;
/**
 * Represent the editor diff hunk registry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorDiffHunkRegistry UmiEditorDiffHunkRegistry;
/**
 * Initialise editor diff hunk registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_diff_hunk_registry_create(UmiEditorDiffHunkRegistry **out_registry);
/**
 * Release or reset state held by editor diff hunk registry so the same storage can be
 * reused safely.
 */
void umi_editor_diff_hunk_registry_destroy(UmiEditorDiffHunkRegistry *registry);
/**
 * Provide the editor diff hunk registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_diff_hunk_registry_upsert(UmiEditorDiffHunkRegistry *registry,const UmiEditorDiffHunkSnapshot *item);
/**
 * Remove editor diff hunk registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_diff_hunk_registry_remove(UmiEditorDiffHunkRegistry *registry,const char *id);
/**
 * Find editor diff hunk registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_diff_hunk_registry_find(const UmiEditorDiffHunkRegistry *registry,const char *id,UmiEditorDiffHunkSnapshot *out_item);
/**
 * Find editor diff hunk registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_diff_hunk_registry_at(const UmiEditorDiffHunkRegistry *registry,size_t index,UmiEditorDiffHunkSnapshot *out_item);
/**
 * Return the number of records represented by editor diff hunk registry without changing
 * their state.
 */
size_t umi_editor_diff_hunk_registry_count(const UmiEditorDiffHunkRegistry *registry);
/**
 * Provide the editor diff hunk registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_diff_hunk_registry_revision(const UmiEditorDiffHunkRegistry *registry);

/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_editor_diff_hunk_snapshot_validate(const UmiEditorDiffHunkSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_EDITOR_DIFF_HUNK_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_editor_diff_hunk_registry_upsert_many(UmiEditorDiffHunkRegistry *registry,
    const UmiEditorDiffHunkSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif
#endif
