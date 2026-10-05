/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_semantic_token.c
 * PURPOSE: Check language semantic_token input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/semantic_token.h"
#include "umicom/language/semantic_token.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageSemanticTokenSnapshot
#define CONTRACT_REGISTRY UmiLanguageSemanticTokenRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_SEMANTIC_TOKEN_CAPACITY
#define CONTRACT_VALIDATE umi_language_semantic_token_snapshot_validate
#define CONTRACT_REPLACE_DOCUMENT umi_language_semantic_token_registry_replace_document
#define CONTRACT_BATCH umi_language_semantic_token_registry_upsert_many
#define CONTRACT_CREATE umi_language_semantic_token_registry_create
#define CONTRACT_DESTROY umi_language_semantic_token_registry_destroy
#define CONTRACT_UPSERT umi_language_semantic_token_registry_upsert
#define CONTRACT_REMOVE umi_language_semantic_token_registry_remove
#define CONTRACT_FIND umi_language_semantic_token_registry_find
#define CONTRACT_AT umi_language_semantic_token_registry_at
#define CONTRACT_COUNT umi_language_semantic_token_registry_count
#define CONTRACT_REVISION umi_language_semantic_token_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageSemanticTokenSnapshot, id), sizeof(((UmiLanguageSemanticTokenSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiLanguageSemanticTokenSnapshot, document_id), sizeof(((UmiLanguageSemanticTokenSnapshot *)0)->document_id), 0 },
    {"token_type", offsetof(UmiLanguageSemanticTokenSnapshot, token_type), sizeof(((UmiLanguageSemanticTokenSnapshot *)0)->token_type), 0 },
    {"modifiers", offsetof(UmiLanguageSemanticTokenSnapshot, modifiers), sizeof(((UmiLanguageSemanticTokenSnapshot *)0)->modifiers), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageSemanticTokenSnapshot *left, const UmiLanguageSemanticTokenSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->token_type, right->token_type, sizeof(left->token_type)) == 0 &&
        memcmp(left->modifiers, right->modifiers, sizeof(left->modifiers)) == 0 &&
        left->line == right->line &&
        left->column == right->column &&
        left->length == right->length &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageSemanticTokenSnapshot *item)
{
    item->document_id[0] = 'v';
    item->token_type[0] = 'v';
    item->modifiers[0] = 'v';
    item->line = (uint32_t)7U;
    item->column = (uint32_t)8U;
    item->length = (uint32_t)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_language_semantic_token_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_language_semantic_token_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiLanguageSemanticTokenEdit
#define CONTRACT_EDIT_CURRENT umi_language_semantic_token_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_language_semantic_token_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiLanguageSemanticTokenSnapshot ArchiveSample(void)
{
    UmiLanguageSemanticTokenSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiLanguageSemanticTokenSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->document_id) + 1U;
        memset(value->document_id + used, 0xa5, sizeof(value->document_id) - used);
    }
    {
        size_t used = strlen(value->token_type) + 1U;
        memset(value->token_type + used, 0xa5, sizeof(value->token_type) - used);
    }
    {
        size_t used = strlen(value->modifiers) + 1U;
        memset(value->modifiers + used, 0xa5, sizeof(value->modifiers) - used);
    }
}
#define ARCHIVE_TYPE UmiLanguageSemanticTokenSnapshot
#define ARCHIVE_ENCODE umi_language_semantic_token_snapshot_archive_encode
#define ARCHIVE_DECODE umi_language_semantic_token_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_language_semantic_token_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_language_semantic_token_registry_archive_restore
#include "snapshot_contract_cases.h"
