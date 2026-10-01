/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_tab_model.c
 * PURPOSE: Exercise ui tab_model snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/tab_model.h"
#include "umicom/ui/tab_model.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiTabSnapshot
#define CONTRACT_REGISTRY UmiUiTabRegistry
#define CONTRACT_CAPACITY UMI_UI_TAB_MODEL_CAPACITY
#define CONTRACT_VALIDATE umi_ui_tab_model_snapshot_validate
#define CONTRACT_BATCH umi_ui_tab_model_registry_upsert_many
#define CONTRACT_CREATE umi_ui_tab_model_registry_create
#define CONTRACT_DESTROY umi_ui_tab_model_registry_destroy
#define CONTRACT_UPSERT umi_ui_tab_model_registry_upsert
#define CONTRACT_REMOVE umi_ui_tab_model_registry_remove
#define CONTRACT_FIND umi_ui_tab_model_registry_find
#define CONTRACT_AT umi_ui_tab_model_registry_at
#define CONTRACT_COUNT umi_ui_tab_model_registry_count
#define CONTRACT_REVISION umi_ui_tab_model_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiTabSnapshot, id), sizeof(((UmiUiTabSnapshot *)0)->id), 1 },
    {"group_id", offsetof(UmiUiTabSnapshot, group_id), sizeof(((UmiUiTabSnapshot *)0)->group_id), 0 },
    {"title", offsetof(UmiUiTabSnapshot, title), sizeof(((UmiUiTabSnapshot *)0)->title), 0 },
    {"resource", offsetof(UmiUiTabSnapshot, resource), sizeof(((UmiUiTabSnapshot *)0)->resource), 0 },
    {"icon_name", offsetof(UmiUiTabSnapshot, icon_name), sizeof(((UmiUiTabSnapshot *)0)->icon_name), 0 }
};
static int ContractSnapshotEqual(const UmiUiTabSnapshot *left,
    const UmiUiTabSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->group_id, right->group_id, sizeof(left->group_id)) == 0 &&
        memcmp(left->title, right->title, sizeof(left->title)) == 0 &&
        memcmp(left->resource, right->resource, sizeof(left->resource)) == 0 &&
        memcmp(left->icon_name, right->icon_name, sizeof(left->icon_name)) == 0 &&
        left->active == right->active &&
        left->pinned == right->pinned &&
        left->preview == right->preview &&
        left->dirty == right->dirty &&
        left->closable == right->closable &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiTabSnapshot *item)
{
    item->group_id[0] = 'v';
    item->title[0] = 'v';
    item->resource[0] = 'v';
    item->icon_name[0] = 'v';
    item->active = (int)8U;
    item->pinned = (int)9U;
    item->preview = (int)10U;
    item->dirty = (int)11U;
    item->closable = (int)12U;
    item->order = (int32_t)13U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_tab_model_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_tab_model_registry_replace_if_current
#include "snapshot_contract_cases.h"
