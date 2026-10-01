/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_test_platform_attachment.c
 * PURPOSE: Check test_platform attachment input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/attachment.h"
#include "umicom/test_platform/attachment.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiTestPlatformAttachmentSnapshot
#define CONTRACT_REGISTRY UmiTestPlatformAttachmentRegistry
#define CONTRACT_CAPACITY UMI_TEST_PLATFORM_ATTACHMENT_CAPACITY
#define CONTRACT_VALIDATE umi_test_platform_attachment_snapshot_validate
#define CONTRACT_BATCH umi_test_platform_attachment_registry_upsert_many
#define CONTRACT_CREATE umi_test_platform_attachment_registry_create
#define CONTRACT_DESTROY umi_test_platform_attachment_registry_destroy
#define CONTRACT_UPSERT umi_test_platform_attachment_registry_upsert
#define CONTRACT_REMOVE umi_test_platform_attachment_registry_remove
#define CONTRACT_FIND umi_test_platform_attachment_registry_find
#define CONTRACT_AT umi_test_platform_attachment_registry_at
#define CONTRACT_COUNT umi_test_platform_attachment_registry_count
#define CONTRACT_REVISION umi_test_platform_attachment_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiTestPlatformAttachmentSnapshot, id), sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->id), 1 },
    {"result_id", offsetof(UmiTestPlatformAttachmentSnapshot, result_id), sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->result_id), 0 },
    {"name", offsetof(UmiTestPlatformAttachmentSnapshot, name), sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->name), 0 },
    {"kind", offsetof(UmiTestPlatformAttachmentSnapshot, kind), sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->kind), 0 },
    {"producer", offsetof(UmiTestPlatformAttachmentSnapshot, producer), sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->producer), 0 },
    {"uri", offsetof(UmiTestPlatformAttachmentSnapshot, uri), sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->uri), 0 },
    {"mime_type", offsetof(UmiTestPlatformAttachmentSnapshot, mime_type), sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->mime_type), 0 },
    {"schema_uri", offsetof(UmiTestPlatformAttachmentSnapshot, schema_uri), sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->schema_uri), 0 },
    {"checksum", offsetof(UmiTestPlatformAttachmentSnapshot, checksum), sizeof(((UmiTestPlatformAttachmentSnapshot *)0)->checksum), 0 }
};
static int ContractSnapshotEqual(const UmiTestPlatformAttachmentSnapshot *left, const UmiTestPlatformAttachmentSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->result_id, right->result_id, sizeof(left->result_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->producer, right->producer, sizeof(left->producer)) == 0 &&
        memcmp(left->uri, right->uri, sizeof(left->uri)) == 0 &&
        memcmp(left->mime_type, right->mime_type, sizeof(left->mime_type)) == 0 &&
        memcmp(left->schema_uri, right->schema_uri, sizeof(left->schema_uri)) == 0 &&
        memcmp(left->checksum, right->checksum, sizeof(left->checksum)) == 0 &&
        left->size_bytes == right->size_bytes &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiTestPlatformAttachmentSnapshot *item)
{
    item->result_id[0] = 'v';
    item->name[0] = 'v';
    item->kind[0] = 'v';
    item->producer[0] = 'v';
    item->uri[0] = 'v';
    item->mime_type[0] = 'v';
    item->schema_uri[0] = 'v';
    item->checksum[0] = 'v';
    item->size_bytes = (uint64_t)12U;
}
#define CONTRACT_API_VERSION UMI_TEST_PLATFORM_ATTACHMENT_API_VERSION
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_test_platform_attachment_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_test_platform_attachment_registry_replace_if_current
#include "snapshot_contract_cases.h"
