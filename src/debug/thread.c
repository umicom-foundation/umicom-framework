/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/thread.c
 *
 * PURPOSE:
 *   Implement a DAP-friendly but adapter-neutral debugger record for native and future Umicom runtimes.
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
#include "umicom/debug/thread.h"
#include "../base/value_archive_internal.h"
#include "../base/registry_archive_internal.h"
#include "../base/snapshot_registry_internal.h"
/* Every field is described with its actual C member size; no string scan can
 * escape a supplied array. Domain values and legacy size/version normalisation
 * stay with the owning service. Diagnostics never copy a record's contents. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiDebugThreadSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiDebugThreadSnapshot, session_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiDebugThreadSnapshot, name, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiDebugThreadSnapshot, detail, 0)
};

UmiStatus umi_debug_thread_snapshot_validate(const UmiDebugThreadSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiDebugThreadRegistry {
    UmiDebugThreadSnapshot items[UMI_DEBUG_THREAD_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiDebugThreadRegistry *registry, const char *id)
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
 * Initialise debug thread registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_debug_thread_registry_create(UmiDebugThreadRegistry **out_registry)
{
    UmiDebugThreadRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiDebugThreadRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by debug thread registry so the same storage can be reused
 * safely.
 */
void umi_debug_thread_registry_destroy(UmiDebugThreadRegistry *registry) { free(registry); }

/*
 * Provide the debug thread registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_thread_registry_upsert(UmiDebugThreadRegistry *registry, const UmiDebugThreadSnapshot *item)
{
    /* Validate before ID lookup or mutation. The previous copy/terminator
     * implementation remains below for review and for normalising valid input.
     * Malformed arrays are now rejected instead of scanned beyond their bounds
     * or silently truncated. Reusable enforcement belongs in Framework. */
    UmiStatus validation = umi_debug_thread_snapshot_validate(item, NULL);
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
        if (registry->count >= UMI_DEBUG_THREAD_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_DEBUG_THREAD_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].session_id[127U] = '\0';
    registry->items[index].name[255U] = '\0';
    registry->items[index].detail[511U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove debug thread registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_thread_registry_remove(UmiDebugThreadRegistry *registry, const char *id)
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
 * Find debug thread registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_thread_registry_find(const UmiDebugThreadRegistry *registry, const char *id, UmiDebugThreadSnapshot *out_item)
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
 * Find debug thread registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_thread_registry_at(const UmiDebugThreadRegistry *registry, size_t index, UmiDebugThreadSnapshot *out_item)
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
 * Return the number of records represented by debug thread registry without changing their
 * state.
 */
size_t umi_debug_thread_registry_count(const UmiDebugThreadRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the debug thread registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_debug_thread_registry_revision(const UmiDebugThreadRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by debug thread registry so the same storage can be reused
 * safely.
 */
void umi_debug_thread_registry_clear(UmiDebugThreadRegistry *registry)
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
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_debug_thread_registry_upsert_many,
    UmiDebugThreadRegistry, UmiDebugThreadSnapshot,
    umi_debug_thread_snapshot_validate, umi_debug_thread_registry_upsert, UMI_DEBUG_THREAD_CAPACITY)

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_debug_thread_registry_capture,
    umi_debug_thread_registry_replace_if_current, UmiDebugThreadRegistry, UmiDebugThreadSnapshot,
    umi_debug_thread_snapshot_validate, umi_debug_thread_registry_upsert, UMI_DEBUG_THREAD_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_debug_thread_registry_edit_if_current,
    UmiDebugThreadRegistry, UmiDebugThreadSnapshot, UmiDebugThreadEdit,
    umi_debug_thread_snapshot_validate, umi_debug_thread_registry_upsert, umi_debug_thread_registry_remove, UMI_DEBUG_THREAD_CAPACITY)

/* Read accepted thread records in bounded pages. The shared
 * Framework rule refuses a changed observation before publishing output;
 * this owner's existing row order and normalized fields remain authoritative. */
UMI_DEFINE_SNAPSHOT_REGISTRY_PAGE(umi_debug_thread_registry_read_page,
    UmiDebugThreadRegistry, UmiDebugThreadSnapshot, UMI_DEBUG_THREAD_CAPACITY)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xca49802ce93cf0fb);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugThreadSnapshot *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugThreadSnapshot *)0)->session_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugThreadSnapshot *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugThreadSnapshot *)0)->detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiDebugThreadSnapshot *)0)->id) - 1U +
        8U + sizeof(((UmiDebugThreadSnapshot *)0)->session_id) - 1U +
        8U + sizeof(((UmiDebugThreadSnapshot *)0)->name) - 1U +
        8U + sizeof(((UmiDebugThreadSnapshot *)0)->detail) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiDebugThreadSnapshot *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->session_id, sizeof(value->session_id));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->detail, sizeof(value->detail));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->native_id);
    UmiArchiveWriteSigned(writer, (int64_t)value->stopped);
    UmiArchiveWriteSigned(writer, (int64_t)value->current);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiDebugThreadSnapshot *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->session_id, sizeof(value->session_id));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->detail, sizeof(value->detail));
    value->native_id = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->stopped = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->current = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus ArchiveValidate(const UmiDebugThreadSnapshot *value)
{
    return umi_debug_thread_snapshot_validate(value, NULL);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_debug_thread_snapshot_archive_encode, umi_debug_thread_snapshot_archive_decode,
    UmiDebugThreadSnapshot, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)

/* Restoring this complete collection reuses its existing reviewed replacement
 * rules. Decoding a saved value alone never changes a live registry. */
UMI_DEFINE_REGISTRY_ARCHIVE(umi_debug_thread_registry_archive_encode, umi_debug_thread_registry_archive_restore,
    UmiDebugThreadRegistry, UmiDebugThreadSnapshot, UMI_DEBUG_THREAD_CAPACITY, ArchiveSchema,
    umi_debug_thread_snapshot_archive_encode, umi_debug_thread_snapshot_archive_decode, umi_debug_thread_registry_replace_if_current)
