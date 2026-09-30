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

#ifdef __cplusplus
}
#endif

#endif
