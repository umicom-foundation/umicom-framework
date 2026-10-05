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
#include "../base/value_archive_internal.h"
#include "../base/registry_archive_internal.h"
#include "../base/snapshot_registry_internal.h"

/* Validate every bounded text member before lookup. Value-only snapshot
 * ownership stays with this existing Framework registry; domain semantics and
 * normalisation remain in its established implementation. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageSignatureSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageSignatureSnapshot, document_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageSignatureSnapshot, label, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiLanguageSignatureSnapshot, documentation, 0)
};
UmiStatus umi_language_signature_snapshot_validate(const UmiLanguageSignatureSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


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
    /* Central bounded validation rejects malformed snapshots before identity
     * comparison or mutation. The existing single-record logic is preserved;
     * valid records keep the same order, metadata and revision behavior. */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus validation = umi_language_signature_snapshot_validate(item, NULL);
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

/* A complete private value registry makes a multi-record import atomic on
 * the owner's thread. The original single-record API remains the authority
 * for valid record normalisation; no application-side registry is introduced. */
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_language_signature_registry_upsert_many,
    UmiLanguageSignatureRegistry, UmiLanguageSignatureSnapshot,
    umi_language_signature_snapshot_validate, umi_language_signature_registry_upsert, UMI_LANGUAGE_SIGNATURE_CAPACITY)

/* Provider refreshes must replace this document as a unit, preserving other
 * documents and the last good list until all new records are accepted. */
UMI_DEFINE_SNAPSHOT_DOCUMENT_REPLACE(umi_language_signature_registry_replace_document,
    UmiLanguageSignatureRegistry, UmiLanguageSignatureSnapshot,
    umi_language_signature_snapshot_validate, umi_language_signature_registry_upsert, UMI_LANGUAGE_SIGNATURE_CAPACITY)

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_language_signature_registry_capture,
    umi_language_signature_registry_replace_if_current, UmiLanguageSignatureRegistry, UmiLanguageSignatureSnapshot,
    umi_language_signature_snapshot_validate, umi_language_signature_registry_upsert, UMI_LANGUAGE_SIGNATURE_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_language_signature_registry_edit_if_current,
    UmiLanguageSignatureRegistry, UmiLanguageSignatureSnapshot, UmiLanguageSignatureEdit,
    umi_language_signature_snapshot_validate, umi_language_signature_registry_upsert, umi_language_signature_registry_remove, UMI_LANGUAGE_SIGNATURE_CAPACITY)

/* Read accepted signature records in bounded pages. The shared
 * Framework rule refuses a changed observation before publishing output;
 * this owner's existing row order and normalized fields remain authoritative. */
UMI_DEFINE_SNAPSHOT_REGISTRY_PAGE(umi_language_signature_registry_read_page,
    UmiLanguageSignatureRegistry, UmiLanguageSignatureSnapshot, UMI_LANGUAGE_SIGNATURE_CAPACITY)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3da54815b0b4f947);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageSignatureSnapshot *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageSignatureSnapshot *)0)->document_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageSignatureSnapshot *)0)->label)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageSignatureSnapshot *)0)->documentation)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiLanguageSignatureSnapshot *)0)->id) - 1U +
        8U + sizeof(((UmiLanguageSignatureSnapshot *)0)->document_id) - 1U +
        8U + sizeof(((UmiLanguageSignatureSnapshot *)0)->label) - 1U +
        8U + sizeof(((UmiLanguageSignatureSnapshot *)0)->documentation) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiLanguageSignatureSnapshot *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->document_id, sizeof(value->document_id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteText(writer, value->documentation, sizeof(value->documentation));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active_parameter);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->column);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiLanguageSignatureSnapshot *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->document_id, sizeof(value->document_id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    UmiArchiveReadText(reader, value->documentation, sizeof(value->documentation));
    value->active_parameter = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->column = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus ArchiveValidate(const UmiLanguageSignatureSnapshot *value)
{
    return umi_language_signature_snapshot_validate(value, NULL);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_language_signature_snapshot_archive_encode, umi_language_signature_snapshot_archive_decode,
    UmiLanguageSignatureSnapshot, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)

/* Restoring this complete collection reuses its existing reviewed replacement
 * rules. Decoding a saved value alone never changes a live registry. */
UMI_DEFINE_REGISTRY_ARCHIVE(umi_language_signature_registry_archive_encode, umi_language_signature_registry_archive_restore,
    UmiLanguageSignatureRegistry, UmiLanguageSignatureSnapshot, UMI_LANGUAGE_SIGNATURE_CAPACITY, ArchiveSchema,
    umi_language_signature_snapshot_archive_encode, umi_language_signature_snapshot_archive_decode, umi_language_signature_registry_replace_if_current)
