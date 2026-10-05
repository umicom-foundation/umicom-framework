/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_platform_file_operation_queue.c
 * PURPOSE: Exercise platform file_operation_queue snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/file_operation_queue.h"
#include "umicom/platform/file_operation_queue.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiFileOperationSnapshot
#define CONTRACT_REGISTRY UmiFileOperationRegistry
#define CONTRACT_CAPACITY UMI_PLATFORM_FILE_OPERATION_QUEUE_CAPACITY
#define CONTRACT_VALIDATE umi_platform_file_operation_queue_snapshot_validate
#define CONTRACT_BATCH umi_platform_file_operation_queue_registry_upsert_many
#define CONTRACT_CREATE umi_platform_file_operation_queue_registry_create
#define CONTRACT_DESTROY umi_platform_file_operation_queue_registry_destroy
#define CONTRACT_UPSERT umi_platform_file_operation_queue_registry_upsert
#define CONTRACT_REMOVE umi_platform_file_operation_queue_registry_remove
#define CONTRACT_FIND umi_platform_file_operation_queue_registry_find
#define CONTRACT_AT umi_platform_file_operation_queue_registry_at
#define CONTRACT_COUNT umi_platform_file_operation_queue_registry_count
#define CONTRACT_REVISION umi_platform_file_operation_queue_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiFileOperationSnapshot, id), sizeof(((UmiFileOperationSnapshot *)0)->id), 1 },
    {"operation", offsetof(UmiFileOperationSnapshot, operation), sizeof(((UmiFileOperationSnapshot *)0)->operation), 0 },
    {"source_uri", offsetof(UmiFileOperationSnapshot, source_uri), sizeof(((UmiFileOperationSnapshot *)0)->source_uri), 0 },
    {"target_uri", offsetof(UmiFileOperationSnapshot, target_uri), sizeof(((UmiFileOperationSnapshot *)0)->target_uri), 0 },
    {"error_text", offsetof(UmiFileOperationSnapshot, error_text), sizeof(((UmiFileOperationSnapshot *)0)->error_text), 0 }
};
static int ContractSnapshotEqual(const UmiFileOperationSnapshot *left,
    const UmiFileOperationSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->operation, right->operation, sizeof(left->operation)) == 0 &&
        memcmp(left->source_uri, right->source_uri, sizeof(left->source_uri)) == 0 &&
        memcmp(left->target_uri, right->target_uri, sizeof(left->target_uri)) == 0 &&
        memcmp(left->error_text, right->error_text, sizeof(left->error_text)) == 0 &&
        left->bytes_total == right->bytes_total &&
        left->bytes_done == right->bytes_done &&
        left->state == right->state &&
        left->cancellable == right->cancellable &&
        left->overwrite == right->overwrite &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiFileOperationSnapshot *item)
{
    item->operation[0] = 'v';
    item->source_uri[0] = 'v';
    item->target_uri[0] = 'v';
    item->error_text[0] = 'v';
    item->bytes_total = (uint64_t)8U;
    item->bytes_done = (uint64_t)9U;
    item->state = (int)10U;
    item->cancellable = (int)11U;
    item->overwrite = (int)12U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_platform_file_operation_queue_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_platform_file_operation_queue_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiFileOperationEdit
#define CONTRACT_EDIT_CURRENT umi_platform_file_operation_queue_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_platform_file_operation_queue_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiFileOperationSnapshot ArchiveSample(void)
{
    UmiFileOperationSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiFileOperationSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->operation) + 1U;
        memset(value->operation + used, 0xa5, sizeof(value->operation) - used);
    }
    {
        size_t used = strlen(value->source_uri) + 1U;
        memset(value->source_uri + used, 0xa5, sizeof(value->source_uri) - used);
    }
    {
        size_t used = strlen(value->target_uri) + 1U;
        memset(value->target_uri + used, 0xa5, sizeof(value->target_uri) - used);
    }
    {
        size_t used = strlen(value->error_text) + 1U;
        memset(value->error_text + used, 0xa5, sizeof(value->error_text) - used);
    }
}
#define ARCHIVE_TYPE UmiFileOperationSnapshot
#define ARCHIVE_ENCODE umi_platform_file_operation_queue_snapshot_archive_encode
#define ARCHIVE_DECODE umi_platform_file_operation_queue_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_platform_file_operation_queue_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_platform_file_operation_queue_registry_archive_restore
#include "snapshot_contract_cases.h"
