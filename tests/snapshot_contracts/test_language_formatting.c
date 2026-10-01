/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_formatting.c
 * PURPOSE: Check language formatting input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/formatting.h"
#include "umicom/language/formatting.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageFormattingSnapshot
#define CONTRACT_REGISTRY UmiLanguageFormattingRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_FORMATTING_CAPACITY
#define CONTRACT_VALIDATE umi_language_formatting_snapshot_validate
#define CONTRACT_BATCH umi_language_formatting_registry_upsert_many
#define CONTRACT_CREATE umi_language_formatting_registry_create
#define CONTRACT_DESTROY umi_language_formatting_registry_destroy
#define CONTRACT_UPSERT umi_language_formatting_registry_upsert
#define CONTRACT_REMOVE umi_language_formatting_registry_remove
#define CONTRACT_FIND umi_language_formatting_registry_find
#define CONTRACT_AT umi_language_formatting_registry_at
#define CONTRACT_COUNT umi_language_formatting_registry_count
#define CONTRACT_REVISION umi_language_formatting_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageFormattingSnapshot, id), sizeof(((UmiLanguageFormattingSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiLanguageFormattingSnapshot, document_id), sizeof(((UmiLanguageFormattingSnapshot *)0)->document_id), 0 },
    {"provider_id", offsetof(UmiLanguageFormattingSnapshot, provider_id), sizeof(((UmiLanguageFormattingSnapshot *)0)->provider_id), 0 },
    {"mode", offsetof(UmiLanguageFormattingSnapshot, mode), sizeof(((UmiLanguageFormattingSnapshot *)0)->mode), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageFormattingSnapshot *left, const UmiLanguageFormattingSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->provider_id, right->provider_id, sizeof(left->provider_id)) == 0 &&
        memcmp(left->mode, right->mode, sizeof(left->mode)) == 0 &&
        left->tab_size == right->tab_size &&
        left->insert_spaces == right->insert_spaces &&
        left->available == right->available &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageFormattingSnapshot *item)
{
    item->document_id[0] = 'v';
    item->provider_id[0] = 'v';
    item->mode[0] = 'v';
    item->tab_size = (uint32_t)7U;
    item->insert_spaces = (int)8U;
    item->available = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_language_formatting_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_language_formatting_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiLanguageFormattingEdit
#define CONTRACT_EDIT_CURRENT umi_language_formatting_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_language_formatting_registry_read_page
#include "snapshot_contract_cases.h"
