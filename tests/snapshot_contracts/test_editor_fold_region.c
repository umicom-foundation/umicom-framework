/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_editor_fold_region.c
 * PURPOSE: Check editor fold_region input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/fold_region.h"
#include "umicom/editor/fold_region.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiEditorFoldRegionSnapshot
#define CONTRACT_REGISTRY UmiEditorFoldRegionRegistry
#define CONTRACT_CAPACITY UMI_EDITOR_FOLD_REGION_CAPACITY
#define CONTRACT_VALIDATE umi_editor_fold_region_snapshot_validate
#define CONTRACT_BATCH umi_editor_fold_region_registry_upsert_many
#define CONTRACT_CREATE umi_editor_fold_region_registry_create
#define CONTRACT_DESTROY umi_editor_fold_region_registry_destroy
#define CONTRACT_UPSERT umi_editor_fold_region_registry_upsert
#define CONTRACT_REMOVE umi_editor_fold_region_registry_remove
#define CONTRACT_FIND umi_editor_fold_region_registry_find
#define CONTRACT_AT umi_editor_fold_region_registry_at
#define CONTRACT_COUNT umi_editor_fold_region_registry_count
#define CONTRACT_REVISION umi_editor_fold_region_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiEditorFoldRegionSnapshot, id), sizeof(((UmiEditorFoldRegionSnapshot *)0)->id), 1 },
    {"document_id", offsetof(UmiEditorFoldRegionSnapshot, document_id), sizeof(((UmiEditorFoldRegionSnapshot *)0)->document_id), 0 },
    {"kind", offsetof(UmiEditorFoldRegionSnapshot, kind), sizeof(((UmiEditorFoldRegionSnapshot *)0)->kind), 0 }
};
static int ContractSnapshotEqual(const UmiEditorFoldRegionSnapshot *left, const UmiEditorFoldRegionSnapshot *right)
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
static void ContractPayload(UmiEditorFoldRegionSnapshot *item)
{
    item->document_id[0] = 'v';
    item->kind[0] = 'v';
    item->start_line = (uint64_t)6U;
    item->end_line = (uint64_t)7U;
    item->collapsed = (int)8U;
}
#include "snapshot_contract_cases.h"
