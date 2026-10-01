/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_frontend_widget_tree.c
 * PURPOSE: Exercise frontend widget_tree snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/widget_tree.h"
#include "umicom/frontend/widget_tree.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiFrontendWidgetSnapshot
#define CONTRACT_REGISTRY UmiFrontendWidgetRegistry
#define CONTRACT_CAPACITY UMI_FRONTEND_WIDGET_TREE_CAPACITY
#define CONTRACT_VALIDATE umi_frontend_widget_tree_snapshot_validate
#define CONTRACT_BATCH umi_frontend_widget_tree_registry_upsert_many
#define CONTRACT_CREATE umi_frontend_widget_tree_registry_create
#define CONTRACT_DESTROY umi_frontend_widget_tree_registry_destroy
#define CONTRACT_UPSERT umi_frontend_widget_tree_registry_upsert
#define CONTRACT_REMOVE umi_frontend_widget_tree_registry_remove
#define CONTRACT_FIND umi_frontend_widget_tree_registry_find
#define CONTRACT_AT umi_frontend_widget_tree_registry_at
#define CONTRACT_COUNT umi_frontend_widget_tree_registry_count
#define CONTRACT_REVISION umi_frontend_widget_tree_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiFrontendWidgetSnapshot, id), sizeof(((UmiFrontendWidgetSnapshot *)0)->id), 1 },
    {"parent_id", offsetof(UmiFrontendWidgetSnapshot, parent_id), sizeof(((UmiFrontendWidgetSnapshot *)0)->parent_id), 0 },
    {"type", offsetof(UmiFrontendWidgetSnapshot, type), sizeof(((UmiFrontendWidgetSnapshot *)0)->type), 0 },
    {"text", offsetof(UmiFrontendWidgetSnapshot, text), sizeof(((UmiFrontendWidgetSnapshot *)0)->text), 0 },
    {"style_class", offsetof(UmiFrontendWidgetSnapshot, style_class), sizeof(((UmiFrontendWidgetSnapshot *)0)->style_class), 0 }
};
static int ContractSnapshotEqual(const UmiFrontendWidgetSnapshot *left,
    const UmiFrontendWidgetSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->parent_id, right->parent_id, sizeof(left->parent_id)) == 0 &&
        memcmp(left->type, right->type, sizeof(left->type)) == 0 &&
        memcmp(left->text, right->text, sizeof(left->text)) == 0 &&
        memcmp(left->style_class, right->style_class, sizeof(left->style_class)) == 0 &&
        left->visible == right->visible &&
        left->enabled == right->enabled &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiFrontendWidgetSnapshot *item)
{
    item->parent_id[0] = 'v';
    item->type[0] = 'v';
    item->text[0] = 'v';
    item->style_class[0] = 'v';
    item->visible = (int)8U;
    item->enabled = (int)9U;
    item->order = (int32_t)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_frontend_widget_tree_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_frontend_widget_tree_registry_replace_if_current
#include "snapshot_contract_cases.h"
