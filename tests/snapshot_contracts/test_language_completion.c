/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_completion.c
 * PURPOSE: Check language completion input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/completion.h"
#include "umicom/language/completion.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageCompletionSnapshot
#define CONTRACT_REGISTRY UmiLanguageCompletionRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_COMPLETION_CAPACITY
#define CONTRACT_VALIDATE umi_language_completion_snapshot_validate
#define CONTRACT_REPLACE_DOCUMENT umi_language_completion_registry_replace_document
#define CONTRACT_BATCH umi_language_completion_registry_upsert_many
#define CONTRACT_CREATE umi_language_completion_registry_create
#define CONTRACT_DESTROY umi_language_completion_registry_destroy
#define CONTRACT_UPSERT umi_language_completion_registry_upsert
#define CONTRACT_REMOVE umi_language_completion_registry_remove
#define CONTRACT_FIND umi_language_completion_registry_find
#define CONTRACT_AT umi_language_completion_registry_at
#define CONTRACT_COUNT umi_language_completion_registry_count
#define CONTRACT_REVISION umi_language_completion_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageCompletionSnapshot, id), sizeof(((UmiLanguageCompletionSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiLanguageCompletionSnapshot, document_id), sizeof(((UmiLanguageCompletionSnapshot *)0)->document_id), 0 },
    {"label", offsetof(UmiLanguageCompletionSnapshot, label), sizeof(((UmiLanguageCompletionSnapshot *)0)->label), 0 },
    {"detail", offsetof(UmiLanguageCompletionSnapshot, detail), sizeof(((UmiLanguageCompletionSnapshot *)0)->detail), 0 },
    {"insert_text", offsetof(UmiLanguageCompletionSnapshot, insert_text), sizeof(((UmiLanguageCompletionSnapshot *)0)->insert_text), 0 },
    {"kind", offsetof(UmiLanguageCompletionSnapshot, kind), sizeof(((UmiLanguageCompletionSnapshot *)0)->kind), 0 },
    {"sort_text", offsetof(UmiLanguageCompletionSnapshot, sort_text), sizeof(((UmiLanguageCompletionSnapshot *)0)->sort_text), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageCompletionSnapshot *left, const UmiLanguageCompletionSnapshot *right)
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
        left->line == right->line &&
        left->column == right->column &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageCompletionSnapshot *item)
{
    item->document_id[0] = 'v';
    item->label[0] = 'v';
    item->detail[0] = 'v';
    item->insert_text[0] = 'v';
    item->kind[0] = 'v';
    item->sort_text[0] = 'v';
    item->line = (uint32_t)10U;
    item->column = (uint32_t)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_language_completion_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_language_completion_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiLanguageCompletionEdit
#define CONTRACT_EDIT_CURRENT umi_language_completion_registry_edit_if_current
#include "snapshot_contract_cases.h"
