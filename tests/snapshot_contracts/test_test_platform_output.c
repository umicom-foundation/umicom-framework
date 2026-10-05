/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_test_platform_output.c
 * PURPOSE: Check test_platform output input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/output.h"
#include "umicom/test_platform/output.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiTestPlatformOutputSnapshot
#define CONTRACT_REGISTRY UmiTestPlatformOutputRegistry
#define CONTRACT_CAPACITY UMI_TEST_PLATFORM_OUTPUT_CAPACITY
#define CONTRACT_VALIDATE umi_test_platform_output_snapshot_validate
#define CONTRACT_BATCH umi_test_platform_output_registry_upsert_many
#define CONTRACT_CREATE umi_test_platform_output_registry_create
#define CONTRACT_DESTROY umi_test_platform_output_registry_destroy
#define CONTRACT_UPSERT umi_test_platform_output_registry_upsert
#define CONTRACT_REMOVE umi_test_platform_output_registry_remove
#define CONTRACT_FIND umi_test_platform_output_registry_find
#define CONTRACT_AT umi_test_platform_output_registry_at
#define CONTRACT_COUNT umi_test_platform_output_registry_count
#define CONTRACT_REVISION umi_test_platform_output_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiTestPlatformOutputSnapshot, id), sizeof(((UmiTestPlatformOutputSnapshot *)0)->id), 1 },
    {"session_id", offsetof(UmiTestPlatformOutputSnapshot, session_id), sizeof(((UmiTestPlatformOutputSnapshot *)0)->session_id), 0 },
    {"item_id", offsetof(UmiTestPlatformOutputSnapshot, item_id), sizeof(((UmiTestPlatformOutputSnapshot *)0)->item_id), 0 },
    {"stream", offsetof(UmiTestPlatformOutputSnapshot, stream), sizeof(((UmiTestPlatformOutputSnapshot *)0)->stream), 0 },
    {"text", offsetof(UmiTestPlatformOutputSnapshot, text), sizeof(((UmiTestPlatformOutputSnapshot *)0)->text), 0 }
};
static int ContractSnapshotEqual(const UmiTestPlatformOutputSnapshot *left, const UmiTestPlatformOutputSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->session_id, right->session_id, sizeof(left->session_id)) == 0 &&
        memcmp(left->item_id, right->item_id, sizeof(left->item_id)) == 0 &&
        memcmp(left->stream, right->stream, sizeof(left->stream)) == 0 &&
        memcmp(left->text, right->text, sizeof(left->text)) == 0 &&
        left->timestamp == right->timestamp &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiTestPlatformOutputSnapshot *item)
{
    item->session_id[0] = 'v';
    item->item_id[0] = 'v';
    item->stream[0] = 'v';
    item->text[0] = 'v';
    item->timestamp = (uint64_t)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_test_platform_output_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_test_platform_output_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiTestPlatformOutputEdit
#define CONTRACT_EDIT_CURRENT umi_test_platform_output_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_test_platform_output_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiTestPlatformOutputSnapshot ArchiveSample(void)
{
    UmiTestPlatformOutputSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiTestPlatformOutputSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
    {
        size_t used = strlen(value->item_id) + 1U;
        memset(value->item_id + used, 0xa5, sizeof(value->item_id) - used);
    }
    {
        size_t used = strlen(value->stream) + 1U;
        memset(value->stream + used, 0xa5, sizeof(value->stream) - used);
    }
    {
        size_t used = strlen(value->text) + 1U;
        memset(value->text + used, 0xa5, sizeof(value->text) - used);
    }
}
#define ARCHIVE_TYPE UmiTestPlatformOutputSnapshot
#define ARCHIVE_ENCODE umi_test_platform_output_snapshot_archive_encode
#define ARCHIVE_DECODE umi_test_platform_output_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_test_platform_output_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_test_platform_output_registry_archive_restore
#include "snapshot_contract_cases.h"
