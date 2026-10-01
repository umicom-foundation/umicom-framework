/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_welcome_view.c
 * PURPOSE: Exercise ui welcome_view snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/welcome_view.h"
#include "umicom/ui/welcome_view.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiWelcomeItemSnapshot
#define CONTRACT_REGISTRY UmiUiWelcomeItemRegistry
#define CONTRACT_CAPACITY UMI_UI_WELCOME_VIEW_CAPACITY
#define CONTRACT_VALIDATE umi_ui_welcome_view_snapshot_validate
#define CONTRACT_BATCH umi_ui_welcome_view_registry_upsert_many
#define CONTRACT_CREATE umi_ui_welcome_view_registry_create
#define CONTRACT_DESTROY umi_ui_welcome_view_registry_destroy
#define CONTRACT_UPSERT umi_ui_welcome_view_registry_upsert
#define CONTRACT_REMOVE umi_ui_welcome_view_registry_remove
#define CONTRACT_FIND umi_ui_welcome_view_registry_find
#define CONTRACT_AT umi_ui_welcome_view_registry_at
#define CONTRACT_COUNT umi_ui_welcome_view_registry_count
#define CONTRACT_REVISION umi_ui_welcome_view_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiWelcomeItemSnapshot, id), sizeof(((UmiUiWelcomeItemSnapshot *)0)->id), 1 },
    {"view_id", offsetof(UmiUiWelcomeItemSnapshot, view_id), sizeof(((UmiUiWelcomeItemSnapshot *)0)->view_id), 0 },
    {"title", offsetof(UmiUiWelcomeItemSnapshot, title), sizeof(((UmiUiWelcomeItemSnapshot *)0)->title), 0 },
    {"description", offsetof(UmiUiWelcomeItemSnapshot, description), sizeof(((UmiUiWelcomeItemSnapshot *)0)->description), 0 },
    {"command_id", offsetof(UmiUiWelcomeItemSnapshot, command_id), sizeof(((UmiUiWelcomeItemSnapshot *)0)->command_id), 0 },
    {"when_expression", offsetof(UmiUiWelcomeItemSnapshot, when_expression), sizeof(((UmiUiWelcomeItemSnapshot *)0)->when_expression), 0 }
};
static int ContractSnapshotEqual(const UmiUiWelcomeItemSnapshot *left,
    const UmiUiWelcomeItemSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->view_id, right->view_id, sizeof(left->view_id)) == 0 &&
        memcmp(left->title, right->title, sizeof(left->title)) == 0 &&
        memcmp(left->description, right->description, sizeof(left->description)) == 0 &&
        memcmp(left->command_id, right->command_id, sizeof(left->command_id)) == 0 &&
        memcmp(left->when_expression, right->when_expression, sizeof(left->when_expression)) == 0 &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiWelcomeItemSnapshot *item)
{
    item->view_id[0] = 'v';
    item->title[0] = 'v';
    item->description[0] = 'v';
    item->command_id[0] = 'v';
    item->when_expression[0] = 'v';
    item->order = (int32_t)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_welcome_view_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_welcome_view_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiWelcomeItemEdit
#define CONTRACT_EDIT_CURRENT umi_ui_welcome_view_registry_edit_if_current
#include "snapshot_contract_cases.h"
