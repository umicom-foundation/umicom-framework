/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/completion.c
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
#include "umicom/language/completion.h"
#include "../base/snapshot_registry_internal.h"

/* Validate every bounded text member before lookup. Value-only snapshot
 * ownership stays with this existing Framework registry; domain semantics and
 * normalisation remain in its established implementation. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageCompletionSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageCompletionSnapshot, document_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageCompletionSnapshot, label, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageCompletionSnapshot, detail, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageCompletionSnapshot, insert_text, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageCompletionSnapshot, kind, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageCompletionSnapshot, sort_text, 0)
};
UmiStatus umi_language_completion_snapshot_validate(const UmiLanguageCompletionSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiLanguageCompletionRegistry {
    UmiLanguageCompletionSnapshot items[UMI_LANGUAGE_COMPLETION_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiLanguageCompletionRegistry *registry, const char *id)
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
 * Initialise language completion registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_completion_registry_create(UmiLanguageCompletionRegistry **out_registry)
{
    UmiLanguageCompletionRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiLanguageCompletionRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by language completion registry so the same storage can be
 * reused safely.
 */
void umi_language_completion_registry_destroy(UmiLanguageCompletionRegistry *registry) { free(registry); }

/*
 * Provide the language completion registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_completion_registry_upsert(UmiLanguageCompletionRegistry *registry, const UmiLanguageCompletionSnapshot *item)
{
    /* Central bounded validation rejects malformed snapshots before identity
     * comparison or mutation. The existing single-record logic is preserved;
     * valid records keep the same order, metadata and revision behavior. */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus validation = umi_language_completion_snapshot_validate(item, NULL);
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
        if (registry->count >= UMI_LANGUAGE_COMPLETION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_LANGUAGE_COMPLETION_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].document_id[127U] = '\0';
    registry->items[index].label[255U] = '\0';
    registry->items[index].detail[511U] = '\0';
    registry->items[index].insert_text[1023U] = '\0';
    registry->items[index].kind[63U] = '\0';
    registry->items[index].sort_text[255U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove language completion registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_completion_registry_remove(UmiLanguageCompletionRegistry *registry, const char *id)
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
 * Find language completion registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_completion_registry_find(const UmiLanguageCompletionRegistry *registry, const char *id, UmiLanguageCompletionSnapshot *out_item)
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
 * Find language completion registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_completion_registry_at(const UmiLanguageCompletionRegistry *registry, size_t index, UmiLanguageCompletionSnapshot *out_item)
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
 * Return the number of records represented by language completion registry without
 * changing their state.
 */
size_t umi_language_completion_registry_count(const UmiLanguageCompletionRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the language completion registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_completion_registry_revision(const UmiLanguageCompletionRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by language completion registry so the same storage can be
 * reused safely.
 */
void umi_language_completion_registry_clear(UmiLanguageCompletionRegistry *registry)
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
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_language_completion_registry_upsert_many,
    UmiLanguageCompletionRegistry, UmiLanguageCompletionSnapshot,
    umi_language_completion_snapshot_validate, umi_language_completion_registry_upsert, UMI_LANGUAGE_COMPLETION_CAPACITY)

/* Provider refreshes must replace this document as a unit, preserving other
 * documents and the last good list until all new records are accepted. */
UMI_DEFINE_SNAPSHOT_DOCUMENT_REPLACE(umi_language_completion_registry_replace_document,
    UmiLanguageCompletionRegistry, UmiLanguageCompletionSnapshot,
    umi_language_completion_snapshot_validate, umi_language_completion_registry_upsert, UMI_LANGUAGE_COMPLETION_CAPACITY)

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_language_completion_registry_capture,
    umi_language_completion_registry_replace_if_current, UmiLanguageCompletionRegistry, UmiLanguageCompletionSnapshot,
    umi_language_completion_snapshot_validate, umi_language_completion_registry_upsert, UMI_LANGUAGE_COMPLETION_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_language_completion_registry_edit_if_current,
    UmiLanguageCompletionRegistry, UmiLanguageCompletionSnapshot, UmiLanguageCompletionEdit,
    umi_language_completion_snapshot_validate, umi_language_completion_registry_upsert, umi_language_completion_registry_remove, UMI_LANGUAGE_COMPLETION_CAPACITY)
