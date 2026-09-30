/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/fold_region.h
 *
 * PURPOSE:
 *   Define folding regions without coupling language analysis to a text widget.
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
#ifndef UMICOM_EDITOR_FOLD_REGION_H
#define UMICOM_EDITOR_FOLD_REGION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_FOLD_REGION_CAPACITY 4096U
/**
 * Represent the editor fold region snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorFoldRegionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char kind[64];
    uint64_t start_line;
    uint64_t end_line;
    int collapsed;
    uint64_t revision;
} UmiEditorFoldRegionSnapshot;
/**
 * Represent the editor fold region registry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorFoldRegionRegistry UmiEditorFoldRegionRegistry;
/**
 * Initialise editor fold region registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_fold_region_registry_create(UmiEditorFoldRegionRegistry **out_registry);
/**
 * Release or reset state held by editor fold region registry so the same storage can be
 * reused safely.
 */
void umi_editor_fold_region_registry_destroy(UmiEditorFoldRegionRegistry *registry);
/**
 * Provide the editor fold region registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_fold_region_registry_upsert(UmiEditorFoldRegionRegistry *registry,const UmiEditorFoldRegionSnapshot *item);
/**
 * Remove editor fold region registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_fold_region_registry_remove(UmiEditorFoldRegionRegistry *registry,const char *id);
/**
 * Find editor fold region registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_fold_region_registry_find(const UmiEditorFoldRegionRegistry *registry,const char *id,UmiEditorFoldRegionSnapshot *out_item);
/**
 * Find editor fold region registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_fold_region_registry_at(const UmiEditorFoldRegionRegistry *registry,size_t index,UmiEditorFoldRegionSnapshot *out_item);
/**
 * Return the number of records represented by editor fold region registry without changing
 * their state.
 */
size_t umi_editor_fold_region_registry_count(const UmiEditorFoldRegionRegistry *registry);
/**
 * Provide the editor fold region registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_fold_region_registry_revision(const UmiEditorFoldRegionRegistry *registry);

/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_editor_fold_region_snapshot_validate(const UmiEditorFoldRegionSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_EDITOR_FOLD_REGION_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_editor_fold_region_registry_upsert_many(UmiEditorFoldRegionRegistry *registry,
    const UmiEditorFoldRegionSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif
#endif
