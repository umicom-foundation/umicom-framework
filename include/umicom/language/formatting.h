/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/formatting.h
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
#ifndef UMICOM_LANGUAGE_FORMATTING_H
#define UMICOM_LANGUAGE_FORMATTING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_FORMATTING_CAPACITY 2048U
#define UMI_LANGUAGE_FORMATTING_API_VERSION 1U

typedef struct UmiLanguageFormattingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char provider_id[128];
    char mode[64];
    uint32_t tab_size;
    int insert_spaces;
    int available;
    uint64_t revision;
} UmiLanguageFormattingSnapshot;

typedef struct UmiLanguageFormattingRegistry UmiLanguageFormattingRegistry;

UmiStatus umi_language_formatting_registry_create(UmiLanguageFormattingRegistry **out_registry);
void umi_language_formatting_registry_destroy(UmiLanguageFormattingRegistry *registry);
UmiStatus umi_language_formatting_registry_upsert(UmiLanguageFormattingRegistry *registry, const UmiLanguageFormattingSnapshot *item);
UmiStatus umi_language_formatting_registry_remove(UmiLanguageFormattingRegistry *registry, const char *id);
UmiStatus umi_language_formatting_registry_find(const UmiLanguageFormattingRegistry *registry, const char *id, UmiLanguageFormattingSnapshot *out_item);
UmiStatus umi_language_formatting_registry_at(const UmiLanguageFormattingRegistry *registry, size_t index, UmiLanguageFormattingSnapshot *out_item);
size_t umi_language_formatting_registry_count(const UmiLanguageFormattingRegistry *registry);
uint64_t umi_language_formatting_registry_revision(const UmiLanguageFormattingRegistry *registry);
void umi_language_formatting_registry_clear(UmiLanguageFormattingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
