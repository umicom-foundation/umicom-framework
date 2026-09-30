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

#ifdef __cplusplus
}
#endif

#endif
