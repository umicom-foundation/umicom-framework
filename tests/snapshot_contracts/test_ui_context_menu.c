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
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_context_menu_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_context_menu_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiContextMenuItemEdit
#define CONTRACT_EDIT_CURRENT umi_ui_context_menu_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_ui_context_menu_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiUiContextMenuItemSnapshot ArchiveSample(void)
{
    UmiUiContextMenuItemSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiUiContextMenuItemSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->menu_id) + 1U;
        memset(value->menu_id + used, 0xa5, sizeof(value->menu_id) - used);
    }
    {
        size_t used = strlen(value->command_id) + 1U;
        memset(value->command_id + used, 0xa5, sizeof(value->command_id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
    {
        size_t used = strlen(value->when_expression) + 1U;
        memset(value->when_expression + used, 0xa5, sizeof(value->when_expression) - used);
    }
    {
        size_t used = strlen(value->group) + 1U;
        memset(value->group + used, 0xa5, sizeof(value->group) - used);
    }
}
#define ARCHIVE_TYPE UmiUiContextMenuItemSnapshot
#define ARCHIVE_ENCODE umi_ui_context_menu_snapshot_archive_encode
#define ARCHIVE_DECODE umi_ui_context_menu_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_ui_context_menu_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_ui_context_menu_registry_archive_restore
#include "snapshot_contract_cases.h"
