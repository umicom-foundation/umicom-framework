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

#ifdef __cplusplus
}
#endif

#endif
