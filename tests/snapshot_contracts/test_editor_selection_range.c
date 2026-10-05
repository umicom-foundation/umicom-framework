/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_editor_selection_range.c
 * PURPOSE: Check editor selection_range input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/selection_range.h"
#include "umicom/editor/selection_range.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiEditorSelectionRangeSnapshot
#define CONTRACT_REGISTRY UmiEditorSelectionRangeRegistry
#define CONTRACT_CAPACITY UMI_EDITOR_SELECTION_RANGE_CAPACITY
#define CONTRACT_VALIDATE umi_editor_selection_range_snapshot_validate
#define CONTRACT_BATCH umi_editor_selection_range_registry_upsert_many
#define CONTRACT_CREATE umi_editor_selection_range_registry_create
#define CONTRACT_DESTROY umi_editor_selection_range_registry_destroy
#define CONTRACT_UPSERT umi_editor_selection_range_registry_upsert
#define CONTRACT_REMOVE umi_editor_selection_range_registry_remove
#define CONTRACT_FIND umi_editor_selection_range_registry_find
#define CONTRACT_AT umi_editor_selection_range_registry_at
#define CONTRACT_COUNT umi_editor_selection_range_registry_count
#define CONTRACT_REVISION umi_editor_selection_range_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiEditorSelectionRangeSnapshot, id), sizeof(((UmiEditorSelectionRangeSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiEditorSelectionRangeSnapshot, document_id), sizeof(((UmiEditorSelectionRangeSnapshot *)0)->document_id), 0 }
};
static int ContractSnapshotEqual(const UmiEditorSelectionRangeSnapshot *left, const UmiEditorSelectionRangeSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        left->anchor_line == right->anchor_line &&
        left->anchor_column == right->anchor_column &&
        left->active_line == right->active_line &&
        left->active_column == right->active_column &&
        left->rectangular == right->rectangular &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiEditorSelectionRangeSnapshot *item)
{
    item->document_id[0] = 'v';
    item->anchor_line = (uint64_t)5U;
    item->anchor_column = (uint64_t)6U;
    item->active_line = (uint64_t)7U;
    item->active_column = (uint64_t)8U;
    item->rectangular = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_editor_selection_range_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_editor_selection_range_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiEditorSelectionRangeEdit
#define CONTRACT_EDIT_CURRENT umi_editor_selection_range_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_editor_selection_range_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiEditorSelectionRangeSnapshot ArchiveSample(void)
{
    UmiEditorSelectionRangeSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiEditorSelectionRangeSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->document_id) + 1U;
        memset(value->document_id + used, 0xa5, sizeof(value->document_id) - used);
    }
}
#define ARCHIVE_TYPE UmiEditorSelectionRangeSnapshot
#define ARCHIVE_ENCODE umi_editor_selection_range_snapshot_archive_encode
#define ARCHIVE_DECODE umi_editor_selection_range_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_editor_selection_range_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_editor_selection_range_registry_archive_restore
#include "snapshot_contract_cases.h"
