/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/attachment.c
 *
 * PURPOSE:
 *   Implement a reusable test-explorer and test-run record independent of any single test framework.
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
#include "umicom/test_platform/attachment.h"
#include "../base/value_archive_internal.h"
#include "../base/registry_archive_internal.h"
#include "../base/snapshot_registry_internal.h"

/* Validate every bounded text member before lookup. Value-only snapshot
 * ownership stays with this existing Framework registry; domain semantics and
 * normalisation remain in its established implementation. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformAttachmentSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformAttachmentSnapshot, result_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformAttachmentSnapshot, name, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformAttachmentSnapshot, kind, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformAttachmentSnapshot, producer, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformAttachmentSnapshot, uri, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformAttachmentSnapshot, mime_type, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformAttachmentSnapshot, schema_uri, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformAttachmentSnapshot, checksum, 0)
};
UmiStatus umi_test_platform_attachment_snapshot_validate(const UmiTestPlatformAttachmentSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiTestPlatformAttachmentRegistry {
    UmiTestPlatformAttachmentSnapshot items[UMI_TEST_PLATFORM_ATTACHMENT_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiTestPlatformAttachmentRegistry *registry, const char *id)
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
 * Initialise test platform attachment registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_attachment_registry_create(UmiTestPlatformAttachmentRegistry **out_registry)
{
    UmiTestPlatformAttachmentRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiTestPlatformAttachmentRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by test platform attachment registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_attachment_registry_destroy(UmiTestPlatformAttachmentRegistry *registry) { free(registry); }

/*
 * Provide the test platform attachment registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_platform_attachment_registry_upsert(UmiTestPlatformAttachmentRegistry *registry, const UmiTestPlatformAttachmentSnapshot *item)
{
    /* Central bounded validation rejects malformed snapshots before identity
     * comparison or mutation. The existing single-record logic is preserved;
     * valid records keep the same order, metadata and revision behavior. */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus validation = umi_test_platform_attachment_snapshot_validate(item, NULL);
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
        if (registry->count >= UMI_TEST_PLATFORM_ATTACHMENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_TEST_PLATFORM_ATTACHMENT_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].result_id[127U] = '\0';
    registry->items[index].name[255U] = '\0';
    registry->items[index].kind[63U] = '\0';
    registry->items[index].producer[127U] = '\0';
    registry->items[index].uri[1023U] = '\0';
    registry->items[index].mime_type[127U] = '\0';
    registry->items[index].schema_uri[1023U] = '\0';
    registry->items[index].checksum[127U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove test platform attachment registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_test_platform_attachment_registry_remove(UmiTestPlatformAttachmentRegistry *registry, const char *id)
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
 * Find test platform attachment registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_attachment_registry_find(const UmiTestPlatformAttachmentRegistry *registry, const char *id, UmiTestPlatformAttachmentSnapshot *out_item)
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
 * Find test platform attachment registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_attachment_registry_at(const UmiTestPlatformAttachmentRegistry *registry, size_t index, UmiTestPlatformAttachmentSnapshot *out_item)
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
 * Return the number of records represented by test platform attachment registry without
 * changing their state.
 */
size_t umi_test_platform_attachment_registry_count(const UmiTestPlatformAttachmentRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the test platform attachment registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_test_platform_attachment_registry_revision(const UmiTestPlatformAttachmentRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by test platform attachment registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_attachment_registry_clear(UmiTestPlatformAttachmentRegistry *registry)
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
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_test_platform_attachment_registry_upsert_many,
    UmiTestPlatformAttachmentRegistry, UmiTestPlatformAttachmentSnapshot,
    umi_test_platform_attachment_snapshot_validate, umi_test_platform_attachment_registry_upsert, UMI_TEST_PLATFORM_ATTACHMENT_CAPACITY)

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_test_platform_attachment_registry_capture,
    umi_test_platform_attachment_registry_replace_if_current, UmiTestPlatformAttachmentRegistry, UmiTestPlatformAttachmentSnapshot,
    umi_test_platform_attachment_snapshot_validate, umi_test_platform_attachment_registry_upsert, UMI_TEST_PLATFORM_ATTACHMENT_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_test_platform_attachment_registry_edit_if_current,
    UmiTestPlatformAttachmentRegistry, UmiTestPlatformAttachmentSnapshot, UmiTestPlatformAttachmentEdit,
    umi_test_platform_attachment_snapshot_validate, umi_test_platform_attachment_registry_upsert, umi_test_platform_attachment_registry_remove, UMI_TEST_PLATFORM_ATTACHMENT_CAPACITY)

/* Read accepted attachment records in bounded pages. The shared
 * Framework rule refuses a changed observation before publishing output;
 * this owner's existing row order and normalized fields remain authoritative. */
UMI_DEFINE_SNAPSHOT_REGISTRY_PAGE(umi_test_platform_attachment_registry_read_page,
    UmiTestPlatformAttachmentRegistry, UmiTestPlatformAttachmentSnapshot, UMI_TEST_PLATFORM_ATTACHMENT_CAPACITY)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5226e229e08763db);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->result_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->kind)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->producer)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->uri)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->mime_type)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->schema_uri)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->checksum)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->id) - 1U +
        8U + sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->result_id) - 1U +
        8U + sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->name) - 1U +
        8U + sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->kind) - 1U +
        8U + sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->producer) - 1U +
        8U + sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->uri) - 1U +
        8U + sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->mime_type) - 1U +
        8U + sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->schema_uri) - 1U +
        8U + sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->checksum) - 1U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiTestPlatformAttachmentSnapshot *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->result_id, sizeof(value->result_id));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->kind, sizeof(value->kind));
    UmiArchiveWriteText(writer, value->producer, sizeof(value->producer));
    UmiArchiveWriteText(writer, value->uri, sizeof(value->uri));
    UmiArchiveWriteText(writer, value->mime_type, sizeof(value->mime_type));
    UmiArchiveWriteText(writer, value->schema_uri, sizeof(value->schema_uri));
    UmiArchiveWriteText(writer, value->checksum, sizeof(value->checksum));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->size_bytes);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiTestPlatformAttachmentSnapshot *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->result_id, sizeof(value->result_id));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->kind, sizeof(value->kind));
    UmiArchiveReadText(reader, value->producer, sizeof(value->producer));
    UmiArchiveReadText(reader, value->uri, sizeof(value->uri));
    UmiArchiveReadText(reader, value->mime_type, sizeof(value->mime_type));
    UmiArchiveReadText(reader, value->schema_uri, sizeof(value->schema_uri));
    UmiArchiveReadText(reader, value->checksum, sizeof(value->checksum));
    value->size_bytes = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus ArchiveValidate(const UmiTestPlatformAttachmentSnapshot *value)
{
    return umi_test_platform_attachment_snapshot_validate(value, NULL);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_test_platform_attachment_snapshot_archive_encode, umi_test_platform_attachment_snapshot_archive_decode,
    UmiTestPlatformAttachmentSnapshot, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)

/* Restoring this complete collection reuses its existing reviewed replacement
 * rules. Decoding a saved value alone never changes a live registry. */
UMI_DEFINE_REGISTRY_ARCHIVE(umi_test_platform_attachment_registry_archive_encode, umi_test_platform_attachment_registry_archive_restore,
    UmiTestPlatformAttachmentRegistry, UmiTestPlatformAttachmentSnapshot, UMI_TEST_PLATFORM_ATTACHMENT_CAPACITY, ArchiveSchema,
    umi_test_platform_attachment_snapshot_archive_encode, umi_test_platform_attachment_snapshot_archive_decode, umi_test_platform_attachment_registry_replace_if_current)
