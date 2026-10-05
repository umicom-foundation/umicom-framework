/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_editor_diff_hunk.c
 * PURPOSE: Check editor diff_hunk input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/diff_hunk.h"
#include "umicom/editor/diff_hunk.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiEditorDiffHunkSnapshot
#define CONTRACT_REGISTRY UmiEditorDiffHunkRegistry
#define CONTRACT_CAPACITY UMI_EDITOR_DIFF_HUNK_CAPACITY
#define CONTRACT_VALIDATE umi_editor_diff_hunk_snapshot_validate
#define CONTRACT_BATCH umi_editor_diff_hunk_registry_upsert_many
#define CONTRACT_CREATE umi_editor_diff_hunk_registry_create
#define CONTRACT_DESTROY umi_editor_diff_hunk_registry_destroy
#define CONTRACT_UPSERT umi_editor_diff_hunk_registry_upsert
#define CONTRACT_REMOVE umi_editor_diff_hunk_registry_remove
#define CONTRACT_FIND umi_editor_diff_hunk_registry_find
#define CONTRACT_AT umi_editor_diff_hunk_registry_at
#define CONTRACT_COUNT umi_editor_diff_hunk_registry_count
#define CONTRACT_REVISION umi_editor_diff_hunk_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiEditorDiffHunkSnapshot, id), sizeof(((UmiEditorDiffHunkSnapshot *)0)->id), 1 },
    {"left_uri", offsetof(UmiEditorDiffHunkSnapshot, left_uri), sizeof(((UmiEditorDiffHunkSnapshot *)0)->left_uri), 0 },
    {"right_uri", offsetof(UmiEditorDiffHunkSnapshot, right_uri), sizeof(((UmiEditorDiffHunkSnapshot *)0)->right_uri), 0 }
};
static int ContractSnapshotEqual(const UmiEditorDiffHunkSnapshot *left, const UmiEditorDiffHunkSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->left_uri, right->left_uri, sizeof(left->left_uri)) == 0 &&
        memcmp(left->right_uri, right->right_uri, sizeof(left->right_uri)) == 0 &&
        left->old_start == right->old_start &&
        left->old_count == right->old_count &&
        left->new_start == right->new_start &&
        left->new_count == right->new_count &&
        left->state == right->state &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiEditorDiffHunkSnapshot *item)
{
    item->left_uri[0] = 'v';
    item->right_uri[0] = 'v';
    item->old_start = (uint64_t)6U;
    item->old_count = (uint64_t)7U;
    item->new_start = (uint64_t)8U;
    item->new_count = (uint64_t)9U;
    item->state = (int)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_editor_diff_hunk_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_editor_diff_hunk_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiEditorDiffHunkEdit
#define CONTRACT_EDIT_CURRENT umi_editor_diff_hunk_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_editor_diff_hunk_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiEditorDiffHunkSnapshot ArchiveSample(void)
{
    UmiEditorDiffHunkSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiEditorDiffHunkSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->left_uri) + 1U;
        memset(value->left_uri + used, 0xa5, sizeof(value->left_uri) - used);
    }
    {
        size_t used = strlen(value->right_uri) + 1U;
        memset(value->right_uri + used, 0xa5, sizeof(value->right_uri) - used);
    }
}
#define ARCHIVE_TYPE UmiEditorDiffHunkSnapshot
#define ARCHIVE_ENCODE umi_editor_diff_hunk_snapshot_archive_encode
#define ARCHIVE_DECODE umi_editor_diff_hunk_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_editor_diff_hunk_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_editor_diff_hunk_registry_archive_restore
#include "snapshot_contract_cases.h"
