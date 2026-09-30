/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_designer_clipboard.c
 * PURPOSE: Check designer clipboard input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/clipboard.h"
#include "umicom/designer/clipboard.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDesignerClipboardItemSnapshot
#define CONTRACT_REGISTRY UmiDesignerClipboardItemRegistry
#define CONTRACT_CAPACITY UMI_DESIGNER_CLIPBOARD_CAPACITY
#define CONTRACT_VALIDATE umi_designer_clipboard_snapshot_validate
#define CONTRACT_BATCH umi_designer_clipboard_registry_upsert_many
#define CONTRACT_CREATE umi_designer_clipboard_registry_create
#define CONTRACT_DESTROY umi_designer_clipboard_registry_destroy
#define CONTRACT_UPSERT umi_designer_clipboard_registry_upsert
#define CONTRACT_REMOVE umi_designer_clipboard_registry_remove
#define CONTRACT_FIND umi_designer_clipboard_registry_find
#define CONTRACT_AT umi_designer_clipboard_registry_at
#define CONTRACT_COUNT umi_designer_clipboard_registry_count
#define CONTRACT_REVISION umi_designer_clipboard_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDesignerClipboardItemSnapshot, id), sizeof(((UmiDesignerClipboardItemSnapshot *)0)->id), 1 },
    {"source_node_id", offsetof(UmiDesignerClipboardItemSnapshot, source_node_id), sizeof(((UmiDesignerClipboardItemSnapshot *)0)->source_node_id), 0 },
    {"component_type", offsetof(UmiDesignerClipboardItemSnapshot, component_type), sizeof(((UmiDesignerClipboardItemSnapshot *)0)->component_type), 0 },
    {"serialized", offsetof(UmiDesignerClipboardItemSnapshot, serialized), sizeof(((UmiDesignerClipboardItemSnapshot *)0)->serialized), 0 }
};
static int ContractSnapshotEqual(const UmiDesignerClipboardItemSnapshot *left, const UmiDesignerClipboardItemSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->source_node_id, right->source_node_id, sizeof(left->source_node_id)) == 0 &&
        memcmp(left->component_type, right->component_type, sizeof(left->component_type)) == 0 &&
        memcmp(left->serialized, right->serialized, sizeof(left->serialized)) == 0 &&
        left->copied_at == right->copied_at &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiDesignerClipboardItemSnapshot *item)
{
    item->source_node_id[0] = 'v';
    item->component_type[0] = 'v';
    item->serialized[0] = 'v';
    item->copied_at = (uint64_t)7U;
}
#include "snapshot_contract_cases.h"
