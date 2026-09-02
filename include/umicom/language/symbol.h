/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/symbol.h
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
#ifndef UMICOM_LANGUAGE_SYMBOL_H
#define UMICOM_LANGUAGE_SYMBOL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_SYMBOL_CAPACITY 2048U
#define UMI_LANGUAGE_SYMBOL_API_VERSION 1U

/**
 * Represent the language symbol snapshot data shared with callers of this public contract.
 */
typedef struct UmiLanguageSymbolSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char name[256];
    char kind[64];
    char container[256];
    uint32_t line;
    uint32_t column;
    uint32_t end_line;
    uint32_t end_column;
    uint64_t revision;
} UmiLanguageSymbolSnapshot;

/**
 * Represent the language symbol registry data shared with callers of this public contract.
 */
typedef struct UmiLanguageSymbolRegistry UmiLanguageSymbolRegistry;

/**
 * Initialise language symbol registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_symbol_registry_create(UmiLanguageSymbolRegistry **out_registry);
/**
 * Release or reset state held by language symbol registry so the same storage can be
 * reused safely.
 */
void umi_language_symbol_registry_destroy(UmiLanguageSymbolRegistry *registry);
/**
 * Provide the language symbol registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_symbol_registry_upsert(UmiLanguageSymbolRegistry *registry, const UmiLanguageSymbolSnapshot *item);
/**
 * Remove language symbol registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_symbol_registry_remove(UmiLanguageSymbolRegistry *registry, const char *id);
/**
 * Find language symbol registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_symbol_registry_find(const UmiLanguageSymbolRegistry *registry, const char *id, UmiLanguageSymbolSnapshot *out_item);
/**
 * Find language symbol registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_symbol_registry_at(const UmiLanguageSymbolRegistry *registry, size_t index, UmiLanguageSymbolSnapshot *out_item);
/**
 * Return the number of records represented by language symbol registry without changing
 * their state.
 */
size_t umi_language_symbol_registry_count(const UmiLanguageSymbolRegistry *registry);
/**
 * Provide the language symbol registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_symbol_registry_revision(const UmiLanguageSymbolRegistry *registry);
/**
 * Release or reset state held by language symbol registry so the same storage can be
 * reused safely.
 */
void umi_language_symbol_registry_clear(UmiLanguageSymbolRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
