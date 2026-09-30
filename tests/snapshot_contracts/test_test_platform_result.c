/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_test_platform_result.c
 * PURPOSE: Check test_platform result input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/result.h"
#include "umicom/test_platform/result.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiTestPlatformResultSnapshot
#define CONTRACT_REGISTRY UmiTestPlatformResultRegistry
#define CONTRACT_CAPACITY UMI_TEST_PLATFORM_RESULT_CAPACITY
#define CONTRACT_VALIDATE umi_test_platform_result_snapshot_validate
#define CONTRACT_BATCH umi_test_platform_result_registry_upsert_many
#define CONTRACT_CREATE umi_test_platform_result_registry_create
#define CONTRACT_DESTROY umi_test_platform_result_registry_destroy
#define CONTRACT_UPSERT umi_test_platform_result_registry_upsert
#define CONTRACT_REMOVE umi_test_platform_result_registry_remove
#define CONTRACT_FIND umi_test_platform_result_registry_find
#define CONTRACT_AT umi_test_platform_result_registry_at
#define CONTRACT_COUNT umi_test_platform_result_registry_count
#define CONTRACT_REVISION umi_test_platform_result_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiTestPlatformResultSnapshot, id), sizeof(((UmiTestPlatformResultSnapshot *)0)->id), 1 },
    {"session_id", offsetof(UmiTestPlatformResultSnapshot, session_id), sizeof(((UmiTestPlatformResultSnapshot *)0)->session_id), 0 },
    {"item_id", offsetof(UmiTestPlatformResultSnapshot, item_id), sizeof(((UmiTestPlatformResultSnapshot *)0)->item_id), 0 },
    {"message", offsetof(UmiTestPlatformResultSnapshot, message), sizeof(((UmiTestPlatformResultSnapshot *)0)->message), 0 },
    {"failure_details", offsetof(UmiTestPlatformResultSnapshot, failure_details), sizeof(((UmiTestPlatformResultSnapshot *)0)->failure_details), 0 },
    {"attachment_id", offsetof(UmiTestPlatformResultSnapshot, attachment_id), sizeof(((UmiTestPlatformResultSnapshot *)0)->attachment_id), 0 }
};
static int ContractSnapshotEqual(const UmiTestPlatformResultSnapshot *left, const UmiTestPlatformResultSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->session_id, right->session_id, sizeof(left->session_id)) == 0 &&
        memcmp(left->item_id, right->item_id, sizeof(left->item_id)) == 0 &&
        memcmp(left->message, right->message, sizeof(left->message)) == 0 &&
        memcmp(left->failure_details, right->failure_details, sizeof(left->failure_details)) == 0 &&
        memcmp(left->attachment_id, right->attachment_id, sizeof(left->attachment_id)) == 0 &&
        left->duration_ms == right->duration_ms &&
        left->outcome == right->outcome &&
        left->exit_code == right->exit_code &&
        left->flaky == right->flaky &&
        left->sequence == right->sequence &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiTestPlatformResultSnapshot *item)
{
    item->session_id[0] = 'v';
    item->item_id[0] = 'v';
    item->message[0] = 'v';
    item->failure_details[0] = 'v';
    item->attachment_id[0] = 'v';
    item->duration_ms = (double)9U;
    item->outcome = (int)10U;
    item->exit_code = (int)11U;
    item->flaky = (int)12U;
    item->sequence = (uint64_t)13U;
}
#include "snapshot_contract_cases.h"
