/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_panel_model.c
 * PURPOSE: Exercise ui panel_model snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/panel_model.h"
#include "umicom/ui/panel_model.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiPanelSnapshot
#define CONTRACT_REGISTRY UmiUiPanelRegistry
#define CONTRACT_CAPACITY UMI_UI_PANEL_MODEL_CAPACITY
#define CONTRACT_VALIDATE umi_ui_panel_model_snapshot_validate
#define CONTRACT_BATCH umi_ui_panel_model_registry_upsert_many
#define CONTRACT_CREATE umi_ui_panel_model_registry_create
#define CONTRACT_DESTROY umi_ui_panel_model_registry_destroy
#define CONTRACT_UPSERT umi_ui_panel_model_registry_upsert
#define CONTRACT_REMOVE umi_ui_panel_model_registry_remove
#define CONTRACT_FIND umi_ui_panel_model_registry_find
#define CONTRACT_AT umi_ui_panel_model_registry_at
#define CONTRACT_COUNT umi_ui_panel_model_registry_count
#define CONTRACT_REVISION umi_ui_panel_model_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiPanelSnapshot, id), sizeof(((UmiUiPanelSnapshot *)0)->id), 1 },
    {"title", offsetof(UmiUiPanelSnapshot, title), sizeof(((UmiUiPanelSnapshot *)0)->title), 0 },
    {"location", offsetof(UmiUiPanelSnapshot, location), sizeof(((UmiUiPanelSnapshot *)0)->location), 0 },
    {"view_id", offsetof(UmiUiPanelSnapshot, view_id), sizeof(((UmiUiPanelSnapshot *)0)->view_id), 0 },
    {"icon_name", offsetof(UmiUiPanelSnapshot, icon_name), sizeof(((UmiUiPanelSnapshot *)0)->icon_name), 0 }
};
static int ContractSnapshotEqual(const UmiUiPanelSnapshot *left,
    const UmiUiPanelSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->title, right->title, sizeof(left->title)) == 0 &&
        memcmp(left->location, right->location, sizeof(left->location)) == 0 &&
        memcmp(left->view_id, right->view_id, sizeof(left->view_id)) == 0 &&
        memcmp(left->icon_name, right->icon_name, sizeof(left->icon_name)) == 0 &&
        left->visible == right->visible &&
        left->maximised == right->maximised &&
        left->order == right->order &&
        left->preferred_size == right->preferred_size &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiPanelSnapshot *item)
{
    item->title[0] = 'v';
    item->location[0] = 'v';
    item->view_id[0] = 'v';
    item->icon_name[0] = 'v';
    item->visible = (int)8U;
    item->maximised = (int)9U;
    item->order = (int32_t)10U;
    item->preferred_size = (int32_t)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_panel_model_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_panel_model_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiPanelEdit
#define CONTRACT_EDIT_CURRENT umi_ui_panel_model_registry_edit_if_current
#include "snapshot_contract_cases.h"
