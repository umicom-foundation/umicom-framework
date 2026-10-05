/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/result.c
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
#include "umicom/test_platform/result.h"
#include "../base/value_archive_internal.h"
#include "../base/registry_archive_internal.h"
#include "../base/snapshot_registry_internal.h"

/* Validate every bounded text member before lookup. Value-only snapshot
 * ownership stays with this existing Framework registry; domain semantics and
 * normalisation remain in its established implementation. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformResultSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformResultSnapshot, session_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformResultSnapshot, item_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformResultSnapshot, message, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformResultSnapshot, failure_details, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiTestPlatformResultSnapshot, attachment_id, 0)
};
UmiStatus umi_test_platform_result_snapshot_validate(const UmiTestPlatformResultSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiTestPlatformResultRegistry {
    UmiTestPlatformResultSnapshot items[UMI_TEST_PLATFORM_RESULT_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiTestPlatformResultRegistry *registry, const char *id)
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
 * Initialise test platform result registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_platform_result_registry_create(UmiTestPlatformResultRegistry **out_registry)
{
    UmiTestPlatformResultRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiTestPlatformResultRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by test platform result registry so the same storage can be
 * reused safely.
 */
void umi_test_platform_result_registry_destroy(UmiTestPlatformResultRegistry *registry) { free(registry); }

/*
 * Provide the test platform result registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_result_registry_upsert(UmiTestPlatformResultRegistry *registry, const UmiTestPlatformResultSnapshot *item)
{
    /* Central bounded validation rejects malformed snapshots before identity
     * comparison or mutation. The existing single-record logic is preserved;
     * valid records keep the same order, metadata and revision behavior. */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus validation = umi_test_platform_result_snapshot_validate(item, NULL);
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
        if (registry->count >= UMI_TEST_PLATFORM_RESULT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_TEST_PLATFORM_RESULT_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].session_id[127U] = '\0';
    registry->items[index].item_id[127U] = '\0';
    registry->items[index].message[1023U] = '\0';
    registry->items[index].failure_details[2047U] = '\0';
    registry->items[index].attachment_id[127U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the test platform outcome text operation used by this module and its client
 * applications.
 */
const char *umi_test_platform_outcome_text(UmiTestPlatformOutcome outcome)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (outcome) {
        case UMI_TEST_PLATFORM_OUTCOME_NOT_RUN: return "not-run";
        case UMI_TEST_PLATFORM_OUTCOME_PASSED: return "passed";
        case UMI_TEST_PLATFORM_OUTCOME_FAILED: return "failed";
        case UMI_TEST_PLATFORM_OUTCOME_SKIPPED: return "skipped";
        case UMI_TEST_PLATFORM_OUTCOME_CANCELLED: return "cancelled";
        case UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT: return "timed-out";
        default: return "unknown";
    }
}

/*
 * Remove test platform result registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_test_platform_result_registry_remove(UmiTestPlatformResultRegistry *registry, const char *id)
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
 * Find test platform result registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_test_platform_result_registry_find(const UmiTestPlatformResultRegistry *registry, const char *id, UmiTestPlatformResultSnapshot *out_item)
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
 * Find test platform result registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_test_platform_result_registry_at(const UmiTestPlatformResultRegistry *registry, size_t index, UmiTestPlatformResultSnapshot *out_item)
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
 * Return the number of records represented by test platform result registry without
 * changing their state.
 */
size_t umi_test_platform_result_registry_count(const UmiTestPlatformResultRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the test platform result registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_test_platform_result_registry_revision(const UmiTestPlatformResultRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by test platform result registry so the same storage can be
 * reused safely.
 */
void umi_test_platform_result_registry_clear(UmiTestPlatformResultRegistry *registry)
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
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_test_platform_result_registry_upsert_many,
    UmiTestPlatformResultRegistry, UmiTestPlatformResultSnapshot,
    umi_test_platform_result_snapshot_validate, umi_test_platform_result_registry_upsert, UMI_TEST_PLATFORM_RESULT_CAPACITY)

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_test_platform_result_registry_capture,
    umi_test_platform_result_registry_replace_if_current, UmiTestPlatformResultRegistry, UmiTestPlatformResultSnapshot,
    umi_test_platform_result_snapshot_validate, umi_test_platform_result_registry_upsert, UMI_TEST_PLATFORM_RESULT_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_test_platform_result_registry_edit_if_current,
    UmiTestPlatformResultRegistry, UmiTestPlatformResultSnapshot, UmiTestPlatformResultEdit,
    umi_test_platform_result_snapshot_validate, umi_test_platform_result_registry_upsert, umi_test_platform_result_registry_remove, UMI_TEST_PLATFORM_RESULT_CAPACITY)

