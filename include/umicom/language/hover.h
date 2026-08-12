/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/hover.h
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
#ifndef UMICOM_LANGUAGE_HOVER_H
#define UMICOM_LANGUAGE_HOVER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_HOVER_CAPACITY 2048U
#define UMI_LANGUAGE_HOVER_API_VERSION 1U

typedef struct UmiLanguageHoverSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char contents[2048];
    uint32_t line;
    uint32_t column;
    uint32_t start_line;
    uint32_t start_column;
    uint32_t end_line;
    uint32_t end_column;
    uint64_t revision;
} UmiLanguageHoverSnapshot;

typedef struct UmiLanguageHoverRegistry UmiLanguageHoverRegistry;

UmiStatus umi_language_hover_registry_create(UmiLanguageHoverRegistry **out_registry);
void umi_language_hover_registry_destroy(UmiLanguageHoverRegistry *registry);
UmiStatus umi_language_hover_registry_upsert(UmiLanguageHoverRegistry *registry, const UmiLanguageHoverSnapshot *item);
UmiStatus umi_language_hover_registry_remove(UmiLanguageHoverRegistry *registry, const char *id);
UmiStatus umi_language_hover_registry_find(const UmiLanguageHoverRegistry *registry, const char *id, UmiLanguageHoverSnapshot *out_item);
UmiStatus umi_language_hover_registry_at(const UmiLanguageHoverRegistry *registry, size_t index, UmiLanguageHoverSnapshot *out_item);
size_t umi_language_hover_registry_count(const UmiLanguageHoverRegistry *registry);
uint64_t umi_language_hover_registry_revision(const UmiLanguageHoverRegistry *registry);
void umi_language_hover_registry_clear(UmiLanguageHoverRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
