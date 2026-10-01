/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_drag_drop.c
 * PURPOSE: Exercise ui drag_drop snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/drag_drop.h"
#include "umicom/ui/drag_drop.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiDragDropSnapshot
#define CONTRACT_REGISTRY UmiUiDragDropRegistry
#define CONTRACT_CAPACITY UMI_UI_DRAG_DROP_CAPACITY
#define CONTRACT_VALIDATE umi_ui_drag_drop_snapshot_validate
#define CONTRACT_BATCH umi_ui_drag_drop_registry_upsert_many
#define CONTRACT_CREATE umi_ui_drag_drop_registry_create
#define CONTRACT_DESTROY umi_ui_drag_drop_registry_destroy
#define CONTRACT_UPSERT umi_ui_drag_drop_registry_upsert
#define CONTRACT_REMOVE umi_ui_drag_drop_registry_remove
#define CONTRACT_FIND umi_ui_drag_drop_registry_find
#define CONTRACT_AT umi_ui_drag_drop_registry_at
#define CONTRACT_COUNT umi_ui_drag_drop_registry_count
#define CONTRACT_REVISION umi_ui_drag_drop_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiDragDropSnapshot, id), sizeof(((UmiUiDragDropSnapshot *)0)->id), 1 },
    {"source_id", offsetof(UmiUiDragDropSnapshot, source_id), sizeof(((UmiUiDragDropSnapshot *)0)->source_id), 0 },
    {"target_id", offsetof(UmiUiDragDropSnapshot, target_id), sizeof(((UmiUiDragDropSnapshot *)0)->target_id), 0 },
    {"mime_type", offsetof(UmiUiDragDropSnapshot, mime_type), sizeof(((UmiUiDragDropSnapshot *)0)->mime_type), 0 },
    {"payload", offsetof(UmiUiDragDropSnapshot, payload), sizeof(((UmiUiDragDropSnapshot *)0)->payload), 0 }
};
static int ContractSnapshotEqual(const UmiUiDragDropSnapshot *left,
    const UmiUiDragDropSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->source_id, right->source_id, sizeof(left->source_id)) == 0 &&
        memcmp(left->target_id, right->target_id, sizeof(left->target_id)) == 0 &&
        memcmp(left->mime_type, right->mime_type, sizeof(left->mime_type)) == 0 &&
        memcmp(left->payload, right->payload, sizeof(left->payload)) == 0 &&
        left->allowed == right->allowed &&
        left->copy == right->copy &&
        left->move == right->move &&
        left->link == right->link &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiDragDropSnapshot *item)
{
    item->source_id[0] = 'v';
    item->target_id[0] = 'v';
    item->mime_type[0] = 'v';
    item->payload[0] = 'v';
    item->allowed = (int)8U;
    item->copy = (int)9U;
    item->move = (int)10U;
    item->link = (int)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_drag_drop_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_drag_drop_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiDragDropEdit
#define CONTRACT_EDIT_CURRENT umi_ui_drag_drop_registry_edit_if_current
#include "snapshot_contract_cases.h"
