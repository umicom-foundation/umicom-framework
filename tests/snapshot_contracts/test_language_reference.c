/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_reference.c
 * PURPOSE: Check language reference input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/reference.h"
#include "umicom/language/reference.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageReferenceSnapshot
#define CONTRACT_REGISTRY UmiLanguageReferenceRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_REFERENCE_CAPACITY
#define CONTRACT_VALIDATE umi_language_reference_snapshot_validate
#define CONTRACT_REPLACE_DOCUMENT umi_language_reference_registry_replace_document
#define CONTRACT_BATCH umi_language_reference_registry_upsert_many
#define CONTRACT_CREATE umi_language_reference_registry_create
#define CONTRACT_DESTROY umi_language_reference_registry_destroy
#define CONTRACT_UPSERT umi_language_reference_registry_upsert
#define CONTRACT_REMOVE umi_language_reference_registry_remove
#define CONTRACT_FIND umi_language_reference_registry_find
#define CONTRACT_AT umi_language_reference_registry_at
#define CONTRACT_COUNT umi_language_reference_registry_count
#define CONTRACT_REVISION umi_language_reference_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageReferenceSnapshot, id), sizeof(((UmiLanguageReferenceSnapshot *)0)->id), 1 },
    {"symbol_id", offsetof(UmiLanguageReferenceSnapshot, symbol_id), sizeof(((UmiLanguageReferenceSnapshot *)0)->symbol_id), 0 },
    {"document_id", offsetof(UmiLanguageReferenceSnapshot, document_id), sizeof(((UmiLanguageReferenceSnapshot *)0)->document_id), 0 },
    {"uri", offsetof(UmiLanguageReferenceSnapshot, uri), sizeof(((UmiLanguageReferenceSnapshot *)0)->uri), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageReferenceSnapshot *left, const UmiLanguageReferenceSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->symbol_id, right->symbol_id, sizeof(left->symbol_id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->uri, right->uri, sizeof(left->uri)) == 0 &&
        left->line == right->line &&
        left->column == right->column &&
        left->definition == right->definition &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageReferenceSnapshot *item)
{
    item->symbol_id[0] = 'v';
    item->document_id[0] = 'v';
    item->uri[0] = 'v';
    item->line = (uint32_t)7U;
    item->column = (uint32_t)8U;
    item->definition = (int)9U;
}
#include "snapshot_contract_cases.h"
