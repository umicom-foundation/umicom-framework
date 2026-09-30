/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_source_control_branch.c
 * PURPOSE: Exercise source_control branch snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/source_control/branch.h"
#include "umicom/source_control/branch.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiSourceControlBranchSnapshot
#define CONTRACT_REGISTRY UmiSourceControlBranchRegistry
#define CONTRACT_CAPACITY UMI_SOURCE_CONTROL_BRANCH_CAPACITY
#define CONTRACT_VALIDATE umi_source_control_branch_snapshot_validate
#define CONTRACT_BATCH umi_source_control_branch_registry_upsert_many
#define CONTRACT_CREATE umi_source_control_branch_registry_create
#define CONTRACT_DESTROY umi_source_control_branch_registry_destroy
#define CONTRACT_UPSERT umi_source_control_branch_registry_upsert
#define CONTRACT_REMOVE umi_source_control_branch_registry_remove
#define CONTRACT_FIND umi_source_control_branch_registry_find
#define CONTRACT_AT umi_source_control_branch_registry_at
#define CONTRACT_COUNT umi_source_control_branch_registry_count
#define CONTRACT_REVISION umi_source_control_branch_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiSourceControlBranchSnapshot, id), sizeof(((UmiSourceControlBranchSnapshot *)0)->id), 1 },
    {"repository_id", offsetof(UmiSourceControlBranchSnapshot, repository_id), sizeof(((UmiSourceControlBranchSnapshot *)0)->repository_id), 0 },
    {"name", offsetof(UmiSourceControlBranchSnapshot, name), sizeof(((UmiSourceControlBranchSnapshot *)0)->name), 0 },
    {"upstream", offsetof(UmiSourceControlBranchSnapshot, upstream), sizeof(((UmiSourceControlBranchSnapshot *)0)->upstream), 0 },
    {"head", offsetof(UmiSourceControlBranchSnapshot, head), sizeof(((UmiSourceControlBranchSnapshot *)0)->head), 0 }
};
static int ContractSnapshotEqual(const UmiSourceControlBranchSnapshot *left,
    const UmiSourceControlBranchSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->repository_id, right->repository_id, sizeof(left->repository_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->upstream, right->upstream, sizeof(left->upstream)) == 0 &&
        memcmp(left->head, right->head, sizeof(left->head)) == 0 &&
        left->current == right->current &&
        left->remote == right->remote &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiSourceControlBranchSnapshot *item)
{
    item->repository_id[0] = 'v';
    item->name[0] = 'v';
    item->upstream[0] = 'v';
    item->head[0] = 'v';
    item->current = (int)8U;
    item->remote = (int)9U;
}
#include "snapshot_contract_cases.h"
