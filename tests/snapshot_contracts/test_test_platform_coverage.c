/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_test_platform_coverage.c
 * PURPOSE: Check test_platform coverage input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/coverage.h"
#include "umicom/test_platform/coverage.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiTestPlatformCoverageSnapshot
#define CONTRACT_REGISTRY UmiTestPlatformCoverageRegistry
#define CONTRACT_CAPACITY UMI_TEST_PLATFORM_COVERAGE_CAPACITY
#define CONTRACT_VALIDATE umi_test_platform_coverage_snapshot_validate
#define CONTRACT_BATCH umi_test_platform_coverage_registry_upsert_many
#define CONTRACT_CREATE umi_test_platform_coverage_registry_create
#define CONTRACT_DESTROY umi_test_platform_coverage_registry_destroy
#define CONTRACT_UPSERT umi_test_platform_coverage_registry_upsert
#define CONTRACT_REMOVE umi_test_platform_coverage_registry_remove
#define CONTRACT_FIND umi_test_platform_coverage_registry_find
#define CONTRACT_AT umi_test_platform_coverage_registry_at
#define CONTRACT_COUNT umi_test_platform_coverage_registry_count
#define CONTRACT_REVISION umi_test_platform_coverage_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiTestPlatformCoverageSnapshot, id), sizeof(((UmiTestPlatformCoverageSnapshot *)0)->id), 1 },
    {"session_id", offsetof(UmiTestPlatformCoverageSnapshot, session_id), sizeof(((UmiTestPlatformCoverageSnapshot *)0)->session_id), 0 },
    {"uri", offsetof(UmiTestPlatformCoverageSnapshot, uri), sizeof(((UmiTestPlatformCoverageSnapshot *)0)->uri), 0 }
};
static int ContractSnapshotEqual(const UmiTestPlatformCoverageSnapshot *left, const UmiTestPlatformCoverageSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->session_id, right->session_id, sizeof(left->session_id)) == 0 &&
        memcmp(left->uri, right->uri, sizeof(left->uri)) == 0 &&
        left->lines_total == right->lines_total &&
        left->lines_covered == right->lines_covered &&
        left->branches_total == right->branches_total &&
        left->branches_covered == right->branches_covered &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiTestPlatformCoverageSnapshot *item)
{
    item->session_id[0] = 'v';
    item->uri[0] = 'v';
    item->lines_total = (uint64_t)6U;
    item->lines_covered = (uint64_t)7U;
    item->branches_total = (uint64_t)8U;
    item->branches_covered = (uint64_t)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_test_platform_coverage_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_test_platform_coverage_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiTestPlatformCoverageEdit
#define CONTRACT_EDIT_CURRENT umi_test_platform_coverage_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_test_platform_coverage_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiTestPlatformCoverageSnapshot ArchiveSample(void)
{
    UmiTestPlatformCoverageSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiTestPlatformCoverageSnapshot *value)
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
        size_t used = strlen(value->uri) + 1U;
        memset(value->uri + used, 0xa5, sizeof(value->uri) - used);
    }
}
#define ARCHIVE_TYPE UmiTestPlatformCoverageSnapshot
#define ARCHIVE_ENCODE umi_test_platform_coverage_snapshot_archive_encode
#define ARCHIVE_DECODE umi_test_platform_coverage_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_test_platform_coverage_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_test_platform_coverage_registry_archive_restore
#include "snapshot_contract_cases.h"
