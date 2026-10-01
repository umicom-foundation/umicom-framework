/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_source_control_staging.c
 * PURPOSE: Exercise source_control staging snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/source_control/staging.h"
#include "umicom/source_control/staging.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiSourceControlStagingSnapshot
#define CONTRACT_REGISTRY UmiSourceControlStagingRegistry
#define CONTRACT_CAPACITY UMI_SOURCE_CONTROL_STAGING_CAPACITY
#define CONTRACT_VALIDATE umi_source_control_staging_snapshot_validate
#define CONTRACT_BATCH umi_source_control_staging_registry_upsert_many
#define CONTRACT_CREATE umi_source_control_staging_registry_create
#define CONTRACT_DESTROY umi_source_control_staging_registry_destroy
#define CONTRACT_UPSERT umi_source_control_staging_registry_upsert
#define CONTRACT_REMOVE umi_source_control_staging_registry_remove
#define CONTRACT_FIND umi_source_control_staging_registry_find
#define CONTRACT_AT umi_source_control_staging_registry_at
#define CONTRACT_COUNT umi_source_control_staging_registry_count
#define CONTRACT_REVISION umi_source_control_staging_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiSourceControlStagingSnapshot, id), sizeof(((UmiSourceControlStagingSnapshot *)0)->id), 1 },
    {"repository_id", offsetof(UmiSourceControlStagingSnapshot, repository_id), sizeof(((UmiSourceControlStagingSnapshot *)0)->repository_id), 0 },
    {"change_id", offsetof(UmiSourceControlStagingSnapshot, change_id), sizeof(((UmiSourceControlStagingSnapshot *)0)->change_id), 0 },
    {"hunk_id", offsetof(UmiSourceControlStagingSnapshot, hunk_id), sizeof(((UmiSourceControlStagingSnapshot *)0)->hunk_id), 0 }
};
static int ContractSnapshotEqual(const UmiSourceControlStagingSnapshot *left,
    const UmiSourceControlStagingSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->repository_id, right->repository_id, sizeof(left->repository_id)) == 0 &&
        memcmp(left->change_id, right->change_id, sizeof(left->change_id)) == 0 &&
        memcmp(left->hunk_id, right->hunk_id, sizeof(left->hunk_id)) == 0 &&
        left->staged == right->staged &&
        left->partial == right->partial &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiSourceControlStagingSnapshot *item)
{
    item->repository_id[0] = 'v';
    item->change_id[0] = 'v';
    item->hunk_id[0] = 'v';
    item->staged = (int)7U;
    item->partial = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_source_control_staging_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_source_control_staging_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiSourceControlStagingEdit
#define CONTRACT_EDIT_CURRENT umi_source_control_staging_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_source_control_staging_registry_read_page
#include "snapshot_contract_cases.h"
