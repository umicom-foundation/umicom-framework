/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_signature.c
 * PURPOSE: Check language signature input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/signature.h"
#include "umicom/language/signature.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageSignatureSnapshot
#define CONTRACT_REGISTRY UmiLanguageSignatureRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_SIGNATURE_CAPACITY
#define CONTRACT_VALIDATE umi_language_signature_snapshot_validate
#define CONTRACT_REPLACE_DOCUMENT umi_language_signature_registry_replace_document
#define CONTRACT_BATCH umi_language_signature_registry_upsert_many
#define CONTRACT_CREATE umi_language_signature_registry_create
#define CONTRACT_DESTROY umi_language_signature_registry_destroy
#define CONTRACT_UPSERT umi_language_signature_registry_upsert
#define CONTRACT_REMOVE umi_language_signature_registry_remove
#define CONTRACT_FIND umi_language_signature_registry_find
#define CONTRACT_AT umi_language_signature_registry_at
#define CONTRACT_COUNT umi_language_signature_registry_count
#define CONTRACT_REVISION umi_language_signature_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageSignatureSnapshot, id), sizeof(((UmiLanguageSignatureSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiLanguageSignatureSnapshot, document_id), sizeof(((UmiLanguageSignatureSnapshot *)0)->document_id), 0 },
    {"label", offsetof(UmiLanguageSignatureSnapshot, label), sizeof(((UmiLanguageSignatureSnapshot *)0)->label), 0 },
    {"documentation", offsetof(UmiLanguageSignatureSnapshot, documentation), sizeof(((UmiLanguageSignatureSnapshot *)0)->documentation), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageSignatureSnapshot *left, const UmiLanguageSignatureSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->label, right->label, sizeof(left->label)) == 0 &&
        memcmp(left->documentation, right->documentation, sizeof(left->documentation)) == 0 &&
        left->active_parameter == right->active_parameter &&
        left->line == right->line &&
        left->column == right->column &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageSignatureSnapshot *item)
{
    item->document_id[0] = 'v';
    item->label[0] = 'v';
    item->documentation[0] = 'v';
    item->active_parameter = (uint32_t)7U;
    item->line = (uint32_t)8U;
    item->column = (uint32_t)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_language_signature_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_language_signature_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiLanguageSignatureEdit
#define CONTRACT_EDIT_CURRENT umi_language_signature_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_language_signature_registry_read_page
#include "snapshot_contract_cases.h"
