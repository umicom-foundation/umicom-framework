/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_test_platform_run_session.c
 * PURPOSE: Check test_platform run_session input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/run_session.h"
#include "umicom/test_platform/run_session.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiTestPlatformRunSessionSnapshot
#define CONTRACT_REGISTRY UmiTestPlatformRunSessionRegistry
#define CONTRACT_CAPACITY UMI_TEST_PLATFORM_RUN_SESSION_CAPACITY
#define CONTRACT_VALIDATE umi_test_platform_run_session_snapshot_validate
#define CONTRACT_BATCH umi_test_platform_run_session_registry_upsert_many
#define CONTRACT_CREATE umi_test_platform_run_session_registry_create
#define CONTRACT_DESTROY umi_test_platform_run_session_registry_destroy
#define CONTRACT_UPSERT umi_test_platform_run_session_registry_upsert
#define CONTRACT_REMOVE umi_test_platform_run_session_registry_remove
#define CONTRACT_FIND umi_test_platform_run_session_registry_find
#define CONTRACT_AT umi_test_platform_run_session_registry_at
#define CONTRACT_COUNT umi_test_platform_run_session_registry_count
#define CONTRACT_REVISION umi_test_platform_run_session_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiTestPlatformRunSessionSnapshot, id), sizeof(((UmiTestPlatformRunSessionSnapshot *)0)->id), 1 },
    {"profile_id", offsetof(UmiTestPlatformRunSessionSnapshot, profile_id), sizeof(((UmiTestPlatformRunSessionSnapshot *)0)->profile_id), 0 },
    {"suite_id", offsetof(UmiTestPlatformRunSessionSnapshot, suite_id), sizeof(((UmiTestPlatformRunSessionSnapshot *)0)->suite_id), 0 }
};
static int ContractSnapshotEqual(const UmiTestPlatformRunSessionSnapshot *left, const UmiTestPlatformRunSessionSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->profile_id, right->profile_id, sizeof(left->profile_id)) == 0 &&
        memcmp(left->suite_id, right->suite_id, sizeof(left->suite_id)) == 0 &&
        left->started_at == right->started_at &&
        left->finished_at == right->finished_at &&
        left->total == right->total &&
        left->passed == right->passed &&
        left->failed == right->failed &&
        left->skipped == right->skipped &&
        left->state == right->state &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiTestPlatformRunSessionSnapshot *item)
{
    item->profile_id[0] = 'v';
    item->suite_id[0] = 'v';
    item->started_at = (uint64_t)6U;
    item->finished_at = (uint64_t)7U;
    item->total = (size_t)8U;
    item->passed = (size_t)9U;
    item->failed = (size_t)10U;
    item->skipped = (size_t)11U;
    item->state = (int)12U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_test_platform_run_session_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_test_platform_run_session_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiTestPlatformRunSessionEdit
#define CONTRACT_EDIT_CURRENT umi_test_platform_run_session_registry_edit_if_current
#include "snapshot_contract_cases.h"
