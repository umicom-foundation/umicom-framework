/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_context_menu.c
 * PURPOSE: Exercise ui context_menu snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/context_menu.h"
#include "umicom/ui/context_menu.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiContextMenuItemSnapshot
#define CONTRACT_REGISTRY UmiUiContextMenuItemRegistry
#define CONTRACT_CAPACITY UMI_UI_CONTEXT_MENU_CAPACITY
#define CONTRACT_VALIDATE umi_ui_context_menu_snapshot_validate
#define CONTRACT_BATCH umi_ui_context_menu_registry_upsert_many
#define CONTRACT_CREATE umi_ui_context_menu_registry_create
#define CONTRACT_DESTROY umi_ui_context_menu_registry_destroy
#define CONTRACT_UPSERT umi_ui_context_menu_registry_upsert
#define CONTRACT_REMOVE umi_ui_context_menu_registry_remove
#define CONTRACT_FIND umi_ui_context_menu_registry_find
#define CONTRACT_AT umi_ui_context_menu_registry_at
#define CONTRACT_COUNT umi_ui_context_menu_registry_count
#define CONTRACT_REVISION umi_ui_context_menu_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiContextMenuItemSnapshot, id), sizeof(((UmiUiContextMenuItemSnapshot *)0)->id), 1 },
    {"menu_id", offsetof(UmiUiContextMenuItemSnapshot, menu_id), sizeof(((UmiUiContextMenuItemSnapshot *)0)->menu_id), 0 },
    {"command_id", offsetof(UmiUiContextMenuItemSnapshot, command_id), sizeof(((UmiUiContextMenuItemSnapshot *)0)->command_id), 0 },
    {"label", offsetof(UmiUiContextMenuItemSnapshot, label), sizeof(((UmiUiContextMenuItemSnapshot *)0)->label), 0 },
    {"when_expression", offsetof(UmiUiContextMenuItemSnapshot, when_expression), sizeof(((UmiUiContextMenuItemSnapshot *)0)->when_expression), 0 },
    {"group", offsetof(UmiUiContextMenuItemSnapshot, group), sizeof(((UmiUiContextMenuItemSnapshot *)0)->group), 0 }
};
static int ContractSnapshotEqual(const UmiUiContextMenuItemSnapshot *left,
    const UmiUiContextMenuItemSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->menu_id, right->menu_id, sizeof(left->menu_id)) == 0 &&
        memcmp(left->command_id, right->command_id, sizeof(left->command_id)) == 0 &&
        memcmp(left->label, right->label, sizeof(left->label)) == 0 &&
        memcmp(left->when_expression, right->when_expression, sizeof(left->when_expression)) == 0 &&
        memcmp(left->group, right->group, sizeof(left->group)) == 0 &&
        left->visible == right->visible &&
        left->enabled == right->enabled &&
        left->separator_before == right->separator_before &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiContextMenuItemSnapshot *item)
{
    item->menu_id[0] = 'v';
    item->command_id[0] = 'v';
    item->label[0] = 'v';
    item->when_expression[0] = 'v';
    item->group[0] = 'v';
    item->visible = (int)9U;
    item->enabled = (int)10U;
    item->separator_before = (int)11U;
    item->order = (int32_t)12U;
}
#include "snapshot_contract_cases.h"
