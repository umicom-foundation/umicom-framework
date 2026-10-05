/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/code_action.h
 *
 * PURPOSE:
 *   Define a provider-neutral language-intelligence record that can be backed by LSP, native analysers or future Umicom language engines.
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
#ifndef UMICOM_LANGUAGE_CODE_ACTION_H
#define UMICOM_LANGUAGE_CODE_ACTION_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_CODE_ACTION_CAPACITY 2048U
#define UMI_LANGUAGE_CODE_ACTION_API_VERSION 1U

/**
 * Represent the language code action snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageCodeActionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char title[512];
    char kind[128];
    char command_id[128];
    char argument[1024];
    int preferred;
    uint64_t revision;
} UmiLanguageCodeActionSnapshot;

/**
 * Represent the language code action registry data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageCodeActionRegistry UmiLanguageCodeActionRegistry;

/**
 * Initialise language code action registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_code_action_registry_create(UmiLanguageCodeActionRegistry **out_registry);
/**
 * Release or reset state held by language code action registry so the same storage can be
 * reused safely.
 */
void umi_language_code_action_registry_destroy(UmiLanguageCodeActionRegistry *registry);
/**
 * Provide the language code action registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_code_action_registry_upsert(UmiLanguageCodeActionRegistry *registry, const UmiLanguageCodeActionSnapshot *item);
/**
 * Remove language code action registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_code_action_registry_remove(UmiLanguageCodeActionRegistry *registry, const char *id);
/**
 * Find language code action registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_code_action_registry_find(const UmiLanguageCodeActionRegistry *registry, const char *id, UmiLanguageCodeActionSnapshot *out_item);
/**
 * Find language code action registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_code_action_registry_at(const UmiLanguageCodeActionRegistry *registry, size_t index, UmiLanguageCodeActionSnapshot *out_item);
/**
 * Return the number of records represented by language code action registry without
 * changing their state.
 */
size_t umi_language_code_action_registry_count(const UmiLanguageCodeActionRegistry *registry);
/**
 * Provide the language code action registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_code_action_registry_revision(const UmiLanguageCodeActionRegistry *registry);
/**
 * Release or reset state held by language code action registry so the same storage can be
 * reused safely.
 */
void umi_language_code_action_registry_clear(UmiLanguageCodeActionRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_language_code_action_snapshot_validate(const UmiLanguageCodeActionSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_LANGUAGE_CODE_ACTION_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_language_code_action_registry_upsert_many(UmiLanguageCodeActionRegistry *registry,
    const UmiLanguageCodeActionSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

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
UmiStatus umi_language_code_action_registry_replace_document(UmiLanguageCodeActionRegistry *registry,
    const char *document_id, uint64_t expected_revision,
    const UmiLanguageCodeActionSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_language_code_action_registry_capture(const UmiLanguageCodeActionRegistry *registry,
    UmiLanguageCodeActionSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_language_code_action_registry_replace_if_current(UmiLanguageCodeActionRegistry *registry,
    uint64_t expected_revision, const UmiLanguageCodeActionSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);


/** One reviewed change. For REMOVE, initialise item.id; its other fields are
 * ignored. For UPSERT, supply a complete record under the usual domain rules.
 * The edit remains caller-owned and is never changed by publication. */
typedef struct UmiLanguageCodeActionEdit {
    UmiSnapshotEditKind kind;
    UmiLanguageCodeActionSnapshot item;
} UmiLanguageCodeActionEdit;

/** Apply this ordered list only if expected_revision still matches this owner.
 * A stale review returns INVALID_STATE, including for an empty list. Each ID
 * may appear once; duplicates return ALREADY_EXISTS. A missing removal returns
 * NOT_FOUND. Upserts retain normal field normalisation and capacity checks;
 * when full, remove an existing row before adding its replacement.
 * At most twice UMI_LANGUAGE_CODE_ACTION_CAPACITY edits are accepted. Success publishes
 * all changes together, advances once, and stamps every remaining row with
 * that revision. An empty current list is a no-op. Every refusal preserves
 * the previous collection. out_result is optional and identifies a rejected
 * edit where possible; validation describes text and duplicate-ID errors.
 * Inputs/result must not overlap each other or the owner. Serialize access;
 * staging allocates one registry and performs no I/O or external callbacks. */
UmiStatus umi_language_code_action_registry_edit_if_current(UmiLanguageCodeActionRegistry *registry,
    uint64_t expected_revision, const UmiLanguageCodeActionEdit *edits, size_t count,
    UmiSnapshotBatchResult *out_result);

/** Read up to capacity records starting at offset, at expected_revision.
 * Obtain the first revision with umi_language_code_action_registry_revision and retain it for
 * every page. INVALID_STATE means the registry changed: discard the partial
 * collection and start a new observation. Offsets past the count are invalid;
 * an offset equal to the count returns an empty final page. A zero capacity
 * permits null items and returns metadata only; it does not advance the cursor.
 * All refusals leave items and out_page unchanged. Success copies accepted
 * values in registry order without changing the owner. items must have room
 * for capacity records; out_page is required. Output storage must not overlap
 * the registry, each other, or concurrently used storage. This performs no
 * allocation or I/O. Serialize all calls and mutations on the registry owner. */
UmiStatus umi_language_code_action_registry_read_page(const UmiLanguageCodeActionRegistry *registry,
    uint64_t expected_revision, size_t offset, UmiLanguageCodeActionSnapshot *items,
    size_t capacity, UmiSnapshotPage *out_page);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_language_code_action_snapshot_archive_encode(const UmiLanguageCodeActionSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_language_code_action_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiLanguageCodeActionSnapshot *value);

/** Encode the ordered collection only at expected_revision. Use the same
 * revision for measurement and encoding; an intervening edit is INVALID_STATE.
 * Short output reports the required size without writing bytes. The common
 * archive byte limit applies. Serialize access on the owning thread. */
UmiStatus umi_language_code_action_registry_archive_encode(const UmiLanguageCodeActionRegistry *registry,
    uint64_t expected_revision, void *bytes, size_t capacity, size_t *out_size);
/** Restore all rows only at the caller's current observation revision.
 * Every row is decoded before the existing replace_if_current operation
 * validates duplicate identities and publishes the collection atomically.
 * Corrupt, incompatible, oversized or stale input leaves the owner unchanged.
 * Publication assigns fresh local revisions; saved row counters grant no
 * authority. An empty archive deliberately clears the collection. This uses
 * bounded staging allocation plus the replacement owner's scratch storage.
 * Optional out_result is initialized on every return; rejected_index identifies
 * a bad row when available. Its validation detail describes domain publication,
 * while the return status describes envelope/codec failures. Keep input,
 * result and registry storage separate. No files, callbacks or commands run. */
UmiStatus umi_language_code_action_registry_archive_restore(UmiLanguageCodeActionRegistry *registry,
    uint64_t expected_revision, const void *bytes, size_t byte_count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
