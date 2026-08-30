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

/* BEGINNER NOTE:
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

typedef struct UmiLanguageCompletionRegistry UmiLanguageCompletionRegistry;

UmiStatus umi_language_completion_registry_create(UmiLanguageCompletionRegistry **out_registry);
void umi_language_completion_registry_destroy(UmiLanguageCompletionRegistry *registry);
UmiStatus umi_language_completion_registry_upsert(UmiLanguageCompletionRegistry *registry, const UmiLanguageCompletionSnapshot *item);
UmiStatus umi_language_completion_registry_remove(UmiLanguageCompletionRegistry *registry, const char *id);
UmiStatus umi_language_completion_registry_find(const UmiLanguageCompletionRegistry *registry, const char *id, UmiLanguageCompletionSnapshot *out_item);
UmiStatus umi_language_completion_registry_at(const UmiLanguageCompletionRegistry *registry, size_t index, UmiLanguageCompletionSnapshot *out_item);
size_t umi_language_completion_registry_count(const UmiLanguageCompletionRegistry *registry);
uint64_t umi_language_completion_registry_revision(const UmiLanguageCompletionRegistry *registry);
void umi_language_completion_registry_clear(UmiLanguageCompletionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
