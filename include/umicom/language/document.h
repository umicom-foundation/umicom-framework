/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/document.h
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

typedef struct UmiLanguageDocumentRegistry UmiLanguageDocumentRegistry;

UmiStatus umi_language_document_registry_create(UmiLanguageDocumentRegistry **out_registry);
void umi_language_document_registry_destroy(UmiLanguageDocumentRegistry *registry);
UmiStatus umi_language_document_registry_upsert(UmiLanguageDocumentRegistry *registry, const UmiLanguageDocumentSnapshot *item);
UmiStatus umi_language_document_registry_remove(UmiLanguageDocumentRegistry *registry, const char *id);
UmiStatus umi_language_document_registry_find(const UmiLanguageDocumentRegistry *registry, const char *id, UmiLanguageDocumentSnapshot *out_item);
UmiStatus umi_language_document_registry_at(const UmiLanguageDocumentRegistry *registry, size_t index, UmiLanguageDocumentSnapshot *out_item);
size_t umi_language_document_registry_count(const UmiLanguageDocumentRegistry *registry);
uint64_t umi_language_document_registry_revision(const UmiLanguageDocumentRegistry *registry);
void umi_language_document_registry_clear(UmiLanguageDocumentRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
