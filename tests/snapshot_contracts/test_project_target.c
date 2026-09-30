/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_project_target.c
 * PURPOSE: Exercise project target snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/target.h"
#include "umicom/project/target.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProjectTargetSnapshot
#define CONTRACT_REGISTRY UmiProjectTargetRegistry
#define CONTRACT_CAPACITY UMI_PROJECT_TARGET_CAPACITY
#define CONTRACT_VALIDATE umi_project_target_snapshot_validate
#define CONTRACT_BATCH umi_project_target_registry_upsert_many
#define CONTRACT_CREATE umi_project_target_registry_create
#define CONTRACT_DESTROY umi_project_target_registry_destroy
#define CONTRACT_UPSERT umi_project_target_registry_upsert
#define CONTRACT_REMOVE umi_project_target_registry_remove
#define CONTRACT_FIND umi_project_target_registry_find
#define CONTRACT_AT umi_project_target_registry_at
#define CONTRACT_COUNT umi_project_target_registry_count
#define CONTRACT_REVISION umi_project_target_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProjectTargetSnapshot, id), sizeof(((UmiProjectTargetSnapshot *)0)->id), 1 },
    {"project_id", offsetof(UmiProjectTargetSnapshot, project_id), sizeof(((UmiProjectTargetSnapshot *)0)->project_id), 0 },
    {"name", offsetof(UmiProjectTargetSnapshot, name), sizeof(((UmiProjectTargetSnapshot *)0)->name), 0 },
    {"kind", offsetof(UmiProjectTargetSnapshot, kind), sizeof(((UmiProjectTargetSnapshot *)0)->kind), 0 },
    {"output_uri", offsetof(UmiProjectTargetSnapshot, output_uri), sizeof(((UmiProjectTargetSnapshot *)0)->output_uri), 0 }
};
static int ContractSnapshotEqual(const UmiProjectTargetSnapshot *left,
    const UmiProjectTargetSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->project_id, right->project_id, sizeof(left->project_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->output_uri, right->output_uri, sizeof(left->output_uri)) == 0 &&
        left->enabled == right->enabled &&
        left->default_target == right->default_target &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiProjectTargetSnapshot *item)
{
    item->project_id[0] = 'v';
    item->name[0] = 'v';
    item->kind[0] = 'v';
    item->output_uri[0] = 'v';
    item->enabled = (int)8U;
    item->default_target = (int)9U;
}
#include "snapshot_contract_cases.h"
