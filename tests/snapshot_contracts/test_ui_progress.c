/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_progress.c
 * PURPOSE: Exercise ui progress snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/progress.h"
#include "umicom/ui/progress.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiProgressSnapshot
#define CONTRACT_REGISTRY UmiUiProgressRegistry
#define CONTRACT_CAPACITY UMI_UI_PROGRESS_CAPACITY
#define CONTRACT_VALIDATE umi_ui_progress_snapshot_validate
#define CONTRACT_BATCH umi_ui_progress_registry_upsert_many
#define CONTRACT_CREATE umi_ui_progress_registry_create
#define CONTRACT_DESTROY umi_ui_progress_registry_destroy
#define CONTRACT_UPSERT umi_ui_progress_registry_upsert
#define CONTRACT_REMOVE umi_ui_progress_registry_remove
#define CONTRACT_FIND umi_ui_progress_registry_find
#define CONTRACT_AT umi_ui_progress_registry_at
#define CONTRACT_COUNT umi_ui_progress_registry_count
#define CONTRACT_REVISION umi_ui_progress_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiProgressSnapshot, id), sizeof(((UmiUiProgressSnapshot *)0)->id), 1 },
    {"title", offsetof(UmiUiProgressSnapshot, title), sizeof(((UmiUiProgressSnapshot *)0)->title), 0 },
    {"detail", offsetof(UmiUiProgressSnapshot, detail), sizeof(((UmiUiProgressSnapshot *)0)->detail), 0 }
};
static int ContractSnapshotEqual(const UmiUiProgressSnapshot *left,
    const UmiUiProgressSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->title, right->title, sizeof(left->title)) == 0 &&
        memcmp(left->detail, right->detail, sizeof(left->detail)) == 0 &&
        left->fraction == right->fraction &&
        left->state == right->state &&
        left->cancellable == right->cancellable &&
        left->indeterminate == right->indeterminate &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiProgressSnapshot *item)
{
    item->title[0] = 'v';
    item->detail[0] = 'v';
    item->fraction = (double)6U;
    item->state = (int)7U;
    item->cancellable = (int)8U;
    item->indeterminate = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_progress_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_progress_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiProgressEdit
#define CONTRACT_EDIT_CURRENT umi_ui_progress_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_ui_progress_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiUiProgressSnapshot ArchiveSample(void)
{
    UmiUiProgressSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiUiProgressSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->title) + 1U;
        memset(value->title + used, 0xa5, sizeof(value->title) - used);
    }
    {
        size_t used = strlen(value->detail) + 1U;
        memset(value->detail + used, 0xa5, sizeof(value->detail) - used);
    }
}
#define ARCHIVE_TYPE UmiUiProgressSnapshot
#define ARCHIVE_ENCODE umi_ui_progress_snapshot_archive_encode
#define ARCHIVE_DECODE umi_ui_progress_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_ui_progress_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_ui_progress_registry_archive_restore
#include "snapshot_contract_cases.h"
