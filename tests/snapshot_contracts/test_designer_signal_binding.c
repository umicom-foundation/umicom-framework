/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_designer_signal_binding.c
 * PURPOSE: Check designer signal_binding input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/signal_binding.h"
#include "umicom/designer/signal_binding.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDesignerSignalBindingSnapshot
#define CONTRACT_REGISTRY UmiDesignerSignalBindingRegistry
#define CONTRACT_CAPACITY UMI_DESIGNER_SIGNAL_BINDING_CAPACITY
#define CONTRACT_VALIDATE umi_designer_signal_binding_snapshot_validate
#define CONTRACT_BATCH umi_designer_signal_binding_registry_upsert_many
#define CONTRACT_CREATE umi_designer_signal_binding_registry_create
#define CONTRACT_DESTROY umi_designer_signal_binding_registry_destroy
#define CONTRACT_UPSERT umi_designer_signal_binding_registry_upsert
#define CONTRACT_REMOVE umi_designer_signal_binding_registry_remove
#define CONTRACT_FIND umi_designer_signal_binding_registry_find
#define CONTRACT_AT umi_designer_signal_binding_registry_at
#define CONTRACT_COUNT umi_designer_signal_binding_registry_count
#define CONTRACT_REVISION umi_designer_signal_binding_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDesignerSignalBindingSnapshot, id), sizeof(((UmiDesignerSignalBindingSnapshot *)0)->id), 1 },
    {"node_id", offsetof(UmiDesignerSignalBindingSnapshot, node_id), sizeof(((UmiDesignerSignalBindingSnapshot *)0)->node_id), 0 },
    {"signal_name", offsetof(UmiDesignerSignalBindingSnapshot, signal_name), sizeof(((UmiDesignerSignalBindingSnapshot *)0)->signal_name), 0 },
    {"command_id", offsetof(UmiDesignerSignalBindingSnapshot, command_id), sizeof(((UmiDesignerSignalBindingSnapshot *)0)->command_id), 0 },
    {"argument", offsetof(UmiDesignerSignalBindingSnapshot, argument), sizeof(((UmiDesignerSignalBindingSnapshot *)0)->argument), 0 }
};
static int ContractSnapshotEqual(const UmiDesignerSignalBindingSnapshot *left, const UmiDesignerSignalBindingSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->node_id, right->node_id, sizeof(left->node_id)) == 0 &&
        memcmp(left->signal_name, right->signal_name, sizeof(left->signal_name)) == 0 &&
        memcmp(left->command_id, right->command_id, sizeof(left->command_id)) == 0 &&
        memcmp(left->argument, right->argument, sizeof(left->argument)) == 0 &&
        left->enabled == right->enabled &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiDesignerSignalBindingSnapshot *item)
{
    item->node_id[0] = 'v';
    item->signal_name[0] = 'v';
    item->command_id[0] = 'v';
    item->argument[0] = 'v';
    item->enabled = (int)8U;
}
#include "snapshot_contract_cases.h"
