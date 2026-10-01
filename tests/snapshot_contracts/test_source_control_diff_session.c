/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_source_control_diff_session.c
 * PURPOSE: Exercise source_control diff_session snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/source_control/diff_session.h"
#include "umicom/source_control/diff_session.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiSourceControlDiffSessionSnapshot
#define CONTRACT_REGISTRY UmiSourceControlDiffSessionRegistry
#define CONTRACT_CAPACITY UMI_SOURCE_CONTROL_DIFF_SESSION_CAPACITY
#define CONTRACT_VALIDATE umi_source_control_diff_session_snapshot_validate
#define CONTRACT_BATCH umi_source_control_diff_session_registry_upsert_many
#define CONTRACT_CREATE umi_source_control_diff_session_registry_create
#define CONTRACT_DESTROY umi_source_control_diff_session_registry_destroy
#define CONTRACT_UPSERT umi_source_control_diff_session_registry_upsert
#define CONTRACT_REMOVE umi_source_control_diff_session_registry_remove
#define CONTRACT_FIND umi_source_control_diff_session_registry_find
#define CONTRACT_AT umi_source_control_diff_session_registry_at
#define CONTRACT_COUNT umi_source_control_diff_session_registry_count
#define CONTRACT_REVISION umi_source_control_diff_session_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiSourceControlDiffSessionSnapshot, id), sizeof(((UmiSourceControlDiffSessionSnapshot *)0)->id), 1 },
    {"repository_id", offsetof(UmiSourceControlDiffSessionSnapshot, repository_id), sizeof(((UmiSourceControlDiffSessionSnapshot *)0)->repository_id), 0 },
    {"left_revision", offsetof(UmiSourceControlDiffSessionSnapshot, left_revision), sizeof(((UmiSourceControlDiffSessionSnapshot *)0)->left_revision), 0 },
    {"right_revision", offsetof(UmiSourceControlDiffSessionSnapshot, right_revision), sizeof(((UmiSourceControlDiffSessionSnapshot *)0)->right_revision), 0 },
    {"path", offsetof(UmiSourceControlDiffSessionSnapshot, path), sizeof(((UmiSourceControlDiffSessionSnapshot *)0)->path), 0 }
};
static int ContractSnapshotEqual(const UmiSourceControlDiffSessionSnapshot *left,
    const UmiSourceControlDiffSessionSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->repository_id, right->repository_id, sizeof(left->repository_id)) == 0 &&
        memcmp(left->left_revision, right->left_revision, sizeof(left->left_revision)) == 0 &&
        memcmp(left->right_revision, right->right_revision, sizeof(left->right_revision)) == 0 &&
        memcmp(left->path, right->path, sizeof(left->path)) == 0 &&
        left->hunk_count == right->hunk_count &&
        left->binary == right->binary &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiSourceControlDiffSessionSnapshot *item)
{
    item->repository_id[0] = 'v';
    item->left_revision[0] = 'v';
    item->right_revision[0] = 'v';
    item->path[0] = 'v';
    item->hunk_count = (size_t)8U;
    item->binary = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_source_control_diff_session_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_source_control_diff_session_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiSourceControlDiffSessionEdit
#define CONTRACT_EDIT_CURRENT umi_source_control_diff_session_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_source_control_diff_session_registry_read_page
#include "snapshot_contract_cases.h"
