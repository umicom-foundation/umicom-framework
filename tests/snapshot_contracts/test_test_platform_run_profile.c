/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_test_platform_run_profile.c
 * PURPOSE: Check test_platform run_profile input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/run_profile.h"
#include "umicom/test_platform/run_profile.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiTestPlatformRunProfileSnapshot
#define CONTRACT_REGISTRY UmiTestPlatformRunProfileRegistry
#define CONTRACT_CAPACITY UMI_TEST_PLATFORM_RUN_PROFILE_CAPACITY
#define CONTRACT_VALIDATE umi_test_platform_run_profile_snapshot_validate
#define CONTRACT_BATCH umi_test_platform_run_profile_registry_upsert_many
#define CONTRACT_CREATE umi_test_platform_run_profile_registry_create
#define CONTRACT_DESTROY umi_test_platform_run_profile_registry_destroy
#define CONTRACT_UPSERT umi_test_platform_run_profile_registry_upsert
#define CONTRACT_REMOVE umi_test_platform_run_profile_registry_remove
#define CONTRACT_FIND umi_test_platform_run_profile_registry_find
#define CONTRACT_AT umi_test_platform_run_profile_registry_at
#define CONTRACT_COUNT umi_test_platform_run_profile_registry_count
#define CONTRACT_REVISION umi_test_platform_run_profile_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiTestPlatformRunProfileSnapshot, id), sizeof(((UmiTestPlatformRunProfileSnapshot *)0)->id), 1 },
    {"name", offsetof(UmiTestPlatformRunProfileSnapshot, name), sizeof(((UmiTestPlatformRunProfileSnapshot *)0)->name), 0 },
    {"mode", offsetof(UmiTestPlatformRunProfileSnapshot, mode), sizeof(((UmiTestPlatformRunProfileSnapshot *)0)->mode), 0 },
    {"configuration", offsetof(UmiTestPlatformRunProfileSnapshot, configuration), sizeof(((UmiTestPlatformRunProfileSnapshot *)0)->configuration), 0 },
    {"filter", offsetof(UmiTestPlatformRunProfileSnapshot, filter), sizeof(((UmiTestPlatformRunProfileSnapshot *)0)->filter), 0 }
};
static int ContractSnapshotEqual(const UmiTestPlatformRunProfileSnapshot *left, const UmiTestPlatformRunProfileSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->mode, right->mode, sizeof(left->mode)) == 0 &&
        memcmp(left->configuration, right->configuration, sizeof(left->configuration)) == 0 &&
        memcmp(left->filter, right->filter, sizeof(left->filter)) == 0 &&
        left->debug == right->debug &&
        left->coverage == right->coverage &&
        left->default_profile == right->default_profile &&
        left->include_disabled == right->include_disabled &&
        left->stop_on_failure == right->stop_on_failure &&
        left->repeat_count == right->repeat_count &&
        left->timeout_ms == right->timeout_ms &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiTestPlatformRunProfileSnapshot *item)
{
    item->name[0] = 'v';
    item->mode[0] = 'v';
    item->configuration[0] = 'v';
    item->filter[0] = 'v';
    item->debug = (int)8U;
    item->coverage = (int)9U;
    item->default_profile = (int)10U;
    item->include_disabled = (int)11U;
    item->stop_on_failure = (int)12U;
    item->repeat_count = (uint32_t)13U;
    item->timeout_ms = (uint32_t)14U;
}
#define CONTRACT_API_VERSION UMI_TEST_PLATFORM_RUN_PROFILE_API_VERSION
#include "snapshot_contract_cases.h"
