/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/semantic_token.h
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
#ifndef UMICOM_LANGUAGE_SEMANTIC_TOKEN_H
#define UMICOM_LANGUAGE_SEMANTIC_TOKEN_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_SEMANTIC_TOKEN_CAPACITY 2048U
#define UMI_LANGUAGE_SEMANTIC_TOKEN_API_VERSION 1U

/**
 * Represent the language semantic token snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageSemanticTokenSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char token_type[128];
    char modifiers[256];
    uint32_t line;
    uint32_t column;
    uint32_t length;
    uint64_t revision;
} UmiLanguageSemanticTokenSnapshot;

/**
 * Represent the language semantic token registry data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageSemanticTokenRegistry UmiLanguageSemanticTokenRegistry;

/**
 * Initialise language semantic token registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_semantic_token_registry_create(UmiLanguageSemanticTokenRegistry **out_registry);
/**
 * Release or reset state held by language semantic token registry so the same storage can
 * be reused safely.
 */
void umi_language_semantic_token_registry_destroy(UmiLanguageSemanticTokenRegistry *registry);
/**
 * Provide the language semantic token registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_semantic_token_registry_upsert(UmiLanguageSemanticTokenRegistry *registry, const UmiLanguageSemanticTokenSnapshot *item);
/**
 * Remove language semantic token registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_language_semantic_token_registry_remove(UmiLanguageSemanticTokenRegistry *registry, const char *id);
/**
 * Find language semantic token registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_language_semantic_token_registry_find(const UmiLanguageSemanticTokenRegistry *registry, const char *id, UmiLanguageSemanticTokenSnapshot *out_item);
/**
 * Find language semantic token registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_language_semantic_token_registry_at(const UmiLanguageSemanticTokenRegistry *registry, size_t index, UmiLanguageSemanticTokenSnapshot *out_item);
/**
 * Return the number of records represented by language semantic token registry without
 * changing their state.
 */
size_t umi_language_semantic_token_registry_count(const UmiLanguageSemanticTokenRegistry *registry);
/**
 * Provide the language semantic token registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_language_semantic_token_registry_revision(const UmiLanguageSemanticTokenRegistry *registry);
/**
 * Release or reset state held by language semantic token registry so the same storage can
 * be reused safely.
 */
void umi_language_semantic_token_registry_clear(UmiLanguageSemanticTokenRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
