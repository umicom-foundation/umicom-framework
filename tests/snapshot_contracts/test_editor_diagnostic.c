/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_editor_diagnostic.c
 * PURPOSE: Check editor diagnostic input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/diagnostic.h"
#include "umicom/editor/diagnostic.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiEditorDiagnosticSnapshot
#define CONTRACT_REGISTRY UmiEditorDiagnosticRegistry
#define CONTRACT_CAPACITY UMI_EDITOR_DIAGNOSTIC_CAPACITY
#define CONTRACT_VALIDATE umi_editor_diagnostic_snapshot_validate
#define CONTRACT_REPLACE_DOCUMENT umi_editor_diagnostic_registry_replace_document
#define CONTRACT_BATCH umi_editor_diagnostic_registry_upsert_many
#define CONTRACT_CREATE umi_editor_diagnostic_registry_create
#define CONTRACT_DESTROY umi_editor_diagnostic_registry_destroy
#define CONTRACT_UPSERT umi_editor_diagnostic_registry_upsert
#define CONTRACT_REMOVE umi_editor_diagnostic_registry_remove
#define CONTRACT_FIND umi_editor_diagnostic_registry_find
#define CONTRACT_AT umi_editor_diagnostic_registry_at
#define CONTRACT_COUNT umi_editor_diagnostic_registry_count
#define CONTRACT_REVISION umi_editor_diagnostic_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiEditorDiagnosticSnapshot, id), sizeof(((UmiEditorDiagnosticSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiEditorDiagnosticSnapshot, document_id), sizeof(((UmiEditorDiagnosticSnapshot *)0)->document_id), 0 },
    {"source", offsetof(UmiEditorDiagnosticSnapshot, source), sizeof(((UmiEditorDiagnosticSnapshot *)0)->source), 0 },
    {"code", offsetof(UmiEditorDiagnosticSnapshot, code), sizeof(((UmiEditorDiagnosticSnapshot *)0)->code), 0 },
    {"message", offsetof(UmiEditorDiagnosticSnapshot, message), sizeof(((UmiEditorDiagnosticSnapshot *)0)->message), 0 }
};
static int ContractSnapshotEqual(const UmiEditorDiagnosticSnapshot *left, const UmiEditorDiagnosticSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->source, right->source, sizeof(left->source)) == 0 &&
        memcmp(left->code, right->code, sizeof(left->code)) == 0 &&
        memcmp(left->message, right->message, sizeof(left->message)) == 0 &&
        left->severity == right->severity &&
        left->line == right->line &&
        left->column == right->column &&
        left->end_line == right->end_line &&
        left->end_column == right->end_column &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiEditorDiagnosticSnapshot *item)
{
    item->document_id[0] = 'v';
    item->source[0] = 'v';
    item->code[0] = 'v';
    item->message[0] = 'v';
    item->severity = (int)8U;
    item->line = (uint64_t)9U;
    item->column = (uint64_t)10U;
    item->end_line = (uint64_t)11U;
    item->end_column = (uint64_t)12U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_editor_diagnostic_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_editor_diagnostic_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiEditorDiagnosticEdit
#define CONTRACT_EDIT_CURRENT umi_editor_diagnostic_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_editor_diagnostic_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiEditorDiagnosticSnapshot ArchiveSample(void)
{
    UmiEditorDiagnosticSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiEditorDiagnosticSnapshot *value)
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
        size_t used = strlen(value->source) + 1U;
        memset(value->source + used, 0xa5, sizeof(value->source) - used);
    }
    {
        size_t used = strlen(value->code) + 1U;
        memset(value->code + used, 0xa5, sizeof(value->code) - used);
    }
    {
        size_t used = strlen(value->message) + 1U;
        memset(value->message + used, 0xa5, sizeof(value->message) - used);
    }
}
#define ARCHIVE_TYPE UmiEditorDiagnosticSnapshot
#define ARCHIVE_ENCODE umi_editor_diagnostic_snapshot_archive_encode
#define ARCHIVE_DECODE umi_editor_diagnostic_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_editor_diagnostic_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_editor_diagnostic_registry_archive_restore
#include "snapshot_contract_cases.h"
