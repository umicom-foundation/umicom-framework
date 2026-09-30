/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_frontend_render_tree.c
 * PURPOSE: Exercise frontend render_tree snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/render_tree.h"
#include "umicom/frontend/render_tree.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiFrontendRenderNodeSnapshot
#define CONTRACT_REGISTRY UmiFrontendRenderNodeRegistry
#define CONTRACT_CAPACITY UMI_FRONTEND_RENDER_TREE_CAPACITY
#define CONTRACT_VALIDATE umi_frontend_render_tree_snapshot_validate
#define CONTRACT_BATCH umi_frontend_render_tree_registry_upsert_many
#define CONTRACT_CREATE umi_frontend_render_tree_registry_create
#define CONTRACT_DESTROY umi_frontend_render_tree_registry_destroy
#define CONTRACT_UPSERT umi_frontend_render_tree_registry_upsert
#define CONTRACT_REMOVE umi_frontend_render_tree_registry_remove
#define CONTRACT_FIND umi_frontend_render_tree_registry_find
#define CONTRACT_AT umi_frontend_render_tree_registry_at
#define CONTRACT_COUNT umi_frontend_render_tree_registry_count
#define CONTRACT_REVISION umi_frontend_render_tree_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiFrontendRenderNodeSnapshot, id), sizeof(((UmiFrontendRenderNodeSnapshot *)0)->id), 1 },
    {"widget_id", offsetof(UmiFrontendRenderNodeSnapshot, widget_id), sizeof(((UmiFrontendRenderNodeSnapshot *)0)->widget_id), 0 },
    {"parent_id", offsetof(UmiFrontendRenderNodeSnapshot, parent_id), sizeof(((UmiFrontendRenderNodeSnapshot *)0)->parent_id), 0 },
    {"markup", offsetof(UmiFrontendRenderNodeSnapshot, markup), sizeof(((UmiFrontendRenderNodeSnapshot *)0)->markup), 0 }
};
static int ContractSnapshotEqual(const UmiFrontendRenderNodeSnapshot *left,
    const UmiFrontendRenderNodeSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->widget_id, right->widget_id, sizeof(left->widget_id)) == 0 &&
        memcmp(left->parent_id, right->parent_id, sizeof(left->parent_id)) == 0 &&
        memcmp(left->markup, right->markup, sizeof(left->markup)) == 0 &&
        left->checksum == right->checksum &&
        left->dirty == right->dirty &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiFrontendRenderNodeSnapshot *item)
{
    item->widget_id[0] = 'v';
    item->parent_id[0] = 'v';
    item->markup[0] = 'v';
    item->checksum = (uint64_t)7U;
    item->dirty = (int)8U;
    item->order = (int32_t)9U;
}
#include "snapshot_contract_cases.h"
