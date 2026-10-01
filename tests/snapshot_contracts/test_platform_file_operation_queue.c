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
#include "snapshot_contract_cases.h"
