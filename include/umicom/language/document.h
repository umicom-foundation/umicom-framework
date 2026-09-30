/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/document.h
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
#ifndef UMICOM_LANGUAGE_DOCUMENT_H
#define UMICOM_LANGUAGE_DOCUMENT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_DOCUMENT_CAPACITY 2048U
#define UMI_LANGUAGE_DOCUMENT_API_VERSION 1U

/**
 * Represent the language document snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageDocumentSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char uri[1024];
    char language_id[128];
    uint64_t version;
    size_t line_count;
    int open;
    int dirty;
    uint64_t revision;
} UmiLanguageDocumentSnapshot;

/**
 * Represent the language document registry data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageDocumentRegistry UmiLanguageDocumentRegistry;

/**
 * Initialise language document registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_document_registry_create(UmiLanguageDocumentRegistry **out_registry);
/**
 * Release or reset state held by language document registry so the same storage can be
 * reused safely.
 */
void umi_language_document_registry_destroy(UmiLanguageDocumentRegistry *registry);
/**
 * Provide the language document registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_document_registry_upsert(UmiLanguageDocumentRegistry *registry, const UmiLanguageDocumentSnapshot *item);
/**
 * Remove language document registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_document_registry_remove(UmiLanguageDocumentRegistry *registry, const char *id);
/**
 * Find language document registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_document_registry_find(const UmiLanguageDocumentRegistry *registry, const char *id, UmiLanguageDocumentSnapshot *out_item);
/**
 * Find language document registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_document_registry_at(const UmiLanguageDocumentRegistry *registry, size_t index, UmiLanguageDocumentSnapshot *out_item);
/**
 * Return the number of records represented by language document registry without changing
 * their state.
 */
size_t umi_language_document_registry_count(const UmiLanguageDocumentRegistry *registry);
/**
 * Provide the language document registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_document_registry_revision(const UmiLanguageDocumentRegistry *registry);
/**
 * Release or reset state held by language document registry so the same storage can be
 * reused safely.
 */
void umi_language_document_registry_clear(UmiLanguageDocumentRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_language_document_snapshot_validate(const UmiLanguageDocumentSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_LANGUAGE_DOCUMENT_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_language_document_registry_upsert_many(UmiLanguageDocumentRegistry *registry,
    const UmiLanguageDocumentSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
