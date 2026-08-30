/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/signature.h
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
#ifndef UMICOM_LANGUAGE_SIGNATURE_H
#define UMICOM_LANGUAGE_SIGNATURE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_SIGNATURE_CAPACITY 2048U
#define UMI_LANGUAGE_SIGNATURE_API_VERSION 1U

typedef struct UmiLanguageSignatureSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char label[512];
    char documentation[1024];
    uint32_t active_parameter;
    uint32_t line;
    uint32_t column;
    uint64_t revision;
} UmiLanguageSignatureSnapshot;

typedef struct UmiLanguageSignatureRegistry UmiLanguageSignatureRegistry;

UmiStatus umi_language_signature_registry_create(UmiLanguageSignatureRegistry **out_registry);
void umi_language_signature_registry_destroy(UmiLanguageSignatureRegistry *registry);
UmiStatus umi_language_signature_registry_upsert(UmiLanguageSignatureRegistry *registry, const UmiLanguageSignatureSnapshot *item);
UmiStatus umi_language_signature_registry_remove(UmiLanguageSignatureRegistry *registry, const char *id);
UmiStatus umi_language_signature_registry_find(const UmiLanguageSignatureRegistry *registry, const char *id, UmiLanguageSignatureSnapshot *out_item);
UmiStatus umi_language_signature_registry_at(const UmiLanguageSignatureRegistry *registry, size_t index, UmiLanguageSignatureSnapshot *out_item);
size_t umi_language_signature_registry_count(const UmiLanguageSignatureRegistry *registry);
uint64_t umi_language_signature_registry_revision(const UmiLanguageSignatureRegistry *registry);
void umi_language_signature_registry_clear(UmiLanguageSignatureRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
