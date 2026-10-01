/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_source_control_remote.c
 * PURPOSE: Exercise source_control remote snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/source_control/remote.h"
#include "umicom/source_control/remote.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiSourceControlRemoteSnapshot
#define CONTRACT_REGISTRY UmiSourceControlRemoteRegistry
#define CONTRACT_CAPACITY UMI_SOURCE_CONTROL_REMOTE_CAPACITY
#define CONTRACT_VALIDATE umi_source_control_remote_snapshot_validate
#define CONTRACT_BATCH umi_source_control_remote_registry_upsert_many
#define CONTRACT_CREATE umi_source_control_remote_registry_create
#define CONTRACT_DESTROY umi_source_control_remote_registry_destroy
#define CONTRACT_UPSERT umi_source_control_remote_registry_upsert
#define CONTRACT_REMOVE umi_source_control_remote_registry_remove
#define CONTRACT_FIND umi_source_control_remote_registry_find
#define CONTRACT_AT umi_source_control_remote_registry_at
#define CONTRACT_COUNT umi_source_control_remote_registry_count
#define CONTRACT_REVISION umi_source_control_remote_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiSourceControlRemoteSnapshot, id), sizeof(((UmiSourceControlRemoteSnapshot *)0)->id), 1 },
    {"repository_id", offsetof(UmiSourceControlRemoteSnapshot, repository_id), sizeof(((UmiSourceControlRemoteSnapshot *)0)->repository_id), 0 },
    {"name", offsetof(UmiSourceControlRemoteSnapshot, name), sizeof(((UmiSourceControlRemoteSnapshot *)0)->name), 0 },
    {"fetch_url", offsetof(UmiSourceControlRemoteSnapshot, fetch_url), sizeof(((UmiSourceControlRemoteSnapshot *)0)->fetch_url), 0 },
    {"push_url", offsetof(UmiSourceControlRemoteSnapshot, push_url), sizeof(((UmiSourceControlRemoteSnapshot *)0)->push_url), 0 }
};
static int ContractSnapshotEqual(const UmiSourceControlRemoteSnapshot *left,
    const UmiSourceControlRemoteSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->repository_id, right->repository_id, sizeof(left->repository_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->fetch_url, right->fetch_url, sizeof(left->fetch_url)) == 0 &&
        memcmp(left->push_url, right->push_url, sizeof(left->push_url)) == 0 &&
        left->default_remote == right->default_remote &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiSourceControlRemoteSnapshot *item)
{
    item->repository_id[0] = 'v';
    item->name[0] = 'v';
    item->fetch_url[0] = 'v';
    item->push_url[0] = 'v';
    item->default_remote = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_source_control_remote_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_source_control_remote_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiSourceControlRemoteEdit
#define CONTRACT_EDIT_CURRENT umi_source_control_remote_registry_edit_if_current
#include "snapshot_contract_cases.h"
