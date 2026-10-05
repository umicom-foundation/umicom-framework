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
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiDesignerActionBindingSnapshot ArchiveSample(void)
{
    UmiDesignerActionBindingSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiDesignerActionBindingSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->node_id) + 1U;
        memset(value->node_id + used, 0xa5, sizeof(value->node_id) - used);
    }
    {
        size_t used = strlen(value->action_name) + 1U;
        memset(value->action_name + used, 0xa5, sizeof(value->action_name) - used);
    }
    {
        size_t used = strlen(value->command_id) + 1U;
        memset(value->command_id + used, 0xa5, sizeof(value->command_id) - used);
    }
    {
        size_t used = strlen(value->state_path) + 1U;
        memset(value->state_path + used, 0xa5, sizeof(value->state_path) - used);
    }
}
#define ARCHIVE_TYPE UmiDesignerActionBindingSnapshot
#define ARCHIVE_ENCODE umi_designer_action_binding_snapshot_archive_encode
#define ARCHIVE_DECODE umi_designer_action_binding_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_designer_action_binding_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_designer_action_binding_registry_archive_restore
#include "snapshot_contract_cases.h"
