/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/signature.c
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
#include "umicom/language/signature.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiLanguageSignatureRegistry {
    UmiLanguageSignatureSnapshot items[UMI_LANGUAGE_SIGNATURE_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiLanguageSignatureRegistry *registry, const char *id)
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
 * Initialise language signature registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_signature_registry_create(UmiLanguageSignatureRegistry **out_registry)
{
    UmiLanguageSignatureRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiLanguageSignatureRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by language signature registry so the same storage can be
 * reused safely.
 */
void umi_language_signature_registry_destroy(UmiLanguageSignatureRegistry *registry) { free(registry); }

/*
 * Provide the language signature registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_signature_registry_upsert(UmiLanguageSignatureRegistry *registry, const UmiLanguageSignatureSnapshot *item)
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
        if (registry->count >= UMI_LANGUAGE_SIGNATURE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_LANGUAGE_SIGNATURE_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].document_id[127U] = '\0';
    registry->items[index].label[511U] = '\0';
    registry->items[index].documentation[1023U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove language signature registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_signature_registry_remove(UmiLanguageSignatureRegistry *registry, const char *id)
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
 * Find language signature registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_signature_registry_find(const UmiLanguageSignatureRegistry *registry, const char *id, UmiLanguageSignatureSnapshot *out_item)
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
 * Find language signature registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_signature_registry_at(const UmiLanguageSignatureRegistry *registry, size_t index, UmiLanguageSignatureSnapshot *out_item)
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
 * Return the number of records represented by language signature registry without changing
 * their state.
 */
size_t umi_language_signature_registry_count(const UmiLanguageSignatureRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the language signature registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_signature_registry_revision(const UmiLanguageSignatureRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by language signature registry so the same storage can be
 * reused safely.
 */
void umi_language_signature_registry_clear(UmiLanguageSignatureRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}
