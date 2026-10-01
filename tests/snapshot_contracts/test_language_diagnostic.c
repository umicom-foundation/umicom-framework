/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_language_diagnostic.c
 * PURPOSE: Check language diagnostic input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/diagnostic.h"
#include "umicom/language/diagnostic.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiLanguageDiagnosticSnapshot
#define CONTRACT_REGISTRY UmiLanguageDiagnosticRegistry
#define CONTRACT_CAPACITY UMI_LANGUAGE_DIAGNOSTIC_CAPACITY
#define CONTRACT_VALIDATE umi_language_diagnostic_snapshot_validate
#define CONTRACT_REPLACE_DOCUMENT umi_language_diagnostic_registry_replace_document
#define CONTRACT_BATCH umi_language_diagnostic_registry_upsert_many
#define CONTRACT_CREATE umi_language_diagnostic_registry_create
#define CONTRACT_DESTROY umi_language_diagnostic_registry_destroy
#define CONTRACT_UPSERT umi_language_diagnostic_registry_upsert
#define CONTRACT_REMOVE umi_language_diagnostic_registry_remove
#define CONTRACT_FIND umi_language_diagnostic_registry_find
#define CONTRACT_AT umi_language_diagnostic_registry_at
#define CONTRACT_COUNT umi_language_diagnostic_registry_count
#define CONTRACT_REVISION umi_language_diagnostic_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiLanguageDiagnosticSnapshot, id), sizeof(((UmiLanguageDiagnosticSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiLanguageDiagnosticSnapshot, document_id), sizeof(((UmiLanguageDiagnosticSnapshot *)0)->document_id), 0 },
    {"message", offsetof(UmiLanguageDiagnosticSnapshot, message), sizeof(((UmiLanguageDiagnosticSnapshot *)0)->message), 0 },
    {"source", offsetof(UmiLanguageDiagnosticSnapshot, source), sizeof(((UmiLanguageDiagnosticSnapshot *)0)->source), 0 },
    {"code", offsetof(UmiLanguageDiagnosticSnapshot, code), sizeof(((UmiLanguageDiagnosticSnapshot *)0)->code), 0 }
};
static int ContractSnapshotEqual(const UmiLanguageDiagnosticSnapshot *left, const UmiLanguageDiagnosticSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->message, right->message, sizeof(left->message)) == 0 &&
        memcmp(left->source, right->source, sizeof(left->source)) == 0 &&
        memcmp(left->code, right->code, sizeof(left->code)) == 0 &&
        left->severity == right->severity &&
        left->line == right->line &&
        left->column == right->column &&
        left->end_line == right->end_line &&
        left->end_column == right->end_column &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiLanguageDiagnosticSnapshot *item)
{
    item->document_id[0] = 'v';
    item->message[0] = 'v';
    item->source[0] = 'v';
    item->code[0] = 'v';
    item->severity = (int)8U;
    item->line = (uint32_t)9U;
    item->column = (uint32_t)10U;
    item->end_line = (uint32_t)11U;
    item->end_column = (uint32_t)12U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_language_diagnostic_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_language_diagnostic_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiLanguageDiagnosticEdit
#define CONTRACT_EDIT_CURRENT umi_language_diagnostic_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_language_diagnostic_registry_read_page
#include "snapshot_contract_cases.h"
