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
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_ui_status_item_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiUiStatusItemSnapshot ArchiveSample(void)
{
    UmiUiStatusItemSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiUiStatusItemSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->text) + 1U;
        memset(value->text + used, 0xa5, sizeof(value->text) - used);
    }
    {
        size_t used = strlen(value->tooltip) + 1U;
        memset(value->tooltip + used, 0xa5, sizeof(value->tooltip) - used);
    }
    {
        size_t used = strlen(value->command_id) + 1U;
        memset(value->command_id + used, 0xa5, sizeof(value->command_id) - used);
    }
    {
        size_t used = strlen(value->alignment) + 1U;
        memset(value->alignment + used, 0xa5, sizeof(value->alignment) - used);
    }
}
#define ARCHIVE_TYPE UmiUiStatusItemSnapshot
#define ARCHIVE_ENCODE umi_ui_status_item_snapshot_archive_encode
#define ARCHIVE_DECODE umi_ui_status_item_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_ui_status_item_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_ui_status_item_registry_archive_restore
#include "snapshot_contract_cases.h"
