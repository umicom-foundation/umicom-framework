/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_document.c
 * PURPOSE: Check language document input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/document.h"
#include "umicom/language/document.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageDocumentSnapshot
#define CONTRACT_REGISTRY UmiLanguageDocumentRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_DOCUMENT_CAPACITY
#define CONTRACT_VALIDATE umi_language_document_snapshot_validate
#define CONTRACT_BATCH umi_language_document_registry_upsert_many
#define CONTRACT_CREATE umi_language_document_registry_create
#define CONTRACT_DESTROY umi_language_document_registry_destroy
#define CONTRACT_UPSERT umi_language_document_registry_upsert
#define CONTRACT_REMOVE umi_language_document_registry_remove
#define CONTRACT_FIND umi_language_document_registry_find
#define CONTRACT_AT umi_language_document_registry_at
#define CONTRACT_COUNT umi_language_document_registry_count
#define CONTRACT_REVISION umi_language_document_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageDocumentSnapshot, id), sizeof(((UmiLanguageDocumentSnapshot *)0)->id), 1 },
    {"uri", offsetof(UmiLanguageDocumentSnapshot, uri), sizeof(((UmiLanguageDocumentSnapshot *)0)->uri), 0 },
    {"language_id", offsetof(UmiLanguageDocumentSnapshot, language_id), sizeof(((UmiLanguageDocumentSnapshot *)0)->language_id), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageDocumentSnapshot *left, const UmiLanguageDocumentSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->uri, right->uri, sizeof(left->uri)) == 0 &&
        memcmp(left->language_id, right->language_id, sizeof(left->language_id)) == 0 &&
        left->version == right->version &&
        left->line_count == right->line_count &&
        left->open == right->open &&
        left->dirty == right->dirty &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageDocumentSnapshot *item)
{
    item->uri[0] = 'v';
    item->language_id[0] = 'v';
    item->version = (uint64_t)6U;
    item->line_count = (size_t)7U;
    item->open = (int)8U;
    item->dirty = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_language_document_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_language_document_registry_replace_if_current
#include "snapshot_contract_cases.h"
