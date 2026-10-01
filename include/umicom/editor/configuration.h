/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/configuration.h
 *
 * PURPOSE:
 *   Define reusable per-language and per-workspace editor configuration snapshots.
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
#ifndef UMICOM_EDITOR_CONFIGURATION_H
#define UMICOM_EDITOR_CONFIGURATION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_CONFIGURATION_CAPACITY 512U
/**
 * Represent the editor configuration snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorConfigurationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char language_id[128];
    uint32_t tab_size;
    int insert_spaces;
    int word_wrap;
    int line_numbers;
    int minimap;
    int auto_indent;
    int format_on_save;
    uint64_t revision;
} UmiEditorConfigurationSnapshot;
/**
 * Represent the editor configuration registry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorConfigurationRegistry UmiEditorConfigurationRegistry;
/**
 * Initialise editor configuration registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_configuration_registry_create(UmiEditorConfigurationRegistry **out_registry);
/**
 * Release or reset state held by editor configuration registry so the same storage can be
 * reused safely.
 */
void umi_editor_configuration_registry_destroy(UmiEditorConfigurationRegistry *registry);
/**
 * Provide the editor configuration registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_configuration_registry_upsert(UmiEditorConfigurationRegistry *registry,const UmiEditorConfigurationSnapshot *item);
/**
 * Remove editor configuration registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_configuration_registry_remove(UmiEditorConfigurationRegistry *registry,const char *id);
/**
 * Find editor configuration registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_configuration_registry_find(const UmiEditorConfigurationRegistry *registry,const char *id,UmiEditorConfigurationSnapshot *out_item);
/**
 * Find editor configuration registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_configuration_registry_at(const UmiEditorConfigurationRegistry *registry,size_t index,UmiEditorConfigurationSnapshot *out_item);
/**
 * Return the number of records represented by editor configuration registry without
 * changing their state.
 */
size_t umi_editor_configuration_registry_count(const UmiEditorConfigurationRegistry *registry);
/**
 * Provide the editor configuration registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_configuration_registry_revision(const UmiEditorConfigurationRegistry *registry);

/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_editor_configuration_snapshot_validate(const UmiEditorConfigurationSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_EDITOR_CONFIGURATION_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_editor_configuration_registry_upsert_many(UmiEditorConfigurationRegistry *registry,
    const UmiEditorConfigurationSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_editor_configuration_registry_capture(const UmiEditorConfigurationRegistry *registry,
    UmiEditorConfigurationSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_editor_configuration_registry_replace_if_current(UmiEditorConfigurationRegistry *registry,
    uint64_t expected_revision, const UmiEditorConfigurationSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);


/** One reviewed change. For REMOVE, initialise item.id; its other fields are
 * ignored. For UPSERT, supply a complete record under the usual domain rules.
 * The edit remains caller-owned and is never changed by publication. */
typedef struct UmiEditorConfigurationEdit {
    UmiSnapshotEditKind kind;
    UmiEditorConfigurationSnapshot item;
} UmiEditorConfigurationEdit;

/** Apply this ordered list only if expected_revision still matches this owner.
 * A stale review returns INVALID_STATE, including for an empty list. Each ID
 * may appear once; duplicates return ALREADY_EXISTS. A missing removal returns
 * NOT_FOUND. Upserts retain normal field normalisation and capacity checks;
 * when full, remove an existing row before adding its replacement.
 * At most twice UMI_EDITOR_CONFIGURATION_CAPACITY edits are accepted. Success publishes
 * all changes together, advances once, and stamps every remaining row with
 * that revision. An empty current list is a no-op. Every refusal preserves
 * the previous collection. out_result is optional and identifies a rejected
 * edit where possible; validation describes text and duplicate-ID errors.
 * Inputs/result must not overlap each other or the owner. Serialize access;
 * staging allocates one registry and performs no I/O or external callbacks. */
UmiStatus umi_editor_configuration_registry_edit_if_current(UmiEditorConfigurationRegistry *registry,
    uint64_t expected_revision, const UmiEditorConfigurationEdit *edits, size_t count,
    UmiSnapshotBatchResult *out_result);

/** Read up to capacity records starting at offset, at expected_revision.
 * Obtain the first revision with umi_editor_configuration_registry_revision and retain it for
 * every page. INVALID_STATE means the registry changed: discard the partial
 * collection and start a new observation. Offsets past the count are invalid;
 * an offset equal to the count returns an empty final page. A zero capacity
 * permits null items and returns metadata only; it does not advance the cursor.
 * All refusals leave items and out_page unchanged. Success copies accepted
 * values in registry order without changing the owner. items must have room
 * for capacity records; out_page is required. Output storage must not overlap
 * the registry, each other, or concurrently used storage. This performs no
 * allocation or I/O. Serialize all calls and mutations on the registry owner. */
UmiStatus umi_editor_configuration_registry_read_page(const UmiEditorConfigurationRegistry *registry,
    uint64_t expected_revision, size_t offset, UmiEditorConfigurationSnapshot *items,
    size_t capacity, UmiSnapshotPage *out_page);

#ifdef __cplusplus
}
#endif
#endif
