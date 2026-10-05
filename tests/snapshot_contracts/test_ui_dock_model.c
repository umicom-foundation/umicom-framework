/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_dock_model.c
 * PURPOSE: Exercise ui dock_model snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/dock_model.h"
#include "umicom/ui/dock_model.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiDockSnapshot
#define CONTRACT_REGISTRY UmiUiDockRegistry
#define CONTRACT_CAPACITY UMI_UI_DOCK_MODEL_CAPACITY
#define CONTRACT_VALIDATE umi_ui_dock_model_snapshot_validate
#define CONTRACT_BATCH umi_ui_dock_model_registry_upsert_many
#define CONTRACT_CREATE umi_ui_dock_model_registry_create
#define CONTRACT_DESTROY umi_ui_dock_model_registry_destroy
#define CONTRACT_UPSERT umi_ui_dock_model_registry_upsert
#define CONTRACT_REMOVE umi_ui_dock_model_registry_remove
#define CONTRACT_FIND umi_ui_dock_model_registry_find
#define CONTRACT_AT umi_ui_dock_model_registry_at
#define CONTRACT_COUNT umi_ui_dock_model_registry_count
#define CONTRACT_REVISION umi_ui_dock_model_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiDockSnapshot, id), sizeof(((UmiUiDockSnapshot *)0)->id), 1 },
    {"title", offsetof(UmiUiDockSnapshot, title), sizeof(((UmiUiDockSnapshot *)0)->title), 0 },
    {"area", offsetof(UmiUiDockSnapshot, area), sizeof(((UmiUiDockSnapshot *)0)->area), 0 },
    {"group_id", offsetof(UmiUiDockSnapshot, group_id), sizeof(((UmiUiDockSnapshot *)0)->group_id), 0 },
    {"active_item_id", offsetof(UmiUiDockSnapshot, active_item_id), sizeof(((UmiUiDockSnapshot *)0)->active_item_id), 0 }
};
static int ContractSnapshotEqual(const UmiUiDockSnapshot *left,
    const UmiUiDockSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->title, right->title, sizeof(left->title)) == 0 &&
        memcmp(left->area, right->area, sizeof(left->area)) == 0 &&
        memcmp(left->group_id, right->group_id, sizeof(left->group_id)) == 0 &&
        memcmp(left->active_item_id, right->active_item_id, sizeof(left->active_item_id)) == 0 &&
        left->visible == right->visible &&
        left->locked == right->locked &&
        left->floating == right->floating &&
        left->order == right->order &&
        left->size == right->size &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiDockSnapshot *item)
{
    item->title[0] = 'v';
    item->area[0] = 'v';
    item->group_id[0] = 'v';
    item->active_item_id[0] = 'v';
    item->visible = (int)8U;
    item->locked = (int)9U;
    item->floating = (int)10U;
    item->order = (int32_t)11U;
    item->size = (int32_t)12U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_dock_model_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_dock_model_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiDockEdit
#define CONTRACT_EDIT_CURRENT umi_ui_dock_model_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_ui_dock_model_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiUiDockSnapshot ArchiveSample(void)
{
    UmiUiDockSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiUiDockSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->title) + 1U;
        memset(value->title + used, 0xa5, sizeof(value->title) - used);
    }
    {
        size_t used = strlen(value->area) + 1U;
        memset(value->area + used, 0xa5, sizeof(value->area) - used);
    }
    {
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
    {
        size_t used = strlen(value->active_item_id) + 1U;
        memset(value->active_item_id + used, 0xa5, sizeof(value->active_item_id) - used);
    }
}
#define ARCHIVE_TYPE UmiUiDockSnapshot
#define ARCHIVE_ENCODE umi_ui_dock_model_snapshot_archive_encode
#define ARCHIVE_DECODE umi_ui_dock_model_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_ui_dock_model_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_ui_dock_model_registry_archive_restore
#include "snapshot_contract_cases.h"
