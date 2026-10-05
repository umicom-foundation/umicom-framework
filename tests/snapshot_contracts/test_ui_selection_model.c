/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_selection_model.c
 * PURPOSE: Exercise ui selection_model snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/selection_model.h"
#include "umicom/ui/selection_model.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiSelectionModelSnapshot
#define CONTRACT_REGISTRY UmiUiSelectionModelRegistry
#define CONTRACT_CAPACITY UMI_UI_SELECTION_MODEL_CAPACITY
#define CONTRACT_VALIDATE umi_ui_selection_model_snapshot_validate
#define CONTRACT_BATCH umi_ui_selection_model_registry_upsert_many
#define CONTRACT_CREATE umi_ui_selection_model_registry_create
#define CONTRACT_DESTROY umi_ui_selection_model_registry_destroy
#define CONTRACT_UPSERT umi_ui_selection_model_registry_upsert
#define CONTRACT_REMOVE umi_ui_selection_model_registry_remove
#define CONTRACT_FIND umi_ui_selection_model_registry_find
#define CONTRACT_AT umi_ui_selection_model_registry_at
#define CONTRACT_COUNT umi_ui_selection_model_registry_count
#define CONTRACT_REVISION umi_ui_selection_model_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiSelectionModelSnapshot, id), sizeof(((UmiUiSelectionModelSnapshot *)0)->id), 1 }
};
static int ContractSnapshotEqual(const UmiUiSelectionModelSnapshot *left,
    const UmiUiSelectionModelSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        left->selected == right->selected &&
        left->focused == right->focused &&
        left->anchor == right->anchor &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiSelectionModelSnapshot *item)
{
    item->selected = (int)4U;
    item->focused = (int)5U;
    item->anchor = (int)6U;
    item->order = (int32_t)7U;
}
/* Keep the registry identity stable while proving scalar replacement. */
static void ContractChangeSelectionPayload(UmiUiSelectionModelSnapshot *item)
{
    item->selected = 9;
}
#define CONTRACT_CHANGE_PAYLOAD ContractChangeSelectionPayload
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_selection_model_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_selection_model_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiSelectionModelEdit
#define CONTRACT_EDIT_CURRENT umi_ui_selection_model_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_ui_selection_model_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiUiSelectionModelSnapshot ArchiveSample(void)
{
    UmiUiSelectionModelSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiUiSelectionModelSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
}
#define ARCHIVE_TYPE UmiUiSelectionModelSnapshot
#define ARCHIVE_ENCODE umi_ui_selection_model_snapshot_archive_encode
#define ARCHIVE_DECODE umi_ui_selection_model_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_ui_selection_model_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_ui_selection_model_registry_archive_restore
#include "snapshot_contract_cases.h"
