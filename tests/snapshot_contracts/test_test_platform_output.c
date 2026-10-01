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
#include "snapshot_contract_cases.h"
