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
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_frontend_render_tree_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_frontend_render_tree_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiFrontendRenderNodeEdit
#define CONTRACT_EDIT_CURRENT umi_frontend_render_tree_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_frontend_render_tree_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiFrontendRenderNodeSnapshot ArchiveSample(void)
{
    UmiFrontendRenderNodeSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiFrontendRenderNodeSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->widget_id) + 1U;
        memset(value->widget_id + used, 0xa5, sizeof(value->widget_id) - used);
    }
    {
        size_t used = strlen(value->parent_id) + 1U;
        memset(value->parent_id + used, 0xa5, sizeof(value->parent_id) - used);
    }
    {
        size_t used = strlen(value->markup) + 1U;
        memset(value->markup + used, 0xa5, sizeof(value->markup) - used);
    }
}
#define ARCHIVE_TYPE UmiFrontendRenderNodeSnapshot
#define ARCHIVE_ENCODE umi_frontend_render_tree_snapshot_archive_encode
#define ARCHIVE_DECODE umi_frontend_render_tree_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_frontend_render_tree_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_frontend_render_tree_registry_archive_restore
#include "snapshot_contract_cases.h"
