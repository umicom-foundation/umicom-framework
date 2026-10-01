/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_designer_action_binding.c
 * PURPOSE: Check designer action_binding input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/action_binding.h"
#include "umicom/designer/action_binding.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDesignerActionBindingSnapshot
#define CONTRACT_REGISTRY UmiDesignerActionBindingRegistry
#define CONTRACT_CAPACITY UMI_DESIGNER_ACTION_BINDING_CAPACITY
#define CONTRACT_VALIDATE umi_designer_action_binding_snapshot_validate
#define CONTRACT_BATCH umi_designer_action_binding_registry_upsert_many
#define CONTRACT_CREATE umi_designer_action_binding_registry_create
#define CONTRACT_DESTROY umi_designer_action_binding_registry_destroy
#define CONTRACT_UPSERT umi_designer_action_binding_registry_upsert
#define CONTRACT_REMOVE umi_designer_action_binding_registry_remove
#define CONTRACT_FIND umi_designer_action_binding_registry_find
#define CONTRACT_AT umi_designer_action_binding_registry_at
#define CONTRACT_COUNT umi_designer_action_binding_registry_count
#define CONTRACT_REVISION umi_designer_action_binding_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDesignerActionBindingSnapshot, id), sizeof(((UmiDesignerActionBindingSnapshot *)0)->id), 1 },
    {"node_id", offsetof(UmiDesignerActionBindingSnapshot, node_id), sizeof(((UmiDesignerActionBindingSnapshot *)0)->node_id), 0 },
    {"action_name", offsetof(UmiDesignerActionBindingSnapshot, action_name), sizeof(((UmiDesignerActionBindingSnapshot *)0)->action_name), 0 },
    {"command_id", offsetof(UmiDesignerActionBindingSnapshot, command_id), sizeof(((UmiDesignerActionBindingSnapshot *)0)->command_id), 0 },
    {"state_path", offsetof(UmiDesignerActionBindingSnapshot, state_path), sizeof(((UmiDesignerActionBindingSnapshot *)0)->state_path), 0 }
};
static int ContractSnapshotEqual(const UmiDesignerActionBindingSnapshot *left, const UmiDesignerActionBindingSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->node_id, right->node_id, sizeof(left->node_id)) == 0 &&
        memcmp(left->action_name, right->action_name, sizeof(left->action_name)) == 0 &&
        memcmp(left->command_id, right->command_id, sizeof(left->command_id)) == 0 &&
        memcmp(left->state_path, right->state_path, sizeof(left->state_path)) == 0 &&
        left->enabled == right->enabled &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiDesignerActionBindingSnapshot *item)
{
    item->node_id[0] = 'v';
    item->action_name[0] = 'v';
    item->command_id[0] = 'v';
    item->state_path[0] = 'v';
    item->enabled = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_designer_action_binding_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_designer_action_binding_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiDesignerActionBindingEdit
#define CONTRACT_EDIT_CURRENT umi_designer_action_binding_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_designer_action_binding_registry_read_page
#include "snapshot_contract_cases.h"
