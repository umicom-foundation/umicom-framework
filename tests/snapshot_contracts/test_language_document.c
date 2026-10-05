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
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiLanguageDocumentEdit
#define CONTRACT_EDIT_CURRENT umi_language_document_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_language_document_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiLanguageDocumentSnapshot ArchiveSample(void)
{
    UmiLanguageDocumentSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiLanguageDocumentSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->uri) + 1U;
        memset(value->uri + used, 0xa5, sizeof(value->uri) - used);
    }
    {
        size_t used = strlen(value->language_id) + 1U;
        memset(value->language_id + used, 0xa5, sizeof(value->language_id) - used);
    }
}
#define ARCHIVE_TYPE UmiLanguageDocumentSnapshot
#define ARCHIVE_ENCODE umi_language_document_snapshot_archive_encode
#define ARCHIVE_DECODE umi_language_document_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_language_document_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_language_document_registry_archive_restore
#include "snapshot_contract_cases.h"
