/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/definition.c
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

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#include "umicom/language/definition.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiLanguageDefinitionRegistry {
    UmiLanguageDefinitionSnapshot items[UMI_LANGUAGE_DEFINITION_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiLanguageDefinitionRegistry *registry, const char *id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

/*
 * Initialise language definition registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_definition_registry_create(UmiLanguageDefinitionRegistry **out_registry)
{
    UmiLanguageDefinitionRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiLanguageDefinitionRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by language definition registry so the same storage can be
 * reused safely.
 */
void umi_language_definition_registry_destroy(UmiLanguageDefinitionRegistry *registry) { free(registry); }

/*
 * Provide the language definition registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_definition_registry_upsert(UmiLanguageDefinitionRegistry *registry, const UmiLanguageDefinitionSnapshot *item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_LANGUAGE_DEFINITION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_LANGUAGE_DEFINITION_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].name[255U] = '\0';
    registry->items[index].file_extensions[255U] = '\0';
    registry->items[index].mime_types[255U] = '\0';
    registry->items[index].language_server[511U] = '\0';
    registry->items[index].formatter[511U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove language definition registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_definition_registry_remove(UmiLanguageDefinitionRegistry *registry, const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < registry->count) {
        memmove(&registry->items[index], &registry->items[index + 1U],
                (registry->count-index-1U)*sizeof(registry->items[0]));
    }
    registry->count -= 1U; registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find language definition registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_definition_registry_find(const UmiLanguageDefinitionRegistry *registry, const char *id, UmiLanguageDefinitionSnapshot *out_item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry,id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

/*
 * Find language definition registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_definition_registry_at(const UmiLanguageDefinitionRegistry *registry, size_t index, UmiLanguageDefinitionSnapshot *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by language definition registry without
 * changing their state.
 */
size_t umi_language_definition_registry_count(const UmiLanguageDefinitionRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the language definition registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_definition_registry_revision(const UmiLanguageDefinitionRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by language definition registry so the same storage can be
 * reused safely.
 */
void umi_language_definition_registry_clear(UmiLanguageDefinitionRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}
