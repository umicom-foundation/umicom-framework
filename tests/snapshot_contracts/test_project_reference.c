/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_project_reference.c
 * PURPOSE: Exercise project reference snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/reference.h"
#include "umicom/project/reference.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProjectReferenceSnapshot
#define CONTRACT_REGISTRY UmiProjectReferenceRegistry
#define CONTRACT_CAPACITY UMI_PROJECT_REFERENCE_CAPACITY
#define CONTRACT_VALIDATE umi_project_reference_snapshot_validate
#define CONTRACT_BATCH umi_project_reference_registry_upsert_many
#define CONTRACT_CREATE umi_project_reference_registry_create
#define CONTRACT_DESTROY umi_project_reference_registry_destroy
#define CONTRACT_UPSERT umi_project_reference_registry_upsert
#define CONTRACT_REMOVE umi_project_reference_registry_remove
#define CONTRACT_FIND umi_project_reference_registry_find
#define CONTRACT_AT umi_project_reference_registry_at
#define CONTRACT_COUNT umi_project_reference_registry_count
#define CONTRACT_REVISION umi_project_reference_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProjectReferenceSnapshot, id), sizeof(((UmiProjectReferenceSnapshot *)0)->id), 1 },
    {"project_id", offsetof(UmiProjectReferenceSnapshot, project_id), sizeof(((UmiProjectReferenceSnapshot *)0)->project_id), 0 },
    {"target_project_id", offsetof(UmiProjectReferenceSnapshot, target_project_id), sizeof(((UmiProjectReferenceSnapshot *)0)->target_project_id), 0 },
    {"kind", offsetof(UmiProjectReferenceSnapshot, kind), sizeof(((UmiProjectReferenceSnapshot *)0)->kind), 0 }
};
static int ContractSnapshotEqual(const UmiProjectReferenceSnapshot *left,
    const UmiProjectReferenceSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->project_id, right->project_id, sizeof(left->project_id)) == 0 &&
        memcmp(left->target_project_id, right->target_project_id, sizeof(left->target_project_id)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        left->required == right->required &&
        left->available == right->available &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiProjectReferenceSnapshot *item)
{
    item->project_id[0] = 'v';
    item->target_project_id[0] = 'v';
    item->kind[0] = 'v';
    item->required = (int)7U;
    item->available = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_project_reference_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_project_reference_registry_replace_if_current
#include "snapshot_contract_cases.h"
