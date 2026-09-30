/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_property_inspector.c
 * PURPOSE: Exercise ui property_inspector snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/property_inspector.h"
#include "umicom/ui/property_inspector.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiInspectorPropertySnapshot
#define CONTRACT_REGISTRY UmiUiInspectorPropertyRegistry
#define CONTRACT_CAPACITY UMI_UI_PROPERTY_INSPECTOR_CAPACITY
#define CONTRACT_VALIDATE umi_ui_property_inspector_snapshot_validate
#define CONTRACT_BATCH umi_ui_property_inspector_registry_upsert_many
#define CONTRACT_CREATE umi_ui_property_inspector_registry_create
#define CONTRACT_DESTROY umi_ui_property_inspector_registry_destroy
#define CONTRACT_UPSERT umi_ui_property_inspector_registry_upsert
#define CONTRACT_REMOVE umi_ui_property_inspector_registry_remove
#define CONTRACT_FIND umi_ui_property_inspector_registry_find
#define CONTRACT_AT umi_ui_property_inspector_registry_at
#define CONTRACT_COUNT umi_ui_property_inspector_registry_count
#define CONTRACT_REVISION umi_ui_property_inspector_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiInspectorPropertySnapshot, id), sizeof(((UmiUiInspectorPropertySnapshot *)0)->id), 1 },
    {"object_id", offsetof(UmiUiInspectorPropertySnapshot, object_id), sizeof(((UmiUiInspectorPropertySnapshot *)0)->object_id), 0 },
    {"category", offsetof(UmiUiInspectorPropertySnapshot, category), sizeof(((UmiUiInspectorPropertySnapshot *)0)->category), 0 },
    {"name", offsetof(UmiUiInspectorPropertySnapshot, name), sizeof(((UmiUiInspectorPropertySnapshot *)0)->name), 0 },
    {"value", offsetof(UmiUiInspectorPropertySnapshot, value), sizeof(((UmiUiInspectorPropertySnapshot *)0)->value), 0 },
    {"value_type", offsetof(UmiUiInspectorPropertySnapshot, value_type), sizeof(((UmiUiInspectorPropertySnapshot *)0)->value_type), 0 }
};
static int ContractSnapshotEqual(const UmiUiInspectorPropertySnapshot *left,
    const UmiUiInspectorPropertySnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->object_id, right->object_id, sizeof(left->object_id)) == 0 &&
        memcmp(left->category, right->category, sizeof(left->category)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->value, right->value, sizeof(left->value)) == 0 &&
        memcmp(left->value_type, right->value_type, sizeof(left->value_type)) == 0 &&
        left->editable == right->editable &&
        left->required == right->required &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiInspectorPropertySnapshot *item)
{
    item->object_id[0] = 'v';
    item->category[0] = 'v';
    item->name[0] = 'v';
    item->value[0] = 'v';
    item->value_type[0] = 'v';
    item->editable = (int)9U;
    item->required = (int)10U;
    item->order = (int32_t)11U;
}
#include "snapshot_contract_cases.h"
