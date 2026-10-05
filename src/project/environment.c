/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/environment.c
 *
 * PURPOSE:
 *   Implement a reusable project-system record used by Studio and future Umicom development products.
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
#include "umicom/project/environment.h"
#include "../base/value_archive_internal.h"
#include "../base/registry_archive_internal.h"
#include "../base/snapshot_registry_internal.h"
/* Every field is described with its actual C member size; no string scan can
 * escape a supplied array. Domain values and legacy size/version normalisation
 * stay with the owning service. Diagnostics never copy a record's contents. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiProjectEnvironmentSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiProjectEnvironmentSnapshot, project_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiProjectEnvironmentSnapshot, name, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiProjectEnvironmentSnapshot, toolchain_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiProjectEnvironmentSnapshot, path_prefix, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiProjectEnvironmentSnapshot, variables, 0)
};

UmiStatus umi_project_environment_snapshot_validate(const UmiProjectEnvironmentSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiProjectEnvironmentRegistry {
    UmiProjectEnvironmentSnapshot items[UMI_PROJECT_ENVIRONMENT_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiProjectEnvironmentRegistry *registry, const char *id)
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
 * Initialise project environment registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_environment_registry_create(UmiProjectEnvironmentRegistry **out_registry)
{
    UmiProjectEnvironmentRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiProjectEnvironmentRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by project environment registry so the same storage can be
 * reused safely.
 */
void umi_project_environment_registry_destroy(UmiProjectEnvironmentRegistry *registry) { free(registry); }

/*
 * Provide the project environment registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_environment_registry_upsert(UmiProjectEnvironmentRegistry *registry, const UmiProjectEnvironmentSnapshot *item)
{
    /* Validate before ID lookup or mutation. The previous copy/terminator
     * implementation remains below for review and for normalising valid input.
     * Malformed arrays are now rejected instead of scanned beyond their bounds
     * or silently truncated. Reusable enforcement belongs in Framework. */
    UmiStatus validation = umi_project_environment_snapshot_validate(item, NULL);
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
        if (registry->count >= UMI_PROJECT_ENVIRONMENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_PROJECT_ENVIRONMENT_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].project_id[127U] = '\0';
    registry->items[index].name[255U] = '\0';
    registry->items[index].toolchain_id[127U] = '\0';
    registry->items[index].path_prefix[1023U] = '\0';
    registry->items[index].variables[2047U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove project environment registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_environment_registry_remove(UmiProjectEnvironmentRegistry *registry, const char *id)
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
 * Find project environment registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_environment_registry_find(const UmiProjectEnvironmentRegistry *registry, const char *id, UmiProjectEnvironmentSnapshot *out_item)
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
 * Find project environment registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_environment_registry_at(const UmiProjectEnvironmentRegistry *registry, size_t index, UmiProjectEnvironmentSnapshot *out_item)
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
 * Return the number of records represented by project environment registry without
 * changing their state.
 */
size_t umi_project_environment_registry_count(const UmiProjectEnvironmentRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the project environment registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_project_environment_registry_revision(const UmiProjectEnvironmentRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by project environment registry so the same storage can be
 * reused safely.
 */
void umi_project_environment_registry_clear(UmiProjectEnvironmentRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}

/* Stage a complete value-only registry before publishing a batch. Existing
 * upsert semantics run against the private copy, so any failed row leaves the
 * caller's count, records and revision unchanged. No application duplicate is
 * needed, and the original single-record implementation remains available. */
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_project_environment_registry_upsert_many,
    UmiProjectEnvironmentRegistry, UmiProjectEnvironmentSnapshot,
    umi_project_environment_snapshot_validate, umi_project_environment_registry_upsert, UMI_PROJECT_ENVIRONMENT_CAPACITY)

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_project_environment_registry_capture,
    umi_project_environment_registry_replace_if_current, UmiProjectEnvironmentRegistry, UmiProjectEnvironmentSnapshot,
    umi_project_environment_snapshot_validate, umi_project_environment_registry_upsert, UMI_PROJECT_ENVIRONMENT_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_project_environment_registry_edit_if_current,
    UmiProjectEnvironmentRegistry, UmiProjectEnvironmentSnapshot, UmiProjectEnvironmentEdit,
    umi_project_environment_snapshot_validate, umi_project_environment_registry_upsert, umi_project_environment_registry_remove, UMI_PROJECT_ENVIRONMENT_CAPACITY)

/* Read accepted environment records in bounded pages. The shared
 * Framework rule refuses a changed observation before publishing output;
 * this owner's existing row order and normalized fields remain authoritative. */
UMI_DEFINE_SNAPSHOT_REGISTRY_PAGE(umi_project_environment_registry_read_page,
    UmiProjectEnvironmentRegistry, UmiProjectEnvironmentSnapshot, UMI_PROJECT_ENVIRONMENT_CAPACITY)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x99d16a043e924dce);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectEnvironmentSnapshot *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectEnvironmentSnapshot *)0)->project_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectEnvironmentSnapshot *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectEnvironmentSnapshot *)0)->toolchain_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectEnvironmentSnapshot *)0)->path_prefix)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectEnvironmentSnapshot *)0)->variables)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiProjectEnvironmentSnapshot *)0)->id) - 1U +
        8U + sizeof(((UmiProjectEnvironmentSnapshot *)0)->project_id) - 1U +
        8U + sizeof(((UmiProjectEnvironmentSnapshot *)0)->name) - 1U +
        8U + sizeof(((UmiProjectEnvironmentSnapshot *)0)->toolchain_id) - 1U +
        8U + sizeof(((UmiProjectEnvironmentSnapshot *)0)->path_prefix) - 1U +
        8U + sizeof(((UmiProjectEnvironmentSnapshot *)0)->variables) - 1U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiProjectEnvironmentSnapshot *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->project_id, sizeof(value->project_id));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->toolchain_id, sizeof(value->toolchain_id));
    UmiArchiveWriteText(writer, value->path_prefix, sizeof(value->path_prefix));
    UmiArchiveWriteText(writer, value->variables, sizeof(value->variables));
    UmiArchiveWriteSigned(writer, (int64_t)value->inherit_parent);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiProjectEnvironmentSnapshot *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->project_id, sizeof(value->project_id));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->toolchain_id, sizeof(value->toolchain_id));
    UmiArchiveReadText(reader, value->path_prefix, sizeof(value->path_prefix));
    UmiArchiveReadText(reader, value->variables, sizeof(value->variables));
    value->inherit_parent = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus ArchiveValidate(const UmiProjectEnvironmentSnapshot *value)
{
    return umi_project_environment_snapshot_validate(value, NULL);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_project_environment_snapshot_archive_encode, umi_project_environment_snapshot_archive_decode,
    UmiProjectEnvironmentSnapshot, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)

/* Restoring this complete collection reuses its existing reviewed replacement
 * rules. Decoding a saved value alone never changes a live registry. */
UMI_DEFINE_REGISTRY_ARCHIVE(umi_project_environment_registry_archive_encode, umi_project_environment_registry_archive_restore,
    UmiProjectEnvironmentRegistry, UmiProjectEnvironmentSnapshot, UMI_PROJECT_ENVIRONMENT_CAPACITY, ArchiveSchema,
    umi_project_environment_snapshot_archive_encode, umi_project_environment_snapshot_archive_decode, umi_project_environment_registry_replace_if_current)
