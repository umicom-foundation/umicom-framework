/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_editor_symbol.c
 * PURPOSE: Check editor symbol input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/symbol.h"
#include "umicom/editor/symbol.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiEditorSymbolSnapshot
#define CONTRACT_REGISTRY UmiEditorSymbolRegistry
#define CONTRACT_CAPACITY UMI_EDITOR_SYMBOL_CAPACITY
#define CONTRACT_VALIDATE umi_editor_symbol_snapshot_validate
#define CONTRACT_REPLACE_DOCUMENT umi_editor_symbol_registry_replace_document
#define CONTRACT_BATCH umi_editor_symbol_registry_upsert_many
#define CONTRACT_CREATE umi_editor_symbol_registry_create
#define CONTRACT_DESTROY umi_editor_symbol_registry_destroy
#define CONTRACT_UPSERT umi_editor_symbol_registry_upsert
#define CONTRACT_REMOVE umi_editor_symbol_registry_remove
#define CONTRACT_FIND umi_editor_symbol_registry_find
#define CONTRACT_AT umi_editor_symbol_registry_at
#define CONTRACT_COUNT umi_editor_symbol_registry_count
#define CONTRACT_REVISION umi_editor_symbol_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiEditorSymbolSnapshot, id), sizeof(((UmiEditorSymbolSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiEditorSymbolSnapshot, document_id), sizeof(((UmiEditorSymbolSnapshot *)0)->document_id), 0 },
    {"parent_id", offsetof(UmiEditorSymbolSnapshot, parent_id), sizeof(((UmiEditorSymbolSnapshot *)0)->parent_id), 0 },
    {"name", offsetof(UmiEditorSymbolSnapshot, name), sizeof(((UmiEditorSymbolSnapshot *)0)->name), 0 },
    {"kind", offsetof(UmiEditorSymbolSnapshot, kind), sizeof(((UmiEditorSymbolSnapshot *)0)->kind), 0 },
    {"detail", offsetof(UmiEditorSymbolSnapshot, detail), sizeof(((UmiEditorSymbolSnapshot *)0)->detail), 0 }
};
static int ContractSnapshotEqual(const UmiEditorSymbolSnapshot *left, const UmiEditorSymbolSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->parent_id, right->parent_id, sizeof(left->parent_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->detail, right->detail, sizeof(left->detail)) == 0 &&
        left->line == right->line &&
        left->column == right->column &&
        left->end_line == right->end_line &&
        left->end_column == right->end_column &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiEditorSymbolSnapshot *item)
{
    item->document_id[0] = 'v';
    item->parent_id[0] = 'v';
    item->name[0] = 'v';
    item->kind[0] = 'v';
    item->detail[0] = 'v';
    item->line = (uint64_t)9U;
    item->column = (uint64_t)10U;
    item->end_line = (uint64_t)11U;
    item->end_column = (uint64_t)12U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_editor_symbol_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_editor_symbol_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiEditorSymbolEdit
#define CONTRACT_EDIT_CURRENT umi_editor_symbol_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_editor_symbol_registry_read_page
#include "snapshot_contract_cases.h"
