/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_folding_range.c
 * PURPOSE: Check language folding_range input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/folding_range.h"
#include "umicom/language/folding_range.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageFoldingRangeSnapshot
#define CONTRACT_REGISTRY UmiLanguageFoldingRangeRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_FOLDING_RANGE_CAPACITY
#define CONTRACT_VALIDATE umi_language_folding_range_snapshot_validate
#define CONTRACT_REPLACE_DOCUMENT umi_language_folding_range_registry_replace_document
#define CONTRACT_BATCH umi_language_folding_range_registry_upsert_many
#define CONTRACT_CREATE umi_language_folding_range_registry_create
#define CONTRACT_DESTROY umi_language_folding_range_registry_destroy
#define CONTRACT_UPSERT umi_language_folding_range_registry_upsert
#define CONTRACT_REMOVE umi_language_folding_range_registry_remove
#define CONTRACT_FIND umi_language_folding_range_registry_find
#define CONTRACT_AT umi_language_folding_range_registry_at
#define CONTRACT_COUNT umi_language_folding_range_registry_count
#define CONTRACT_REVISION umi_language_folding_range_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageFoldingRangeSnapshot, id), sizeof(((UmiLanguageFoldingRangeSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiLanguageFoldingRangeSnapshot, document_id), sizeof(((UmiLanguageFoldingRangeSnapshot *)0)->document_id), 0 },
    {"kind", offsetof(UmiLanguageFoldingRangeSnapshot, kind), sizeof(((UmiLanguageFoldingRangeSnapshot *)0)->kind), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageFoldingRangeSnapshot *left, const UmiLanguageFoldingRangeSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        left->start_line == right->start_line &&
        left->end_line == right->end_line &&
        left->collapsed == right->collapsed &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageFoldingRangeSnapshot *item)
{
    item->document_id[0] = 'v';
    item->kind[0] = 'v';
    item->start_line = (uint32_t)6U;
    item->end_line = (uint32_t)7U;
    item->collapsed = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_language_folding_range_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_language_folding_range_registry_replace_if_current
#include "snapshot_contract_cases.h"
