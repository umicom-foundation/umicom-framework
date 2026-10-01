/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_view_state.c
 * PURPOSE: Exercise ui view_state snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/view_state.h"
#include "umicom/ui/view_state.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiViewStateSnapshot
#define CONTRACT_REGISTRY UmiUiViewStateRegistry
#define CONTRACT_CAPACITY UMI_UI_VIEW_STATE_CAPACITY
#define CONTRACT_VALIDATE umi_ui_view_state_snapshot_validate
#define CONTRACT_BATCH umi_ui_view_state_registry_upsert_many
#define CONTRACT_CREATE umi_ui_view_state_registry_create
#define CONTRACT_DESTROY umi_ui_view_state_registry_destroy
#define CONTRACT_UPSERT umi_ui_view_state_registry_upsert
#define CONTRACT_REMOVE umi_ui_view_state_registry_remove
#define CONTRACT_FIND umi_ui_view_state_registry_find
#define CONTRACT_AT umi_ui_view_state_registry_at
#define CONTRACT_COUNT umi_ui_view_state_registry_count
#define CONTRACT_REVISION umi_ui_view_state_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiViewStateSnapshot, id), sizeof(((UmiUiViewStateSnapshot *)0)->id), 1 },
    {"view_id", offsetof(UmiUiViewStateSnapshot, view_id), sizeof(((UmiUiViewStateSnapshot *)0)->view_id), 0 },
    {"workspace_id", offsetof(UmiUiViewStateSnapshot, workspace_id), sizeof(((UmiUiViewStateSnapshot *)0)->workspace_id), 0 },
    {"state_key", offsetof(UmiUiViewStateSnapshot, state_key), sizeof(((UmiUiViewStateSnapshot *)0)->state_key), 0 },
    {"state_value", offsetof(UmiUiViewStateSnapshot, state_value), sizeof(((UmiUiViewStateSnapshot *)0)->state_value), 0 }
};
static int ContractSnapshotEqual(const UmiUiViewStateSnapshot *left,
    const UmiUiViewStateSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->view_id, right->view_id, sizeof(left->view_id)) == 0 &&
        memcmp(left->workspace_id, right->workspace_id, sizeof(left->workspace_id)) == 0 &&
        memcmp(left->state_key, right->state_key, sizeof(left->state_key)) == 0 &&
        memcmp(left->state_value, right->state_value, sizeof(left->state_value)) == 0 &&
        left->persistent == right->persistent &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiViewStateSnapshot *item)
{
    item->view_id[0] = 'v';
    item->workspace_id[0] = 'v';
    item->state_key[0] = 'v';
    item->state_value[0] = 'v';
    item->persistent = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_view_state_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_view_state_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiViewStateEdit
#define CONTRACT_EDIT_CURRENT umi_ui_view_state_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_ui_view_state_registry_read_page
#include "snapshot_contract_cases.h"
