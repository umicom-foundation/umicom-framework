/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/reference.h
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
#ifndef UMICOM_LANGUAGE_REFERENCE_H
#define UMICOM_LANGUAGE_REFERENCE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_REFERENCE_CAPACITY 2048U
#define UMI_LANGUAGE_REFERENCE_API_VERSION 1U

/**
 * Represent the language reference snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageReferenceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char symbol_id[128];
    char document_id[128];
    char uri[1024];
    uint32_t line;
    uint32_t column;
    int definition;
    uint64_t revision;
} UmiLanguageReferenceSnapshot;

/**
 * Represent the language reference registry data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageReferenceRegistry UmiLanguageReferenceRegistry;

/**
 * Initialise language reference registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_reference_registry_create(UmiLanguageReferenceRegistry **out_registry);
/**
 * Release or reset state held by language reference registry so the same storage can be
 * reused safely.
 */
void umi_language_reference_registry_destroy(UmiLanguageReferenceRegistry *registry);
/**
 * Provide the language reference registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_reference_registry_upsert(UmiLanguageReferenceRegistry *registry, const UmiLanguageReferenceSnapshot *item);
/**
 * Remove language reference registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_reference_registry_remove(UmiLanguageReferenceRegistry *registry, const char *id);
/**
 * Find language reference registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_reference_registry_find(const UmiLanguageReferenceRegistry *registry, const char *id, UmiLanguageReferenceSnapshot *out_item);
/**
 * Find language reference registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_reference_registry_at(const UmiLanguageReferenceRegistry *registry, size_t index, UmiLanguageReferenceSnapshot *out_item);
/**
 * Return the number of records represented by language reference registry without changing
 * their state.
 */
size_t umi_language_reference_registry_count(const UmiLanguageReferenceRegistry *registry);
/**
 * Provide the language reference registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_reference_registry_revision(const UmiLanguageReferenceRegistry *registry);
/**
 * Release or reset state held by language reference registry so the same storage can be
 * reused safely.
 */
void umi_language_reference_registry_clear(UmiLanguageReferenceRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_language_reference_snapshot_validate(const UmiLanguageReferenceSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_LANGUAGE_REFERENCE_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_language_reference_registry_upsert_many(UmiLanguageReferenceRegistry *registry,
    const UmiLanguageReferenceSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

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
UmiStatus umi_language_reference_registry_replace_document(UmiLanguageReferenceRegistry *registry,
    const char *document_id, uint64_t expected_revision,
    const UmiLanguageReferenceSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);


/** Copy this registry's ordered records into caller-owned storage. Pass NULL
 * and zero capacity to query the required count. On a short destination, no
 * records are written and out_capture still reports the required count and
 * current revision. Other argument errors clear that metadata when supplied.
 * out_capture is required. Outputs must not overlap each other or the registry.
 * Serialize access on the owning thread; the result remains an independent
 * value copy when the registry later changes. No allocation or I/O occurs. */
UmiStatus umi_language_reference_registry_capture(const UmiLanguageReferenceRegistry *registry,
    UmiLanguageReferenceSnapshot *out_items, size_t capacity, UmiSnapshotCapture *out_capture);

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
UmiStatus umi_language_reference_registry_replace_if_current(UmiLanguageReferenceRegistry *registry,
    uint64_t expected_revision, const UmiLanguageReferenceSnapshot *items, size_t count,
    UmiSnapshotBatchResult *out_result);


/** One reviewed change. For REMOVE, initialise item.id; its other fields are
 * ignored. For UPSERT, supply a complete record under the usual domain rules.
 * The edit remains caller-owned and is never changed by publication. */
typedef struct UmiLanguageReferenceEdit {
    UmiSnapshotEditKind kind;
    UmiLanguageReferenceSnapshot item;
} UmiLanguageReferenceEdit;

/** Apply this ordered list only if expected_revision still matches this owner.
 * A stale review returns INVALID_STATE, including for an empty list. Each ID
 * may appear once; duplicates return ALREADY_EXISTS. A missing removal returns
 * NOT_FOUND. Upserts retain normal field normalisation and capacity checks;
 * when full, remove an existing row before adding its replacement.
 * At most twice UMI_LANGUAGE_REFERENCE_CAPACITY edits are accepted. Success publishes
 * all changes together, advances once, and stamps every remaining row with
 * that revision. An empty current list is a no-op. Every refusal preserves
 * the previous collection. out_result is optional and identifies a rejected
 * edit where possible; validation describes text and duplicate-ID errors.
 * Inputs/result must not overlap each other or the owner. Serialize access;
 * staging allocates one registry and performs no I/O or external callbacks. */
UmiStatus umi_language_reference_registry_edit_if_current(UmiLanguageReferenceRegistry *registry,
    uint64_t expected_revision, const UmiLanguageReferenceEdit *edits, size_t count,
    UmiSnapshotBatchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
