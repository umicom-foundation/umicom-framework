/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_list_model.c
 * PURPOSE: Exercise ui list_model snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/list_model.h"
#include "umicom/ui/list_model.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiListModelSnapshot
#define CONTRACT_REGISTRY UmiUiListModelRegistry
#define CONTRACT_CAPACITY UMI_UI_LIST_MODEL_CAPACITY
#define CONTRACT_VALIDATE umi_ui_list_model_snapshot_validate
#define CONTRACT_BATCH umi_ui_list_model_registry_upsert_many
#define CONTRACT_CREATE umi_ui_list_model_registry_create
#define CONTRACT_DESTROY umi_ui_list_model_registry_destroy
#define CONTRACT_UPSERT umi_ui_list_model_registry_upsert
#define CONTRACT_REMOVE umi_ui_list_model_registry_remove
#define CONTRACT_FIND umi_ui_list_model_registry_find
#define CONTRACT_AT umi_ui_list_model_registry_at
#define CONTRACT_COUNT umi_ui_list_model_registry_count
#define CONTRACT_REVISION umi_ui_list_model_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiListModelSnapshot, id), sizeof(((UmiUiListModelSnapshot *)0)->id), 1 },
    {"label", offsetof(UmiUiListModelSnapshot, label), sizeof(((UmiUiListModelSnapshot *)0)->label), 0 },
    {"description", offsetof(UmiUiListModelSnapshot, description), sizeof(((UmiUiListModelSnapshot *)0)->description), 0 },
    {"icon_name", offsetof(UmiUiListModelSnapshot, icon_name), sizeof(((UmiUiListModelSnapshot *)0)->icon_name), 0 }
};
static int ContractSnapshotEqual(const UmiUiListModelSnapshot *left,
    const UmiUiListModelSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->label, right->label, sizeof(left->label)) == 0 &&
        memcmp(left->description, right->description, sizeof(left->description)) == 0 &&
        memcmp(left->icon_name, right->icon_name, sizeof(left->icon_name)) == 0 &&
        left->visible == right->visible &&
        left->enabled == right->enabled &&
        left->checked == right->checked &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiListModelSnapshot *item)
{
    item->label[0] = 'v';
    item->description[0] = 'v';
    item->icon_name[0] = 'v';
    item->visible = (int)7U;
    item->enabled = (int)8U;
    item->checked = (int)9U;
    item->order = (int32_t)10U;
}
/* Visible/enabled are normalised to truth values; checked/order are payload. */
static void ContractNormalise(UmiUiListModelSnapshot *item)
{
    item->visible = item->visible != 0;
    item->enabled = item->enabled != 0;
}
#define CONTRACT_NORMALISE(item) ContractNormalise(item)
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_list_model_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_list_model_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiListModelEdit
#define CONTRACT_EDIT_CURRENT umi_ui_list_model_registry_edit_if_current
#include "snapshot_contract_cases.h"
