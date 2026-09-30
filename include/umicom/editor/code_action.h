/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/code_action.h
 *
 * PURPOSE:
 *   Define editor code actions that execute through canonical command IDs.
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
#ifndef UMICOM_EDITOR_CODE_ACTION_H
#define UMICOM_EDITOR_CODE_ACTION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_CODE_ACTION_CAPACITY 4096U
/**
 * Represent the editor code action snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorCodeActionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char title[512];
    char kind[128];
    char command_id[128];
    char argument[1024];
    int preferred;
    int enabled;
    uint64_t revision;
} UmiEditorCodeActionSnapshot;
/**
 * Represent the editor code action registry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorCodeActionRegistry UmiEditorCodeActionRegistry;
/**
 * Initialise editor code action registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_code_action_registry_create(UmiEditorCodeActionRegistry **out_registry);
/**
 * Release or reset state held by editor code action registry so the same storage can be
 * reused safely.
 */
void umi_editor_code_action_registry_destroy(UmiEditorCodeActionRegistry *registry);
/**
 * Provide the editor code action registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_code_action_registry_upsert(UmiEditorCodeActionRegistry *registry,const UmiEditorCodeActionSnapshot *item);
/**
 * Remove editor code action registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_code_action_registry_remove(UmiEditorCodeActionRegistry *registry,const char *id);
/**
 * Find editor code action registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_code_action_registry_find(const UmiEditorCodeActionRegistry *registry,const char *id,UmiEditorCodeActionSnapshot *out_item);
/**
 * Find editor code action registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_code_action_registry_at(const UmiEditorCodeActionRegistry *registry,size_t index,UmiEditorCodeActionSnapshot *out_item);
/**
 * Return the number of records represented by editor code action registry without changing
 * their state.
 */
size_t umi_editor_code_action_registry_count(const UmiEditorCodeActionRegistry *registry);
/**
 * Provide the editor code action registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_code_action_registry_revision(const UmiEditorCodeActionRegistry *registry);

/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_editor_code_action_snapshot_validate(const UmiEditorCodeActionSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_EDITOR_CODE_ACTION_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_editor_code_action_registry_upsert_many(UmiEditorCodeActionRegistry *registry,
    const UmiEditorCodeActionSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

/** Atomically replace all records belonging to one nonempty document ID.
 * expected_revision must match the current registry. Other documents retain
 * their records, order and record revisions; replacements follow them in input
 * order. Each removal and insertion advances the registry revision once.
 * Empty input clears only this document, and is a no-op when it has no records.
 * Every supplied row must match document_id. An ID owned by another document
 * returns PERMISSION_DENIED; duplicate input IDs return ALREADY_EXISTS. Text,
 * capacity, stale revision, overflow or allocation failure publishes nothing.
 * Inputs are borrowed and unchanged. outResult follows upsert_many's lifetime
 * and alias restrictions. Call only on the owning thread. No I/O is performed. */
UmiStatus umi_editor_code_action_registry_replace_document(UmiEditorCodeActionRegistry *registry,
    const char *document_id, uint64_t expected_revision,
    const UmiEditorCodeActionSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif
#endif
