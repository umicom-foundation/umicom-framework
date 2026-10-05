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
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_property_inspector_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_property_inspector_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiInspectorPropertyEdit
#define CONTRACT_EDIT_CURRENT umi_ui_property_inspector_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_ui_property_inspector_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiUiInspectorPropertySnapshot ArchiveSample(void)
{
    UmiUiInspectorPropertySnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiUiInspectorPropertySnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->object_id) + 1U;
        memset(value->object_id + used, 0xa5, sizeof(value->object_id) - used);
    }
    {
        size_t used = strlen(value->category) + 1U;
        memset(value->category + used, 0xa5, sizeof(value->category) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->value) + 1U;
        memset(value->value + used, 0xa5, sizeof(value->value) - used);
    }
    {
        size_t used = strlen(value->value_type) + 1U;
        memset(value->value_type + used, 0xa5, sizeof(value->value_type) - used);
    }
}
#define ARCHIVE_TYPE UmiUiInspectorPropertySnapshot
#define ARCHIVE_ENCODE umi_ui_property_inspector_snapshot_archive_encode
#define ARCHIVE_DECODE umi_ui_property_inspector_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_ui_property_inspector_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_ui_property_inspector_registry_archive_restore
#include "snapshot_contract_cases.h"