/* Read accepted result records in bounded pages. The shared
 * Framework rule refuses a changed observation before publishing output;
 * this owner's existing row order and normalized fields remain authoritative. */
UMI_DEFINE_SNAPSHOT_REGISTRY_PAGE(umi_test_platform_result_registry_read_page,
    UmiTestPlatformResultRegistry, UmiTestPlatformResultSnapshot, UMI_TEST_PLATFORM_RESULT_CAPACITY)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa68befcd254e2c60);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformResultSnapshot *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformResultSnapshot *)0)->session_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformResultSnapshot *)0)->item_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformResultSnapshot *)0)->message)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformResultSnapshot *)0)->failure_details)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformResultSnapshot *)0)->attachment_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiTestPlatformResultSnapshot *)0)->id) - 1U +
        8U + sizeof(((UmiTestPlatformResultSnapshot *)0)->session_id) - 1U +
        8U + sizeof(((UmiTestPlatformResultSnapshot *)0)->item_id) - 1U +
        8U + sizeof(((UmiTestPlatformResultSnapshot *)0)->message) - 1U +
        8U + sizeof(((UmiTestPlatformResultSnapshot *)0)->failure_details) - 1U +
        8U + sizeof(((UmiTestPlatformResultSnapshot *)0)->attachment_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiTestPlatformResultSnapshot *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->session_id, sizeof(value->session_id));
    UmiArchiveWriteText(writer, value->item_id, sizeof(value->item_id));
    UmiArchiveWriteText(writer, value->message, sizeof(value->message));
    UmiArchiveWriteText(writer, value->failure_details, sizeof(value->failure_details));
    UmiArchiveWriteText(writer, value->attachment_id, sizeof(value->attachment_id));
    UmiArchiveWriteDouble(writer, value->duration_ms);
    UmiArchiveWriteSigned(writer, (int64_t)value->outcome);
    UmiArchiveWriteSigned(writer, (int64_t)value->exit_code);
    UmiArchiveWriteSigned(writer, (int64_t)value->flaky);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiTestPlatformResultSnapshot *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->session_id, sizeof(value->session_id));
    UmiArchiveReadText(reader, value->item_id, sizeof(value->item_id));
    UmiArchiveReadText(reader, value->message, sizeof(value->message));
    UmiArchiveReadText(reader, value->failure_details, sizeof(value->failure_details));
    UmiArchiveReadText(reader, value->attachment_id, sizeof(value->attachment_id));
    value->duration_ms = UmiArchiveReadDouble(reader);
    value->outcome = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->exit_code = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->flaky = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus ArchiveValidate(const UmiTestPlatformResultSnapshot *value)
{
    return umi_test_platform_result_snapshot_validate(value, NULL);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_test_platform_result_snapshot_archive_encode, umi_test_platform_result_snapshot_archive_decode,
    UmiTestPlatformResultSnapshot, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)

/* Restoring this complete collection reuses its existing reviewed replacement
 * rules. Decoding a saved value alone never changes a live registry. */
UMI_DEFINE_REGISTRY_ARCHIVE(umi_test_platform_result_registry_archive_encode, umi_test_platform_result_registry_archive_restore,
    UmiTestPlatformResultRegistry, UmiTestPlatformResultSnapshot, UMI_TEST_PLATFORM_RESULT_CAPACITY, ArchiveSchema,
    umi_test_platform_result_snapshot_archive_encode, umi_test_platform_result_snapshot_archive_decode, umi_test_platform_result_registry_replace_if_current)
