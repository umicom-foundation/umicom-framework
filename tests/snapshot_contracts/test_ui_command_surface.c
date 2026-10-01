/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_command_surface.c
 * PURPOSE: Exercise ui command_surface snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/command_surface.h"
#include "umicom/ui/command_surface.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiCommandSurfaceSnapshot
#define CONTRACT_REGISTRY UmiUiCommandSurfaceRegistry
#define CONTRACT_CAPACITY UMI_UI_COMMAND_SURFACE_CAPACITY
#define CONTRACT_VALIDATE umi_ui_command_surface_snapshot_validate
#define CONTRACT_BATCH umi_ui_command_surface_registry_upsert_many
#define CONTRACT_CREATE umi_ui_command_surface_registry_create
#define CONTRACT_DESTROY umi_ui_command_surface_registry_destroy
#define CONTRACT_UPSERT umi_ui_command_surface_registry_upsert
#define CONTRACT_REMOVE umi_ui_command_surface_registry_remove
#define CONTRACT_FIND umi_ui_command_surface_registry_find
#define CONTRACT_AT umi_ui_command_surface_registry_at
#define CONTRACT_COUNT umi_ui_command_surface_registry_count
#define CONTRACT_REVISION umi_ui_command_surface_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiCommandSurfaceSnapshot, id), sizeof(((UmiUiCommandSurfaceSnapshot *)0)->id), 1 },
    {"command_id", offsetof(UmiUiCommandSurfaceSnapshot, command_id), sizeof(((UmiUiCommandSurfaceSnapshot *)0)->command_id), 0 },
    {"title", offsetof(UmiUiCommandSurfaceSnapshot, title), sizeof(((UmiUiCommandSurfaceSnapshot *)0)->title), 0 },
    {"category", offsetof(UmiUiCommandSurfaceSnapshot, category), sizeof(((UmiUiCommandSurfaceSnapshot *)0)->category), 0 },
    {"icon_name", offsetof(UmiUiCommandSurfaceSnapshot, icon_name), sizeof(((UmiUiCommandSurfaceSnapshot *)0)->icon_name), 0 },
    {"key_hint", offsetof(UmiUiCommandSurfaceSnapshot, key_hint), sizeof(((UmiUiCommandSurfaceSnapshot *)0)->key_hint), 0 },
    {"when_expression", offsetof(UmiUiCommandSurfaceSnapshot, when_expression), sizeof(((UmiUiCommandSurfaceSnapshot *)0)->when_expression), 0 }
};
static int ContractSnapshotEqual(const UmiUiCommandSurfaceSnapshot *left,
    const UmiUiCommandSurfaceSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->command_id, right->command_id, sizeof(left->command_id)) == 0 &&
        memcmp(left->title, right->title, sizeof(left->title)) == 0 &&
        memcmp(left->category, right->category, sizeof(left->category)) == 0 &&
        memcmp(left->icon_name, right->icon_name, sizeof(left->icon_name)) == 0 &&
        memcmp(left->key_hint, right->key_hint, sizeof(left->key_hint)) == 0 &&
        memcmp(left->when_expression, right->when_expression, sizeof(left->when_expression)) == 0 &&
        left->enabled == right->enabled &&
        left->visible == right->visible &&
        left->score == right->score &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiCommandSurfaceSnapshot *item)
{
    item->command_id[0] = 'v';
    item->title[0] = 'v';
    item->category[0] = 'v';
    item->icon_name[0] = 'v';
    item->key_hint[0] = 'v';
    item->when_expression[0] = 'v';
    item->enabled = (int)10U;
    item->visible = (int)11U;
    item->score = (int32_t)12U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_command_surface_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_command_surface_registry_replace_if_current
#include "snapshot_contract_cases.h"
