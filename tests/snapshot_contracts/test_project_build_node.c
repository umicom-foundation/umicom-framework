/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_project_build_node.c
 * PURPOSE: Exercise project build_node snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/build_node.h"
#include "umicom/project/build_node.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProjectBuildNodeSnapshot
#define CONTRACT_REGISTRY UmiProjectBuildNodeRegistry
#define CONTRACT_CAPACITY UMI_PROJECT_BUILD_NODE_CAPACITY
#define CONTRACT_VALIDATE umi_project_build_node_snapshot_validate
#define CONTRACT_BATCH umi_project_build_node_registry_upsert_many
#define CONTRACT_CREATE umi_project_build_node_registry_create
#define CONTRACT_DESTROY umi_project_build_node_registry_destroy
#define CONTRACT_UPSERT umi_project_build_node_registry_upsert
#define CONTRACT_REMOVE umi_project_build_node_registry_remove
#define CONTRACT_FIND umi_project_build_node_registry_find
#define CONTRACT_AT umi_project_build_node_registry_at
#define CONTRACT_COUNT umi_project_build_node_registry_count
#define CONTRACT_REVISION umi_project_build_node_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProjectBuildNodeSnapshot, id), sizeof(((UmiProjectBuildNodeSnapshot *)0)->id), 1 },
    {"project_id", offsetof(UmiProjectBuildNodeSnapshot, project_id), sizeof(((UmiProjectBuildNodeSnapshot *)0)->project_id), 0 },
    {"target_id", offsetof(UmiProjectBuildNodeSnapshot, target_id), sizeof(((UmiProjectBuildNodeSnapshot *)0)->target_id), 0 },
    {"label", offsetof(UmiProjectBuildNodeSnapshot, label), sizeof(((UmiProjectBuildNodeSnapshot *)0)->label), 0 },
    {"kind", offsetof(UmiProjectBuildNodeSnapshot, kind), sizeof(((UmiProjectBuildNodeSnapshot *)0)->kind), 0 },
    {"depends_on", offsetof(UmiProjectBuildNodeSnapshot, depends_on), sizeof(((UmiProjectBuildNodeSnapshot *)0)->depends_on), 0 }
};
static int ContractSnapshotEqual(const UmiProjectBuildNodeSnapshot *left,
    const UmiProjectBuildNodeSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->project_id, right->project_id, sizeof(left->project_id)) == 0 &&
        memcmp(left->target_id, right->target_id, sizeof(left->target_id)) == 0 &&
        memcmp(left->label, right->label, sizeof(left->label)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->depends_on, right->depends_on, sizeof(left->depends_on)) == 0 &&
        left->state == right->state &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiProjectBuildNodeSnapshot *item)
{
    item->project_id[0] = 'v';
    item->target_id[0] = 'v';
    item->label[0] = 'v';
    item->kind[0] = 'v';
    item->depends_on[0] = 'v';
    item->state = (int)9U;
    item->order = (int32_t)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_project_build_node_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_project_build_node_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiProjectBuildNodeEdit
#define CONTRACT_EDIT_CURRENT umi_project_build_node_registry_edit_if_current
#include "snapshot_contract_cases.h"
