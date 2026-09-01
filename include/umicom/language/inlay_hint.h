/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/inlay_hint.h
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
#ifndef UMICOM_LANGUAGE_INLAY_HINT_H
#define UMICOM_LANGUAGE_INLAY_HINT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INLAY_HINT_CAPACITY 2048U
#define UMI_LANGUAGE_INLAY_HINT_API_VERSION 1U

typedef struct UmiLanguageInlayHintSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char label[512];
    char kind[64];
    uint32_t line;
    uint32_t column;
    int visible;
    uint64_t revision;
} UmiLanguageInlayHintSnapshot;

typedef struct UmiLanguageInlayHintRegistry UmiLanguageInlayHintRegistry;

UmiStatus umi_language_inlay_hint_registry_create(UmiLanguageInlayHintRegistry **out_registry);
void umi_language_inlay_hint_registry_destroy(UmiLanguageInlayHintRegistry *registry);
UmiStatus umi_language_inlay_hint_registry_upsert(UmiLanguageInlayHintRegistry *registry, const UmiLanguageInlayHintSnapshot *item);
UmiStatus umi_language_inlay_hint_registry_remove(UmiLanguageInlayHintRegistry *registry, const char *id);
UmiStatus umi_language_inlay_hint_registry_find(const UmiLanguageInlayHintRegistry *registry, const char *id, UmiLanguageInlayHintSnapshot *out_item);
UmiStatus umi_language_inlay_hint_registry_at(const UmiLanguageInlayHintRegistry *registry, size_t index, UmiLanguageInlayHintSnapshot *out_item);
size_t umi_language_inlay_hint_registry_count(const UmiLanguageInlayHintRegistry *registry);
uint64_t umi_language_inlay_hint_registry_revision(const UmiLanguageInlayHintRegistry *registry);
void umi_language_inlay_hint_registry_clear(UmiLanguageInlayHintRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
