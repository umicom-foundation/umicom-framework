/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_debug_variable.c
 * PURPOSE: Exercise debug variable snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/variable.h"
#include "umicom/debug/variable.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDebugVariableSnapshot
#define CONTRACT_REGISTRY UmiDebugVariableRegistry
#define CONTRACT_CAPACITY UMI_DEBUG_VARIABLE_CAPACITY
#define CONTRACT_VALIDATE umi_debug_variable_snapshot_validate
#define CONTRACT_BATCH umi_debug_variable_registry_upsert_many
#define CONTRACT_CREATE umi_debug_variable_registry_create
#define CONTRACT_DESTROY umi_debug_variable_registry_destroy
#define CONTRACT_UPSERT umi_debug_variable_registry_upsert
#define CONTRACT_REMOVE umi_debug_variable_registry_remove
#define CONTRACT_FIND umi_debug_variable_registry_find
#define CONTRACT_AT umi_debug_variable_registry_at
#define CONTRACT_COUNT umi_debug_variable_registry_count
#define CONTRACT_REVISION umi_debug_variable_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDebugVariableSnapshot, id), sizeof(((UmiDebugVariableSnapshot *)0)->id), 1 },
    {"scope_id", offsetof(UmiDebugVariableSnapshot, scope_id), sizeof(((UmiDebugVariableSnapshot *)0)->scope_id), 0 },
    {"name", offsetof(UmiDebugVariableSnapshot, name), sizeof(((UmiDebugVariableSnapshot *)0)->name), 0 },
    {"value", offsetof(UmiDebugVariableSnapshot, value), sizeof(((UmiDebugVariableSnapshot *)0)->value), 0 },
    {"type", offsetof(UmiDebugVariableSnapshot, type), sizeof(((UmiDebugVariableSnapshot *)0)->type), 0 },
    {"evaluate_name", offsetof(UmiDebugVariableSnapshot, evaluate_name), sizeof(((UmiDebugVariableSnapshot *)0)->evaluate_name), 0 }
};
static int ContractSnapshotEqual(const UmiDebugVariableSnapshot *left,
    const UmiDebugVariableSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->scope_id, right->scope_id, sizeof(left->scope_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->value, right->value, sizeof(left->value)) == 0 &&
        memcmp(left->type, right->type, sizeof(left->type)) == 0 &&
        memcmp(left->evaluate_name, right->evaluate_name, sizeof(left->evaluate_name)) == 0 &&
        left->variables_reference == right->variables_reference &&
        left->changed == right->changed &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiDebugVariableSnapshot *item)
{
    item->scope_id[0] = 'v';
    item->name[0] = 'v';
    item->value[0] = 'v';
    item->type[0] = 'v';
    item->evaluate_name[0] = 'v';
    item->variables_reference = (uint64_t)9U;
    item->changed = (int)10U;
}
#include "snapshot_contract_cases.h"
