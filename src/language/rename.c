/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/rename.c
 *
 * PURPOSE:
 *   Implement a provider-neutral language-intelligence record that can be backed by LSP, native analysers or future Umicom language engines.
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
#include "umicom/language/rename.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiLanguageRenameRegistry {
    UmiLanguageRenameSnapshot items[UMI_LANGUAGE_RENAME_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiLanguageRenameRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_language_rename_registry_create(UmiLanguageRenameRegistry **out_registry)
{
    UmiLanguageRenameRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiLanguageRenameRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_language_rename_registry_destroy(UmiLanguageRenameRegistry *registry) { free(registry); }

UmiStatus umi_language_rename_registry_upsert(UmiLanguageRenameRegistry *registry, const UmiLanguageRenameSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_LANGUAGE_RENAME_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_LANGUAGE_RENAME_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].symbol_id[127U] = '\0';
    registry->items[index].old_name[255U] = '\0';
    registry->items[index].new_name[255U] = '\0';
    registry->items[index].document_id[127U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_language_rename_registry_remove(UmiLanguageRenameRegistry *registry, const char *id)
{
    size_t index;
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < registry->count) {
        memmove(&registry->items[index], &registry->items[index + 1U],
                (registry->count-index-1U)*sizeof(registry->items[0]));
    }
    registry->count -= 1U; registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_language_rename_registry_find(const UmiLanguageRenameRegistry *registry, const char *id, UmiLanguageRenameSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry,id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

UmiStatus umi_language_rename_registry_at(const UmiLanguageRenameRegistry *registry, size_t index, UmiLanguageRenameSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

size_t umi_language_rename_registry_count(const UmiLanguageRenameRegistry *registry) { return registry != NULL ? registry->count : 0U; }
uint64_t umi_language_rename_registry_revision(const UmiLanguageRenameRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
void umi_language_rename_registry_clear(UmiLanguageRenameRegistry *registry)
{
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}
