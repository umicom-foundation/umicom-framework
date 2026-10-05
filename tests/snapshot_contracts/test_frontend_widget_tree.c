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
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiFrontendWidgetEdit
#define CONTRACT_EDIT_CURRENT umi_frontend_widget_tree_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_frontend_widget_tree_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiFrontendWidgetSnapshot ArchiveSample(void)
{
    UmiFrontendWidgetSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiFrontendWidgetSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->parent_id) + 1U;
        memset(value->parent_id + used, 0xa5, sizeof(value->parent_id) - used);
    }
    {
        size_t used = strlen(value->type) + 1U;
        memset(value->type + used, 0xa5, sizeof(value->type) - used);
    }
    {
        size_t used = strlen(value->text) + 1U;
        memset(value->text + used, 0xa5, sizeof(value->text) - used);
    }
    {
        size_t used = strlen(value->style_class) + 1U;
        memset(value->style_class + used, 0xa5, sizeof(value->style_class) - used);
    }
}
#define ARCHIVE_TYPE UmiFrontendWidgetSnapshot
#define ARCHIVE_ENCODE umi_frontend_widget_tree_snapshot_archive_encode
#define ARCHIVE_DECODE umi_frontend_widget_tree_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_frontend_widget_tree_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_frontend_widget_tree_registry_archive_restore
#include "snapshot_contract_cases.h"
