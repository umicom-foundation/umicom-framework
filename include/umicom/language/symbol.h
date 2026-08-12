/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/symbol.h
 *
 * PURPOSE:
 *   Define a provider-neutral language-intelligence record that can be backed by LSP, native analysers or future Umicom language engines.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiLanguageSymbolRegistry UmiLanguageSymbolRegistry;

UmiStatus umi_language_symbol_registry_create(UmiLanguageSymbolRegistry **out_registry);
void umi_language_symbol_registry_destroy(UmiLanguageSymbolRegistry *registry);
UmiStatus umi_language_symbol_registry_upsert(UmiLanguageSymbolRegistry *registry, const UmiLanguageSymbolSnapshot *item);
UmiStatus umi_language_symbol_registry_remove(UmiLanguageSymbolRegistry *registry, const char *id);
UmiStatus umi_language_symbol_registry_find(const UmiLanguageSymbolRegistry *registry, const char *id, UmiLanguageSymbolSnapshot *out_item);
UmiStatus umi_language_symbol_registry_at(const UmiLanguageSymbolRegistry *registry, size_t index, UmiLanguageSymbolSnapshot *out_item);
size_t umi_language_symbol_registry_count(const UmiLanguageSymbolRegistry *registry);
uint64_t umi_language_symbol_registry_revision(const UmiLanguageSymbolRegistry *registry);
void umi_language_symbol_registry_clear(UmiLanguageSymbolRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
