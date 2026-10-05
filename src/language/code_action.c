/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/code_action.c
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
#include "umicom/language/code_action.h"
#include "../base/value_archive_internal.h"
#include "../base/registry_archive_internal.h"
#include "../base/snapshot_registry_internal.h"

/* Validate every bounded text member before lookup. Value-only snapshot
 * ownership stays with this existing Framework registry; domain semantics and
 * normalisation remain in its established implementation. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageCodeActionSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageCodeActionSnapshot, document_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageCodeActionSnapshot, title, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageCodeActionSnapshot, kind, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageCodeActionSnapshot, command_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageCodeActionSnapshot, argument, 0)
};
UmiStatus umi_language_code_action_snapshot_validate(const UmiLanguageCodeActionSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiLanguageCodeActionRegistry {
    UmiLanguageCodeActionSnapshot items[UMI_LANGUAGE_CODE_ACTION_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiLanguageCodeActionRegistry *registry, const char *id)
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
 * Initialise language code action registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_code_action_registry_create(UmiLanguageCodeActionRegistry **out_registry)
{
    UmiLanguageCodeActionRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiLanguageCodeActionRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by language code action registry so the same storage can be
 * reused safely.
 */
void umi_language_code_action_registry_destroy(UmiLanguageCodeActionRegistry *registry) { free(registry); }

/*
 * Provide the language code action registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_code_action_registry_upsert(UmiLanguageCodeActionRegistry *registry, const UmiLanguageCodeActionSnapshot *item)
{
    /* Central bounded validation rejects malformed snapshots before identity
     * comparison or mutation. The existing single-record logic is preserved;
     * valid records keep the same order, metadata and revision behavior. */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus validation = umi_language_code_action_snapshot_validate(item, NULL);
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
        if (registry->count >= UMI_LANGUAGE_CODE_ACTION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_LANGUAGE_CODE_ACTION_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].document_id[127U] = '\0';
    registry->items[index].title[511U] = '\0';
    registry->items[index].kind[127U] = '\0';
    registry->items[index].command_id[127U] = '\0';
    registry->items[index].argument[1023U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove language code action registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_language_code_action_registry_remove(UmiLanguageCodeActionRegistry *registry, const char *id)
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
 * Find language code action registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_code_action_registry_find(const UmiLanguageCodeActionRegistry *registry, const char *id, UmiLanguageCodeActionSnapshot *out_item)
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
 * Find language code action registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_language_code_action_registry_at(const UmiLanguageCodeActionRegistry *registry, size_t index, UmiLanguageCodeActionSnapshot *out_item)
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
 * Return the number of records represented by language code action registry without
 * changing their state.
 */
size_t umi_language_code_action_registry_count(const UmiLanguageCodeActionRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the language code action registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_language_code_action_registry_revision(const UmiLanguageCodeActionRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by language code action registry so the same storage can be
 * reused safely.
 */
void umi_language_code_action_registry_clear(UmiLanguageCodeActionRegistry *registry)
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
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_language_code_action_registry_upsert_many,
    UmiLanguageCodeActionRegistry, UmiLanguageCodeActionSnapshot,
    umi_language_code_action_snapshot_validate, umi_language_code_action_registry_upsert, UMI_LANGUAGE_CODE_ACTION_CAPACITY)

/* Provider refreshes must replace this document as a unit, preserving other
 * documents and the last good list until all new records are accepted. */
UMI_DEFINE_SNAPSHOT_DOCUMENT_REPLACE(umi_language_code_action_registry_replace_document,
    UmiLanguageCodeActionRegistry, UmiLanguageCodeActionSnapshot,
    umi_language_code_action_snapshot_validate, umi_language_code_action_registry_upsert, UMI_LANGUAGE_CODE_ACTION_CAPACITY)

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_language_code_action_registry_capture,
    umi_language_code_action_registry_replace_if_current, UmiLanguageCodeActionRegistry, UmiLanguageCodeActionSnapshot,
    umi_language_code_action_snapshot_validate, umi_language_code_action_registry_upsert, UMI_LANGUAGE_CODE_ACTION_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_language_code_action_registry_edit_if_current,
    UmiLanguageCodeActionRegistry, UmiLanguageCodeActionSnapshot, UmiLanguageCodeActionEdit,
    umi_language_code_action_snapshot_validate, umi_language_code_action_registry_upsert, umi_language_code_action_registry_remove, UMI_LANGUAGE_CODE_ACTION_CAPACITY)

/* Read accepted code action records in bounded pages. The shared
 * Framework rule refuses a changed observation before publishing output;
 * this owner's existing row order and normalized fields remain authoritative. */
UMI_DEFINE_SNAPSHOT_REGISTRY_PAGE(umi_language_code_action_registry_read_page,
    UmiLanguageCodeActionRegistry, UmiLanguageCodeActionSnapshot, UMI_LANGUAGE_CODE_ACTION_CAPACITY)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb9c8bef656b03ab0);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageCodeActionSnapshot *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageCodeActionSnapshot *)0)->document_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageCodeActionSnapshot *)0)->title)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageCodeActionSnapshot *)0)->kind)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageCodeActionSnapshot *)0)->command_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageCodeActionSnapshot *)0)->argument)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiLanguageCodeActionSnapshot *)0)->id) - 1U +
        8U + sizeof(((UmiLanguageCodeActionSnapshot *)0)->document_id) - 1U +
        8U + sizeof(((UmiLanguageCodeActionSnapshot *)0)->title) - 1U +
        8U + sizeof(((UmiLanguageCodeActionSnapshot *)0)->kind) - 1U +
        8U + sizeof(((UmiLanguageCodeActionSnapshot *)0)->command_id) - 1U +
        8U + sizeof(((UmiLanguageCodeActionSnapshot *)0)->argument) - 1U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiLanguageCodeActionSnapshot *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->document_id, sizeof(value->document_id));
    UmiArchiveWriteText(writer, value->title, sizeof(value->title));
    UmiArchiveWriteText(writer, value->kind, sizeof(value->kind));
    UmiArchiveWriteText(writer, value->command_id, sizeof(value->command_id));
    UmiArchiveWriteText(writer, value->argument, sizeof(value->argument));
    UmiArchiveWriteSigned(writer, (int64_t)value->preferred);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiLanguageCodeActionSnapshot *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->document_id, sizeof(value->document_id));
    UmiArchiveReadText(reader, value->title, sizeof(value->title));
    UmiArchiveReadText(reader, value->kind, sizeof(value->kind));
    UmiArchiveReadText(reader, value->command_id, sizeof(value->command_id));
    UmiArchiveReadText(reader, value->argument, sizeof(value->argument));
    value->preferred = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus ArchiveValidate(const UmiLanguageCodeActionSnapshot *value)
{
    return umi_language_code_action_snapshot_validate(value, NULL);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_language_code_action_snapshot_archive_encode, umi_language_code_action_snapshot_archive_decode,
    UmiLanguageCodeActionSnapshot, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)

/* Restoring this complete collection reuses its existing reviewed replacement
 * rules. Decoding a saved value alone never changes a live registry. */
UMI_DEFINE_REGISTRY_ARCHIVE(umi_language_code_action_registry_archive_encode, umi_language_code_action_registry_archive_restore,
    UmiLanguageCodeActionRegistry, UmiLanguageCodeActionSnapshot, UMI_LANGUAGE_CODE_ACTION_CAPACITY, ArchiveSchema,
    umi_language_code_action_snapshot_archive_encode, umi_language_code_action_snapshot_archive_decode, umi_language_code_action_registry_replace_if_current)
