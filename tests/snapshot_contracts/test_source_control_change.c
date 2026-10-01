/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_source_control_change.c
 * PURPOSE: Exercise source_control change snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/source_control/change.h"
#include "umicom/source_control/change.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiSourceControlChangeSnapshot
#define CONTRACT_REGISTRY UmiSourceControlChangeRegistry
#define CONTRACT_CAPACITY UMI_SOURCE_CONTROL_CHANGE_CAPACITY
#define CONTRACT_VALIDATE umi_source_control_change_snapshot_validate
#define CONTRACT_BATCH umi_source_control_change_registry_upsert_many
#define CONTRACT_CREATE umi_source_control_change_registry_create
#define CONTRACT_DESTROY umi_source_control_change_registry_destroy
#define CONTRACT_UPSERT umi_source_control_change_registry_upsert
#define CONTRACT_REMOVE umi_source_control_change_registry_remove
#define CONTRACT_FIND umi_source_control_change_registry_find
#define CONTRACT_AT umi_source_control_change_registry_at
#define CONTRACT_COUNT umi_source_control_change_registry_count
#define CONTRACT_REVISION umi_source_control_change_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiSourceControlChangeSnapshot, id), sizeof(((UmiSourceControlChangeSnapshot *)0)->id), 1 },
    {"repository_id", offsetof(UmiSourceControlChangeSnapshot, repository_id), sizeof(((UmiSourceControlChangeSnapshot *)0)->repository_id), 0 },
    {"uri", offsetof(UmiSourceControlChangeSnapshot, uri), sizeof(((UmiSourceControlChangeSnapshot *)0)->uri), 0 },
    {"status", offsetof(UmiSourceControlChangeSnapshot, status), sizeof(((UmiSourceControlChangeSnapshot *)0)->status), 0 },
    {"old_uri", offsetof(UmiSourceControlChangeSnapshot, old_uri), sizeof(((UmiSourceControlChangeSnapshot *)0)->old_uri), 0 }
};
static int ContractSnapshotEqual(const UmiSourceControlChangeSnapshot *left,
    const UmiSourceControlChangeSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->repository_id, right->repository_id, sizeof(left->repository_id)) == 0 &&
        memcmp(left->uri, right->uri, sizeof(left->uri)) == 0 &&
        memcmp(left->status, right->status, sizeof(left->status)) == 0 &&
        memcmp(left->old_uri, right->old_uri, sizeof(left->old_uri)) == 0 &&
        left->staged == right->staged &&
        left->conflict == right->conflict &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiSourceControlChangeSnapshot *item)
{
    item->repository_id[0] = 'v';
    item->uri[0] = 'v';
    item->status[0] = 'v';
    item->old_uri[0] = 'v';
    item->staged = (int)8U;
    item->conflict = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_source_control_change_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_source_control_change_registry_replace_if_current
#include "snapshot_contract_cases.h"
