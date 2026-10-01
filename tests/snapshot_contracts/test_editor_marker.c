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
#include "snapshot_contract_cases.h"
