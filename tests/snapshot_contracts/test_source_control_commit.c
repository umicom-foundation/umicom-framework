/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_source_control_commit.c
 * PURPOSE: Exercise source_control commit snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/source_control/commit.h"
#include "umicom/source_control/commit.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiSourceControlCommitSnapshot
#define CONTRACT_REGISTRY UmiSourceControlCommitRegistry
#define CONTRACT_CAPACITY UMI_SOURCE_CONTROL_COMMIT_CAPACITY
#define CONTRACT_VALIDATE umi_source_control_commit_snapshot_validate
#define CONTRACT_BATCH umi_source_control_commit_registry_upsert_many
#define CONTRACT_CREATE umi_source_control_commit_registry_create
#define CONTRACT_DESTROY umi_source_control_commit_registry_destroy
#define CONTRACT_UPSERT umi_source_control_commit_registry_upsert
#define CONTRACT_REMOVE umi_source_control_commit_registry_remove
#define CONTRACT_FIND umi_source_control_commit_registry_find
#define CONTRACT_AT umi_source_control_commit_registry_at
#define CONTRACT_COUNT umi_source_control_commit_registry_count
#define CONTRACT_REVISION umi_source_control_commit_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiSourceControlCommitSnapshot, id), sizeof(((UmiSourceControlCommitSnapshot *)0)->id), 1 },
    {"repository_id", offsetof(UmiSourceControlCommitSnapshot, repository_id), sizeof(((UmiSourceControlCommitSnapshot *)0)->repository_id), 0 },
    {"hash", offsetof(UmiSourceControlCommitSnapshot, hash), sizeof(((UmiSourceControlCommitSnapshot *)0)->hash), 0 },
    {"author", offsetof(UmiSourceControlCommitSnapshot, author), sizeof(((UmiSourceControlCommitSnapshot *)0)->author), 0 },
    {"email", offsetof(UmiSourceControlCommitSnapshot, email), sizeof(((UmiSourceControlCommitSnapshot *)0)->email), 0 },
    {"subject", offsetof(UmiSourceControlCommitSnapshot, subject), sizeof(((UmiSourceControlCommitSnapshot *)0)->subject), 0 }
};
static int ContractSnapshotEqual(const UmiSourceControlCommitSnapshot *left,
    const UmiSourceControlCommitSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->repository_id, right->repository_id, sizeof(left->repository_id)) == 0 &&
        memcmp(left->hash, right->hash, sizeof(left->hash)) == 0 &&
        memcmp(left->author, right->author, sizeof(left->author)) == 0 &&
        memcmp(left->email, right->email, sizeof(left->email)) == 0 &&
        memcmp(left->subject, right->subject, sizeof(left->subject)) == 0 &&
        left->timestamp == right->timestamp &&
        left->head == right->head &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiSourceControlCommitSnapshot *item)
{
    item->repository_id[0] = 'v';
    item->hash[0] = 'v';
    item->author[0] = 'v';
    item->email[0] = 'v';
    item->subject[0] = 'v';
    item->timestamp = (uint64_t)9U;
    item->head = (int)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_source_control_commit_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_source_control_commit_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiSourceControlCommitEdit
#define CONTRACT_EDIT_CURRENT umi_source_control_commit_registry_edit_if_current
#include "snapshot_contract_cases.h"
