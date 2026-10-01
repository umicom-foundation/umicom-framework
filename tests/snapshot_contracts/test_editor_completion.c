/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_editor_completion.c
 * PURPOSE: Check editor completion input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/completion.h"
#include "umicom/editor/completion.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiEditorCompletionSnapshot
#define CONTRACT_REGISTRY UmiEditorCompletionRegistry
#define CONTRACT_CAPACITY UMI_EDITOR_COMPLETION_CAPACITY
#define CONTRACT_VALIDATE umi_editor_completion_snapshot_validate
#define CONTRACT_REPLACE_DOCUMENT umi_editor_completion_registry_replace_document
#define CONTRACT_BATCH umi_editor_completion_registry_upsert_many
#define CONTRACT_CREATE umi_editor_completion_registry_create
#define CONTRACT_DESTROY umi_editor_completion_registry_destroy
#define CONTRACT_UPSERT umi_editor_completion_registry_upsert
#define CONTRACT_REMOVE umi_editor_completion_registry_remove
#define CONTRACT_FIND umi_editor_completion_registry_find
#define CONTRACT_AT umi_editor_completion_registry_at
#define CONTRACT_COUNT umi_editor_completion_registry_count
#define CONTRACT_REVISION umi_editor_completion_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiEditorCompletionSnapshot, id), sizeof(((UmiEditorCompletionSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiEditorCompletionSnapshot, document_id), sizeof(((UmiEditorCompletionSnapshot *)0)->document_id), 0 },
    {"label", offsetof(UmiEditorCompletionSnapshot, label), sizeof(((UmiEditorCompletionSnapshot *)0)->label), 0 },
    {"detail", offsetof(UmiEditorCompletionSnapshot, detail), sizeof(((UmiEditorCompletionSnapshot *)0)->detail), 0 },
    {"insert_text", offsetof(UmiEditorCompletionSnapshot, insert_text), sizeof(((UmiEditorCompletionSnapshot *)0)->insert_text), 0 },
    {"kind", offsetof(UmiEditorCompletionSnapshot, kind), sizeof(((UmiEditorCompletionSnapshot *)0)->kind), 0 },
    {"sort_text", offsetof(UmiEditorCompletionSnapshot, sort_text), sizeof(((UmiEditorCompletionSnapshot *)0)->sort_text), 0 },
    {"filter_text", offsetof(UmiEditorCompletionSnapshot, filter_text), sizeof(((UmiEditorCompletionSnapshot *)0)->filter_text), 0 }
};
static int ContractSnapshotEqual(const UmiEditorCompletionSnapshot *left, const UmiEditorCompletionSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->label, right->label, sizeof(left->label)) == 0 &&
        memcmp(left->detail, right->detail, sizeof(left->detail)) == 0 &&
        memcmp(left->insert_text, right->insert_text, sizeof(left->insert_text)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->sort_text, right->sort_text, sizeof(left->sort_text)) == 0 &&
        memcmp(left->filter_text, right->filter_text, sizeof(left->filter_text)) == 0 &&
        left->deprecated == right->deprecated &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiEditorCompletionSnapshot *item)
{
    item->document_id[0] = 'v';
    item->label[0] = 'v';
    item->detail[0] = 'v';
    item->insert_text[0] = 'v';
    item->kind[0] = 'v';
    item->sort_text[0] = 'v';
    item->filter_text[0] = 'v';
    item->deprecated = (int)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_editor_completion_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_editor_completion_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiEditorCompletionEdit
#define CONTRACT_EDIT_CURRENT umi_editor_completion_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_editor_completion_registry_read_page
#include "snapshot_contract_cases.h"
