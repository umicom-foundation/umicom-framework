/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_debug_scope.c
 * PURPOSE: Exercise debug scope snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/scope.h"
#include "umicom/debug/scope.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDebugScopeSnapshot
#define CONTRACT_REGISTRY UmiDebugScopeRegistry
#define CONTRACT_CAPACITY UMI_DEBUG_SCOPE_CAPACITY
#define CONTRACT_VALIDATE umi_debug_scope_snapshot_validate
#define CONTRACT_BATCH umi_debug_scope_registry_upsert_many
#define CONTRACT_CREATE umi_debug_scope_registry_create
#define CONTRACT_DESTROY umi_debug_scope_registry_destroy
#define CONTRACT_UPSERT umi_debug_scope_registry_upsert
#define CONTRACT_REMOVE umi_debug_scope_registry_remove
#define CONTRACT_FIND umi_debug_scope_registry_find
#define CONTRACT_AT umi_debug_scope_registry_at
#define CONTRACT_COUNT umi_debug_scope_registry_count
#define CONTRACT_REVISION umi_debug_scope_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDebugScopeSnapshot, id), sizeof(((UmiDebugScopeSnapshot *)0)->id), 1 },
    {"frame_id", offsetof(UmiDebugScopeSnapshot, frame_id), sizeof(((UmiDebugScopeSnapshot *)0)->frame_id), 0 },
    {"name", offsetof(UmiDebugScopeSnapshot, name), sizeof(((UmiDebugScopeSnapshot *)0)->name), 0 }
};
static int ContractSnapshotEqual(const UmiDebugScopeSnapshot *left,
    const UmiDebugScopeSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->frame_id, right->frame_id, sizeof(left->frame_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        left->variables_reference == right->variables_reference &&
        left->expensive == right->expensive &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiDebugScopeSnapshot *item)
{
    item->frame_id[0] = 'v';
    item->name[0] = 'v';
    item->variables_reference = (uint64_t)6U;
    item->expensive = (int)7U;
    item->order = (int32_t)8U;
}
#include "snapshot_contract_cases.h"
