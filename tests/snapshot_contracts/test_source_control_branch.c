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
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_source_control_branch_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_source_control_branch_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiSourceControlBranchEdit
#define CONTRACT_EDIT_CURRENT umi_source_control_branch_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_source_control_branch_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiSourceControlBranchSnapshot ArchiveSample(void)
{
    UmiSourceControlBranchSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiSourceControlBranchSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->repository_id) + 1U;
        memset(value->repository_id + used, 0xa5, sizeof(value->repository_id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->upstream) + 1U;
        memset(value->upstream + used, 0xa5, sizeof(value->upstream) - used);
    }
    {
        size_t used = strlen(value->head) + 1U;
        memset(value->head + used, 0xa5, sizeof(value->head) - used);
    }
}
#define ARCHIVE_TYPE UmiSourceControlBranchSnapshot
#define ARCHIVE_ENCODE umi_source_control_branch_snapshot_archive_encode
#define ARCHIVE_DECODE umi_source_control_branch_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_source_control_branch_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_source_control_branch_registry_archive_restore
#include "snapshot_contract_cases.h"
