/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_editor_document.c
 * PURPOSE: Check editor document input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/document.h"
#include "umicom/editor/document.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiEditorDocumentSnapshot
#define CONTRACT_REGISTRY UmiEditorDocumentRegistry
#define CONTRACT_CAPACITY UMI_EDITOR_DOCUMENT_CAPACITY
#define CONTRACT_VALIDATE umi_editor_document_snapshot_validate
#define CONTRACT_BATCH umi_editor_document_registry_upsert_many
#define CONTRACT_CREATE umi_editor_document_registry_create
#define CONTRACT_DESTROY umi_editor_document_registry_destroy
#define CONTRACT_UPSERT umi_editor_document_registry_upsert
#define CONTRACT_REMOVE umi_editor_document_registry_remove
#define CONTRACT_FIND umi_editor_document_registry_find
#define CONTRACT_AT umi_editor_document_registry_at
#define CONTRACT_COUNT umi_editor_document_registry_count
#define CONTRACT_REVISION umi_editor_document_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiEditorDocumentSnapshot, id), sizeof(((UmiEditorDocumentSnapshot *)0)->id), 1 },
    {"uri", offsetof(UmiEditorDocumentSnapshot, uri), sizeof(((UmiEditorDocumentSnapshot *)0)->uri), 0 },
    {"language_id", offsetof(UmiEditorDocumentSnapshot, language_id), sizeof(((UmiEditorDocumentSnapshot *)0)->language_id), 0 },
    {"title", offsetof(UmiEditorDocumentSnapshot, title), sizeof(((UmiEditorDocumentSnapshot *)0)->title), 0 }
};
static int ContractSnapshotEqual(const UmiEditorDocumentSnapshot *left, const UmiEditorDocumentSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->uri, right->uri, sizeof(left->uri)) == 0 &&
        memcmp(left->language_id, right->language_id, sizeof(left->language_id)) == 0 &&
        memcmp(left->title, right->title, sizeof(left->title)) == 0 &&
        left->version == right->version &&
        left->byte_count == right->byte_count &&
        left->line_count == right->line_count &&
        left->dirty == right->dirty &&
        left->read_only == right->read_only &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiEditorDocumentSnapshot *item)
{
    item->uri[0] = 'v';
    item->language_id[0] = 'v';
    item->title[0] = 'v';
    item->version = (uint64_t)7U;
    item->byte_count = (uint64_t)8U;
    item->line_count = (uint64_t)9U;
    item->dirty = (int)10U;
    item->read_only = (int)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_editor_document_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_editor_document_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiEditorDocumentEdit
#define CONTRACT_EDIT_CURRENT umi_editor_document_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_editor_document_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiEditorDocumentSnapshot ArchiveSample(void)
{
    UmiEditorDocumentSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiEditorDocumentSnapshot *value)
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
    {
        size_t used = strlen(value->title) + 1U;
        memset(value->title + used, 0xa5, sizeof(value->title) - used);
    }
}
#define ARCHIVE_TYPE UmiEditorDocumentSnapshot
#define ARCHIVE_ENCODE umi_editor_document_snapshot_archive_encode
#define ARCHIVE_DECODE umi_editor_document_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_editor_document_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_editor_document_registry_archive_restore
#include "snapshot_contract_cases.h"
