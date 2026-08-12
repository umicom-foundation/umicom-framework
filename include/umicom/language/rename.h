/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/rename.h
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
#ifndef UMICOM_LANGUAGE_RENAME_H
#define UMICOM_LANGUAGE_RENAME_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_RENAME_CAPACITY 2048U
#define UMI_LANGUAGE_RENAME_API_VERSION 1U

typedef struct UmiLanguageRenameSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char symbol_id[128];
    char old_name[256];
    char new_name[256];
    char document_id[128];
    int state;
    size_t conflict_count;
    uint64_t revision;
} UmiLanguageRenameSnapshot;

typedef struct UmiLanguageRenameRegistry UmiLanguageRenameRegistry;

UmiStatus umi_language_rename_registry_create(UmiLanguageRenameRegistry **out_registry);
void umi_language_rename_registry_destroy(UmiLanguageRenameRegistry *registry);
UmiStatus umi_language_rename_registry_upsert(UmiLanguageRenameRegistry *registry, const UmiLanguageRenameSnapshot *item);
UmiStatus umi_language_rename_registry_remove(UmiLanguageRenameRegistry *registry, const char *id);
UmiStatus umi_language_rename_registry_find(const UmiLanguageRenameRegistry *registry, const char *id, UmiLanguageRenameSnapshot *out_item);
UmiStatus umi_language_rename_registry_at(const UmiLanguageRenameRegistry *registry, size_t index, UmiLanguageRenameSnapshot *out_item);
size_t umi_language_rename_registry_count(const UmiLanguageRenameRegistry *registry);
uint64_t umi_language_rename_registry_revision(const UmiLanguageRenameRegistry *registry);
void umi_language_rename_registry_clear(UmiLanguageRenameRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
