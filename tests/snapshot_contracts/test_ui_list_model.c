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
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_ui_list_model_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiUiListModelSnapshot ArchiveSample(void)
{
    UmiUiListModelSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiUiListModelSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
    {
        size_t used = strlen(value->description) + 1U;
        memset(value->description + used, 0xa5, sizeof(value->description) - used);
    }
    {
        size_t used = strlen(value->icon_name) + 1U;
        memset(value->icon_name + used, 0xa5, sizeof(value->icon_name) - used);
    }
}
#define ARCHIVE_TYPE UmiUiListModelSnapshot
#define ARCHIVE_ENCODE umi_ui_list_model_snapshot_archive_encode
#define ARCHIVE_DECODE umi_ui_list_model_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_ui_list_model_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_ui_list_model_registry_archive_restore
#include "snapshot_contract_cases.h"
