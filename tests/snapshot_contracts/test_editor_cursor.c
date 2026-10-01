/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_editor_cursor.c
 * PURPOSE: Check editor cursor input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/cursor.h"
#include "umicom/editor/cursor.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiEditorCursorSnapshot
#define CONTRACT_REGISTRY UmiEditorCursorRegistry
#define CONTRACT_CAPACITY UMI_EDITOR_CURSOR_CAPACITY
#define CONTRACT_VALIDATE umi_editor_cursor_snapshot_validate
#define CONTRACT_BATCH umi_editor_cursor_registry_upsert_many
#define CONTRACT_CREATE umi_editor_cursor_registry_create
#define CONTRACT_DESTROY umi_editor_cursor_registry_destroy
#define CONTRACT_UPSERT umi_editor_cursor_registry_upsert
#define CONTRACT_REMOVE umi_editor_cursor_registry_remove
#define CONTRACT_FIND umi_editor_cursor_registry_find
#define CONTRACT_AT umi_editor_cursor_registry_at
#define CONTRACT_COUNT umi_editor_cursor_registry_count
#define CONTRACT_REVISION umi_editor_cursor_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiEditorCursorSnapshot, id), sizeof(((UmiEditorCursorSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiEditorCursorSnapshot, document_id), sizeof(((UmiEditorCursorSnapshot *)0)->document_id), 0 }
};
static int ContractSnapshotEqual(const UmiEditorCursorSnapshot *left, const UmiEditorCursorSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        left->line == right->line &&
        left->column == right->column &&
        left->preferred_column == right->preferred_column &&
        left->primary == right->primary &&
        left->visible == right->visible &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiEditorCursorSnapshot *item)
{
    item->document_id[0] = 'v';
    item->line = (uint64_t)5U;
    item->column = (uint64_t)6U;
    item->preferred_column = (uint64_t)7U;
    item->primary = (int)8U;
    item->visible = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_editor_cursor_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_editor_cursor_registry_replace_if_current
#include "snapshot_contract_cases.h"
