/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_hover.c
 * PURPOSE: Check language hover input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/hover.h"
#include "umicom/language/hover.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageHoverSnapshot
#define CONTRACT_REGISTRY UmiLanguageHoverRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_HOVER_CAPACITY
#define CONTRACT_VALIDATE umi_language_hover_snapshot_validate
#define CONTRACT_REPLACE_DOCUMENT umi_language_hover_registry_replace_document
#define CONTRACT_BATCH umi_language_hover_registry_upsert_many
#define CONTRACT_CREATE umi_language_hover_registry_create
#define CONTRACT_DESTROY umi_language_hover_registry_destroy
#define CONTRACT_UPSERT umi_language_hover_registry_upsert
#define CONTRACT_REMOVE umi_language_hover_registry_remove
#define CONTRACT_FIND umi_language_hover_registry_find
#define CONTRACT_AT umi_language_hover_registry_at
#define CONTRACT_COUNT umi_language_hover_registry_count
#define CONTRACT_REVISION umi_language_hover_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageHoverSnapshot, id), sizeof(((UmiLanguageHoverSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiLanguageHoverSnapshot, document_id), sizeof(((UmiLanguageHoverSnapshot *)0)->document_id), 0 },
    {"contents", offsetof(UmiLanguageHoverSnapshot, contents), sizeof(((UmiLanguageHoverSnapshot *)0)->contents), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageHoverSnapshot *left, const UmiLanguageHoverSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->contents, right->contents, sizeof(left->contents)) == 0 &&
        left->line == right->line &&
        left->column == right->column &&
        left->start_line == right->start_line &&
        left->start_column == right->start_column &&
        left->end_line == right->end_line &&
        left->end_column == right->end_column &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageHoverSnapshot *item)
{
    item->document_id[0] = 'v';
    item->contents[0] = 'v';
    item->line = (uint32_t)6U;
    item->column = (uint32_t)7U;
    item->start_line = (uint32_t)8U;
    item->start_column = (uint32_t)9U;
    item->end_line = (uint32_t)10U;
    item->end_column = (uint32_t)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_language_hover_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_language_hover_registry_replace_if_current
#include "snapshot_contract_cases.h"
