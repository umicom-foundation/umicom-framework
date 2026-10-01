/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_status_item.c
 * PURPOSE: Exercise ui status_item snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/status_item.h"
#include "umicom/ui/status_item.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiStatusItemSnapshot
#define CONTRACT_REGISTRY UmiUiStatusItemRegistry
#define CONTRACT_CAPACITY UMI_UI_STATUS_ITEM_CAPACITY
#define CONTRACT_VALIDATE umi_ui_status_item_snapshot_validate
#define CONTRACT_BATCH umi_ui_status_item_registry_upsert_many
#define CONTRACT_CREATE umi_ui_status_item_registry_create
#define CONTRACT_DESTROY umi_ui_status_item_registry_destroy
#define CONTRACT_UPSERT umi_ui_status_item_registry_upsert
#define CONTRACT_REMOVE umi_ui_status_item_registry_remove
#define CONTRACT_FIND umi_ui_status_item_registry_find
#define CONTRACT_AT umi_ui_status_item_registry_at
#define CONTRACT_COUNT umi_ui_status_item_registry_count
#define CONTRACT_REVISION umi_ui_status_item_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiStatusItemSnapshot, id), sizeof(((UmiUiStatusItemSnapshot *)0)->id), 1 },
    {"text", offsetof(UmiUiStatusItemSnapshot, text), sizeof(((UmiUiStatusItemSnapshot *)0)->text), 0 },
    {"tooltip", offsetof(UmiUiStatusItemSnapshot, tooltip), sizeof(((UmiUiStatusItemSnapshot *)0)->tooltip), 0 },
    {"command_id", offsetof(UmiUiStatusItemSnapshot, command_id), sizeof(((UmiUiStatusItemSnapshot *)0)->command_id), 0 },
    {"alignment", offsetof(UmiUiStatusItemSnapshot, alignment), sizeof(((UmiUiStatusItemSnapshot *)0)->alignment), 0 }
};
static int ContractSnapshotEqual(const UmiUiStatusItemSnapshot *left,
    const UmiUiStatusItemSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->text, right->text, sizeof(left->text)) == 0 &&
        memcmp(left->tooltip, right->tooltip, sizeof(left->tooltip)) == 0 &&
        memcmp(left->command_id, right->command_id, sizeof(left->command_id)) == 0 &&
        memcmp(left->alignment, right->alignment, sizeof(left->alignment)) == 0 &&
        left->visible == right->visible &&
        left->priority == right->priority &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiStatusItemSnapshot *item)
{
    item->text[0] = 'v';
    item->tooltip[0] = 'v';
    item->command_id[0] = 'v';
    item->alignment[0] = 'v';
    item->visible = (int)8U;
    item->priority = (int32_t)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_status_item_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_status_item_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiStatusItemEdit
#define CONTRACT_EDIT_CURRENT umi_ui_status_item_registry_edit_if_current
#include "snapshot_contract_cases.h"
