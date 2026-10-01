/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_platform_workspace_history.c
 * PURPOSE: Exercise platform workspace_history snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/workspace_history.h"
#include "umicom/platform/workspace_history.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiWorkspaceHistorySnapshot
#define CONTRACT_REGISTRY UmiWorkspaceHistoryRegistry
#define CONTRACT_CAPACITY UMI_PLATFORM_WORKSPACE_HISTORY_CAPACITY
#define CONTRACT_VALIDATE umi_platform_workspace_history_snapshot_validate
#define CONTRACT_BATCH umi_platform_workspace_history_registry_upsert_many
#define CONTRACT_CREATE umi_platform_workspace_history_registry_create
#define CONTRACT_DESTROY umi_platform_workspace_history_registry_destroy
#define CONTRACT_UPSERT umi_platform_workspace_history_registry_upsert
#define CONTRACT_REMOVE umi_platform_workspace_history_registry_remove
#define CONTRACT_FIND umi_platform_workspace_history_registry_find
#define CONTRACT_AT umi_platform_workspace_history_registry_at
#define CONTRACT_COUNT umi_platform_workspace_history_registry_count
#define CONTRACT_REVISION umi_platform_workspace_history_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiWorkspaceHistorySnapshot, id), sizeof(((UmiWorkspaceHistorySnapshot *)0)->id), 1 },
    {"root_uri", offsetof(UmiWorkspaceHistorySnapshot, root_uri), sizeof(((UmiWorkspaceHistorySnapshot *)0)->root_uri), 0 },
    {"label", offsetof(UmiWorkspaceHistorySnapshot, label), sizeof(((UmiWorkspaceHistorySnapshot *)0)->label), 0 },
    {"profile", offsetof(UmiWorkspaceHistorySnapshot, profile), sizeof(((UmiWorkspaceHistorySnapshot *)0)->profile), 0 }
};
static int ContractSnapshotEqual(const UmiWorkspaceHistorySnapshot *left,
    const UmiWorkspaceHistorySnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->root_uri, right->root_uri, sizeof(left->root_uri)) == 0 &&
        memcmp(left->label, right->label, sizeof(left->label)) == 0 &&
        memcmp(left->profile, right->profile, sizeof(left->profile)) == 0 &&
        left->last_opened == right->last_opened &&
        left->duration_seconds == right->duration_seconds &&
        left->trusted == right->trusted &&
        left->pinned == right->pinned &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiWorkspaceHistorySnapshot *item)
{
    item->root_uri[0] = 'v';
    item->label[0] = 'v';
    item->profile[0] = 'v';
    item->last_opened = (uint64_t)7U;
    item->duration_seconds = (uint64_t)8U;
    item->trusted = (int)9U;
    item->pinned = (int)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_platform_workspace_history_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_platform_workspace_history_registry_replace_if_current
#include "snapshot_contract_cases.h"
