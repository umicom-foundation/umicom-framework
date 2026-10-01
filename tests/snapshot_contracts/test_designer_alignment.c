/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_designer_alignment.c
 * PURPOSE: Check designer alignment input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/alignment.h"
#include "umicom/designer/alignment.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDesignerAlignmentSnapshot
#define CONTRACT_REGISTRY UmiDesignerAlignmentRegistry
#define CONTRACT_CAPACITY UMI_DESIGNER_ALIGNMENT_CAPACITY
#define CONTRACT_VALIDATE umi_designer_alignment_snapshot_validate
#define CONTRACT_BATCH umi_designer_alignment_registry_upsert_many
#define CONTRACT_CREATE umi_designer_alignment_registry_create
#define CONTRACT_DESTROY umi_designer_alignment_registry_destroy
#define CONTRACT_UPSERT umi_designer_alignment_registry_upsert
#define CONTRACT_REMOVE umi_designer_alignment_registry_remove
#define CONTRACT_FIND umi_designer_alignment_registry_find
#define CONTRACT_AT umi_designer_alignment_registry_at
#define CONTRACT_COUNT umi_designer_alignment_registry_count
#define CONTRACT_REVISION umi_designer_alignment_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDesignerAlignmentSnapshot, id), sizeof(((UmiDesignerAlignmentSnapshot *)0)->id), 1 },
    {"operation", offsetof(UmiDesignerAlignmentSnapshot, operation), sizeof(((UmiDesignerAlignmentSnapshot *)0)->operation), 0 },
    {"selection_id", offsetof(UmiDesignerAlignmentSnapshot, selection_id), sizeof(((UmiDesignerAlignmentSnapshot *)0)->selection_id), 0 }
};
static int ContractSnapshotEqual(const UmiDesignerAlignmentSnapshot *left, const UmiDesignerAlignmentSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->operation, right->operation, sizeof(left->operation)) == 0 &&
        memcmp(left->selection_id, right->selection_id, sizeof(left->selection_id)) == 0 &&
        left->spacing == right->spacing &&
        left->horizontal == right->horizontal &&
        left->vertical == right->vertical &&
        left->distribute == right->distribute &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiDesignerAlignmentSnapshot *item)
{
    item->operation[0] = 'v';
    item->selection_id[0] = 'v';
    item->spacing = (double)6U;
    item->horizontal = (int)7U;
    item->vertical = (int)8U;
    item->distribute = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_designer_alignment_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_designer_alignment_registry_replace_if_current
#include "snapshot_contract_cases.h"
