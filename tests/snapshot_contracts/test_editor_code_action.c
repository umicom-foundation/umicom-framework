/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_editor_code_action.c
 * PURPOSE: Check editor code_action input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/code_action.h"
#include "umicom/editor/code_action.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiEditorCodeActionSnapshot
#define CONTRACT_REGISTRY UmiEditorCodeActionRegistry
#define CONTRACT_CAPACITY UMI_EDITOR_CODE_ACTION_CAPACITY
#define CONTRACT_VALIDATE umi_editor_code_action_snapshot_validate
#define CONTRACT_REPLACE_DOCUMENT umi_editor_code_action_registry_replace_document
#define CONTRACT_BATCH umi_editor_code_action_registry_upsert_many
#define CONTRACT_CREATE umi_editor_code_action_registry_create
#define CONTRACT_DESTROY umi_editor_code_action_registry_destroy
#define CONTRACT_UPSERT umi_editor_code_action_registry_upsert
#define CONTRACT_REMOVE umi_editor_code_action_registry_remove
#define CONTRACT_FIND umi_editor_code_action_registry_find
#define CONTRACT_AT umi_editor_code_action_registry_at
#define CONTRACT_COUNT umi_editor_code_action_registry_count
#define CONTRACT_REVISION umi_editor_code_action_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiEditorCodeActionSnapshot, id), sizeof(((UmiEditorCodeActionSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiEditorCodeActionSnapshot, document_id), sizeof(((UmiEditorCodeActionSnapshot *)0)->document_id), 0 },
    {"title", offsetof(UmiEditorCodeActionSnapshot, title), sizeof(((UmiEditorCodeActionSnapshot *)0)->title), 0 },
    {"kind", offsetof(UmiEditorCodeActionSnapshot, kind), sizeof(((UmiEditorCodeActionSnapshot *)0)->kind), 0 },
    {"command_id", offsetof(UmiEditorCodeActionSnapshot, command_id), sizeof(((UmiEditorCodeActionSnapshot *)0)->command_id), 0 },
    {"argument", offsetof(UmiEditorCodeActionSnapshot, argument), sizeof(((UmiEditorCodeActionSnapshot *)0)->argument), 0 }
};
static int ContractSnapshotEqual(const UmiEditorCodeActionSnapshot *left, const UmiEditorCodeActionSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->title, right->title, sizeof(left->title)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->command_id, right->command_id, sizeof(left->command_id)) == 0 &&
        memcmp(left->argument, right->argument, sizeof(left->argument)) == 0 &&
        left->preferred == right->preferred &&
        left->enabled == right->enabled &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiEditorCodeActionSnapshot *item)
{
    item->document_id[0] = 'v';
    item->title[0] = 'v';
    item->kind[0] = 'v';
    item->command_id[0] = 'v';
    item->argument[0] = 'v';
    item->preferred = (int)9U;
    item->enabled = (int)10U;
}
#include "snapshot_contract_cases.h"
