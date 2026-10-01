/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/provider.c
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
#include "umicom/language/provider.h"
#include "../base/snapshot_registry_internal.h"

/* Validate every bounded text member before lookup. Value-only snapshot
 * ownership stays with this existing Framework registry; domain semantics and
 * normalisation remain in its established implementation. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageProviderSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageProviderSnapshot, language_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageProviderSnapshot, kind, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageProviderSnapshot, name, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageProviderSnapshot, command, 0)
};
UmiStatus umi_language_provider_snapshot_validate(const UmiLanguageProviderSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiLanguageProviderRegistry {
    UmiLanguageProviderSnapshot items[UMI_LANGUAGE_PROVIDER_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiLanguageProviderRegistry *registry, const char *id)
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
 * Initialise language provider registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_provider_registry_create(UmiLanguageProviderRegistry **out_registry)
{
    UmiLanguageProviderRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiLanguageProviderRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by language provider registry so the same storage can be
 * reused safely.
 */
void umi_language_provider_registry_destroy(UmiLanguageProviderRegistry *registry) { free(registry); }

/*
 * Provide the language provider registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_provider_registry_upsert(UmiLanguageProviderRegistry *registry, const UmiLanguageProviderSnapshot *item)
{
    /* Central bounded validation rejects malformed snapshots before identity
     * comparison or mutation. The existing single-record logic is preserved;
     * valid records keep the same order, metadata and revision behavior. */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus validation = umi_language_provider_snapshot_validate(item, NULL);
    if (validation != UMI_STATUS_OK) return validation;
    if (registry->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;

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
        if (registry->count >= UMI_LANGUAGE_PROVIDER_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_LANGUAGE_PROVIDER_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].language_id[127U] = '\0';
    registry->items[index].kind[63U] = '\0';
    registry->items[index].name[255U] = '\0';
    registry->items[index].command[1023U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove language provider registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_provider_registry_remove(UmiLanguageProviderRegistry *registry, const char *id)
{
    /* A wrapped revision could make an old edit appear current again. Refuse
     * mutation before touching records when no fresh revision is available. */
    if (registry != NULL && registry->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

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
 * Find language provider registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_provider_registry_find(const UmiLanguageProviderRegistry *registry, const char *id, UmiLanguageProviderSnapshot *out_item)
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
 * Find language provider registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_provider_registry_at(const UmiLanguageProviderRegistry *registry, size_t index, UmiLanguageProviderSnapshot *out_item)
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
 * Return the number of records represented by language provider registry without changing
 * their state.
 */
size_t umi_language_provider_registry_count(const UmiLanguageProviderRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the language provider registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_provider_registry_revision(const UmiLanguageProviderRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by language provider registry so the same storage can be
 * reused safely.
 */
void umi_language_provider_registry_clear(UmiLanguageProviderRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}

/* A complete private value registry makes a multi-record import atomic on
 * the owner's thread. The original single-record API remains the authority
 * for valid record normalisation; no application-side registry is introduced. */
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_language_provider_registry_upsert_many,
    UmiLanguageProviderRegistry, UmiLanguageProviderSnapshot,
    umi_language_provider_snapshot_validate, umi_language_provider_registry_upsert, UMI_LANGUAGE_PROVIDER_CAPACITY)

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_language_provider_registry_capture,
    umi_language_provider_registry_replace_if_current, UmiLanguageProviderRegistry, UmiLanguageProviderSnapshot,
    umi_language_provider_snapshot_validate, umi_language_provider_registry_upsert, UMI_LANGUAGE_PROVIDER_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_language_provider_registry_edit_if_current,
    UmiLanguageProviderRegistry, UmiLanguageProviderSnapshot, UmiLanguageProviderEdit,
    umi_language_provider_snapshot_validate, umi_language_provider_registry_upsert, umi_language_provider_registry_remove, UMI_LANGUAGE_PROVIDER_CAPACITY)

/* Read accepted provider records in bounded pages. The shared
 * Framework rule refuses a changed observation before publishing output;
 * this owner's existing row order and normalized fields remain authoritative. */
UMI_DEFINE_SNAPSHOT_REGISTRY_PAGE(umi_language_provider_registry_read_page,
    UmiLanguageProviderRegistry, UmiLanguageProviderSnapshot, UMI_LANGUAGE_PROVIDER_CAPACITY)
