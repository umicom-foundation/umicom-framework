/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/completion.h
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
#ifndef UMICOM_LANGUAGE_COMPLETION_H
#define UMICOM_LANGUAGE_COMPLETION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_COMPLETION_CAPACITY 2048U
#define UMI_LANGUAGE_COMPLETION_API_VERSION 1U

/**
 * Represent the language completion snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageCompletionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char label[256];
    char detail[512];
    char insert_text[1024];
    char kind[64];
    char sort_text[256];
    uint32_t line;
    uint32_t column;
    uint64_t revision;
} UmiLanguageCompletionSnapshot;

/**
 * Represent the language completion registry data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageCompletionRegistry UmiLanguageCompletionRegistry;

/**
 * Initialise language completion registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_completion_registry_create(UmiLanguageCompletionRegistry **out_registry);
/**
 * Release or reset state held by language completion registry so the same storage can be
 * reused safely.
 */
void umi_language_completion_registry_destroy(UmiLanguageCompletionRegistry *registry);
/**
 * Provide the language completion registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_completion_registry_upsert(UmiLanguageCompletionRegistry *registry, const UmiLanguageCompletionSnapshot *item);
/**
 * Remove language completion registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_completion_registry_remove(UmiLanguageCompletionRegistry *registry, const char *id);
/**
 * Find language completion registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_completion_registry_find(const UmiLanguageCompletionRegistry *registry, const char *id, UmiLanguageCompletionSnapshot *out_item);
/**
 * Find language completion registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_completion_registry_at(const UmiLanguageCompletionRegistry *registry, size_t index, UmiLanguageCompletionSnapshot *out_item);
/**
 * Return the number of records represented by language completion registry without
 * changing their state.
 */
size_t umi_language_completion_registry_count(const UmiLanguageCompletionRegistry *registry);
/**
 * Provide the language completion registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_completion_registry_revision(const UmiLanguageCompletionRegistry *registry);
/**
 * Release or reset state held by language completion registry so the same storage can be
 * reused safely.
 */
void umi_language_completion_registry_clear(UmiLanguageCompletionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
