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
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_test_platform_run_profile_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_test_platform_run_profile_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiTestPlatformRunProfileEdit
#define CONTRACT_EDIT_CURRENT umi_test_platform_run_profile_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_test_platform_run_profile_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiTestPlatformRunProfileSnapshot ArchiveSample(void)
{
    UmiTestPlatformRunProfileSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiTestPlatformRunProfileSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->mode) + 1U;
        memset(value->mode + used, 0xa5, sizeof(value->mode) - used);
    }
    {
        size_t used = strlen(value->configuration) + 1U;
        memset(value->configuration + used, 0xa5, sizeof(value->configuration) - used);
    }
    {
        size_t used = strlen(value->filter) + 1U;
        memset(value->filter + used, 0xa5, sizeof(value->filter) - used);
    }
}
#define ARCHIVE_TYPE UmiTestPlatformRunProfileSnapshot
#define ARCHIVE_ENCODE umi_test_platform_run_profile_snapshot_archive_encode
#define ARCHIVE_DECODE umi_test_platform_run_profile_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_test_platform_run_profile_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_test_platform_run_profile_registry_archive_restore
#include "snapshot_contract_cases.h"
