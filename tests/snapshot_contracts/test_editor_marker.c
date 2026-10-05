/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_editor_marker.c
 * PURPOSE: Check editor marker input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/marker.h"
#include "umicom/editor/marker.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiEditorMarkerSnapshot
#define CONTRACT_REGISTRY UmiEditorMarkerRegistry
#define CONTRACT_CAPACITY UMI_EDITOR_MARKER_CAPACITY
#define CONTRACT_VALIDATE umi_editor_marker_snapshot_validate
#define CONTRACT_BATCH umi_editor_marker_registry_upsert_many
#define CONTRACT_CREATE umi_editor_marker_registry_create
#define CONTRACT_DESTROY umi_editor_marker_registry_destroy
#define CONTRACT_UPSERT umi_editor_marker_registry_upsert
#define CONTRACT_REMOVE umi_editor_marker_registry_remove
#define CONTRACT_FIND umi_editor_marker_registry_find
#define CONTRACT_AT umi_editor_marker_registry_at
#define CONTRACT_COUNT umi_editor_marker_registry_count
#define CONTRACT_REVISION umi_editor_marker_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiEditorMarkerSnapshot, id), sizeof(((UmiEditorMarkerSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiEditorMarkerSnapshot, document_id), sizeof(((UmiEditorMarkerSnapshot *)0)->document_id), 0 },
    {"kind", offsetof(UmiEditorMarkerSnapshot, kind), sizeof(((UmiEditorMarkerSnapshot *)0)->kind), 0 },
    {"label", offsetof(UmiEditorMarkerSnapshot, label), sizeof(((UmiEditorMarkerSnapshot *)0)->label), 0 }
};
static int ContractSnapshotEqual(const UmiEditorMarkerSnapshot *left, const UmiEditorMarkerSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->document_id, right->document_id, sizeof(left->document_id)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->label, right->label, sizeof(left->label)) == 0 &&
        left->line == right->line &&
        left->column == right->column &&
        left->severity == right->severity &&
        left->enabled == right->enabled &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiEditorMarkerSnapshot *item)
{
    item->document_id[0] = 'v';
    item->kind[0] = 'v';
    item->label[0] = 'v';
    item->line = (uint64_t)7U;
    item->column = (uint64_t)8U;
    item->severity = (int)9U;
    item->enabled = (int)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_editor_marker_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_editor_marker_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiEditorMarkerEdit
#define CONTRACT_EDIT_CURRENT umi_editor_marker_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_editor_marker_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiEditorMarkerSnapshot ArchiveSample(void)
{
    UmiEditorMarkerSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiEditorMarkerSnapshot *value)
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
        size_t used = strlen(value->kind) + 1U;
        memset(value->kind + used, 0xa5, sizeof(value->kind) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
#define ARCHIVE_TYPE UmiEditorMarkerSnapshot
#define ARCHIVE_ENCODE umi_editor_marker_snapshot_archive_encode
#define ARCHIVE_DECODE umi_editor_marker_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_editor_marker_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_editor_marker_registry_archive_restore
#include "snapshot_contract_cases.h"
