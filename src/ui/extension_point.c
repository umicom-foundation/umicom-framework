/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/extension_point.c
 *
 * PURPOSE:
 *   Implement named UI extension points so products and plug-ins can contribute without private coupling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Each operation is deliberately small and deterministic. Snapshots are copied
 * into bounded storage, revisions advance on mutation, and callers retain
 * responsibility for higher-level threading and persistence policy.
 */
#include "umicom/ui/extension_point.h"
#include "../base/value_archive_internal.h"
#include "../base/registry_archive_internal.h"
#include "../base/snapshot_registry_internal.h"
/* Every field is described with its actual C member size; no string scan can
 * escape a supplied array. Domain values and legacy size/version normalisation
 * stay with the owning service. Diagnostics never copy a record's contents. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiUiExtensionPointSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiUiExtensionPointSnapshot, owner, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiUiExtensionPointSnapshot, kind, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiUiExtensionPointSnapshot, location, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiUiExtensionPointSnapshot, schema_id, 0)
};

UmiStatus umi_ui_extension_point_snapshot_validate(const UmiUiExtensionPointSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdlib.h>
#include <string.h>

struct UmiUiExtensionPointRegistry {
    UmiUiExtensionPointSnapshot items[UMI_UI_EXTENSION_POINT_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiUiExtensionPointRegistry *registry, const char *id)
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
 * Initialise ui extension point registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_extension_point_registry_create(UmiUiExtensionPointRegistry **out_registry)
{
    UmiUiExtensionPointRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiUiExtensionPointRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by ui extension point registry so the same storage can be
 * reused safely.
 */
void umi_ui_extension_point_registry_destroy(UmiUiExtensionPointRegistry *registry)
{
    free(registry);
}

/*
 * Provide the ui extension point registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_extension_point_registry_upsert(UmiUiExtensionPointRegistry *registry, const UmiUiExtensionPointSnapshot *item)
{
    /* Validate before ID lookup or mutation. The previous copy/terminator
     * implementation remains below for review and for normalising valid input.
     * Malformed arrays are now rejected instead of scanned beyond their bounds
     * or silently truncated. Reusable enforcement belongs in Framework. */
    UmiStatus validation = umi_ui_extension_point_snapshot_validate(item, NULL);
    if (validation != UMI_STATUS_OK) return validation;
    if (registry != NULL && registry->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
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
        if (registry->count >= UMI_UI_EXTENSION_POINT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiUiExtensionPointSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove ui extension point registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_extension_point_registry_remove(UmiUiExtensionPointRegistry *registry, const char *id)
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
                (registry->count - index - 1U) * sizeof(registry->items[0]));
    }
    registry->count -= 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find ui extension point registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_extension_point_registry_find(const UmiUiExtensionPointRegistry *registry, const char *id, UmiUiExtensionPointSnapshot *out_item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find ui extension point registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_extension_point_registry_at(const UmiUiExtensionPointRegistry *registry, size_t index, UmiUiExtensionPointSnapshot *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ui extension point registry without changing
 * their state.
 */
size_t umi_ui_extension_point_registry_count(const UmiUiExtensionPointRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the ui extension point registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_extension_point_registry_revision(const UmiUiExtensionPointRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

/* Stage a complete value-only registry before publishing a batch. Existing
 * upsert semantics run against the private copy, so any failed row leaves the
 * caller's count, records and revision unchanged. No application duplicate is
 * needed, and the original single-record implementation remains available. */
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_ui_extension_point_registry_upsert_many,
    UmiUiExtensionPointRegistry, UmiUiExtensionPointSnapshot,
    umi_ui_extension_point_snapshot_validate, umi_ui_extension_point_registry_upsert, UMI_UI_EXTENSION_POINT_CAPACITY)

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_ui_extension_point_registry_capture,
    umi_ui_extension_point_registry_replace_if_current, UmiUiExtensionPointRegistry, UmiUiExtensionPointSnapshot,
    umi_ui_extension_point_snapshot_validate, umi_ui_extension_point_registry_upsert, UMI_UI_EXTENSION_POINT_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_ui_extension_point_registry_edit_if_current,
    UmiUiExtensionPointRegistry, UmiUiExtensionPointSnapshot, UmiUiExtensionPointEdit,
    umi_ui_extension_point_snapshot_validate, umi_ui_extension_point_registry_upsert, umi_ui_extension_point_registry_remove, UMI_UI_EXTENSION_POINT_CAPACITY)

/* Read accepted extension point records in bounded pages. The shared
 * Framework rule refuses a changed observation before publishing output;
 * this owner's existing row order and normalized fields remain authoritative. */
UMI_DEFINE_SNAPSHOT_REGISTRY_PAGE(umi_ui_extension_point_registry_read_page,
    UmiUiExtensionPointRegistry, UmiUiExtensionPointSnapshot, UMI_UI_EXTENSION_POINT_CAPACITY)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x59e795982ce91136);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiExtensionPointSnapshot *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiExtensionPointSnapshot *)0)->owner)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiExtensionPointSnapshot *)0)->kind)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiExtensionPointSnapshot *)0)->location)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiExtensionPointSnapshot *)0)->schema_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiUiExtensionPointSnapshot *)0)->id) - 1U +
        8U + sizeof(((UmiUiExtensionPointSnapshot *)0)->owner) - 1U +
        8U + sizeof(((UmiUiExtensionPointSnapshot *)0)->kind) - 1U +
        8U + sizeof(((UmiUiExtensionPointSnapshot *)0)->location) - 1U +
        8U + sizeof(((UmiUiExtensionPointSnapshot *)0)->schema_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiUiExtensionPointSnapshot *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->owner, sizeof(value->owner));
    UmiArchiveWriteText(writer, value->kind, sizeof(value->kind));
    UmiArchiveWriteText(writer, value->location, sizeof(value->location));
    UmiArchiveWriteText(writer, value->schema_id, sizeof(value->schema_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->enabled);
    UmiArchiveWriteSigned(writer, (int64_t)value->multiple);
    UmiArchiveWriteSigned(writer, (int64_t)value->order);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiUiExtensionPointSnapshot *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->owner, sizeof(value->owner));
    UmiArchiveReadText(reader, value->kind, sizeof(value->kind));
    UmiArchiveReadText(reader, value->location, sizeof(value->location));
    UmiArchiveReadText(reader, value->schema_id, sizeof(value->schema_id));
    value->enabled = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->multiple = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->order = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus ArchiveValidate(const UmiUiExtensionPointSnapshot *value)
{
    return umi_ui_extension_point_snapshot_validate(value, NULL);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_extension_point_snapshot_archive_encode, umi_ui_extension_point_snapshot_archive_decode,
    UmiUiExtensionPointSnapshot, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)

/* Restoring this complete collection reuses its existing reviewed replacement
 * rules. Decoding a saved value alone never changes a live registry. */
UMI_DEFINE_REGISTRY_ARCHIVE(umi_ui_extension_point_registry_archive_encode, umi_ui_extension_point_registry_archive_restore,
    UmiUiExtensionPointRegistry, UmiUiExtensionPointSnapshot, UMI_UI_EXTENSION_POINT_CAPACITY, ArchiveSchema,
    umi_ui_extension_point_snapshot_archive_encode, umi_ui_extension_point_snapshot_archive_decode, umi_ui_extension_point_registry_replace_if_current)
