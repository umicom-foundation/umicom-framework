/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/definition.h
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
#ifndef UMICOM_LANGUAGE_DEFINITION_H
#define UMICOM_LANGUAGE_DEFINITION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_DEFINITION_CAPACITY 2048U
#define UMI_LANGUAGE_DEFINITION_API_VERSION 1U

typedef struct UmiLanguageDefinitionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char file_extensions[256];
    char mime_types[256];
    char language_server[512];
    char formatter[512];
    int enabled;
    uint64_t revision;
} UmiLanguageDefinitionSnapshot;

typedef struct UmiLanguageDefinitionRegistry UmiLanguageDefinitionRegistry;

UmiStatus umi_language_definition_registry_create(UmiLanguageDefinitionRegistry **out_registry);
void umi_language_definition_registry_destroy(UmiLanguageDefinitionRegistry *registry);
UmiStatus umi_language_definition_registry_upsert(UmiLanguageDefinitionRegistry *registry, const UmiLanguageDefinitionSnapshot *item);
UmiStatus umi_language_definition_registry_remove(UmiLanguageDefinitionRegistry *registry, const char *id);
UmiStatus umi_language_definition_registry_find(const UmiLanguageDefinitionRegistry *registry, const char *id, UmiLanguageDefinitionSnapshot *out_item);
UmiStatus umi_language_definition_registry_at(const UmiLanguageDefinitionRegistry *registry, size_t index, UmiLanguageDefinitionSnapshot *out_item);
size_t umi_language_definition_registry_count(const UmiLanguageDefinitionRegistry *registry);
uint64_t umi_language_definition_registry_revision(const UmiLanguageDefinitionRegistry *registry);
void umi_language_definition_registry_clear(UmiLanguageDefinitionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
