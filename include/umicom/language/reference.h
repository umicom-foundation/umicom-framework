/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/reference.h
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
#ifndef UMICOM_LANGUAGE_REFERENCE_H
#define UMICOM_LANGUAGE_REFERENCE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_REFERENCE_CAPACITY 2048U
#define UMI_LANGUAGE_REFERENCE_API_VERSION 1U

typedef struct UmiLanguageReferenceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char symbol_id[128];
    char document_id[128];
    char uri[1024];
    uint32_t line;
    uint32_t column;
    int definition;
    uint64_t revision;
} UmiLanguageReferenceSnapshot;

typedef struct UmiLanguageReferenceRegistry UmiLanguageReferenceRegistry;

UmiStatus umi_language_reference_registry_create(UmiLanguageReferenceRegistry **out_registry);
void umi_language_reference_registry_destroy(UmiLanguageReferenceRegistry *registry);
UmiStatus umi_language_reference_registry_upsert(UmiLanguageReferenceRegistry *registry, const UmiLanguageReferenceSnapshot *item);
UmiStatus umi_language_reference_registry_remove(UmiLanguageReferenceRegistry *registry, const char *id);
UmiStatus umi_language_reference_registry_find(const UmiLanguageReferenceRegistry *registry, const char *id, UmiLanguageReferenceSnapshot *out_item);
UmiStatus umi_language_reference_registry_at(const UmiLanguageReferenceRegistry *registry, size_t index, UmiLanguageReferenceSnapshot *out_item);
size_t umi_language_reference_registry_count(const UmiLanguageReferenceRegistry *registry);
uint64_t umi_language_reference_registry_revision(const UmiLanguageReferenceRegistry *registry);
void umi_language_reference_registry_clear(UmiLanguageReferenceRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
