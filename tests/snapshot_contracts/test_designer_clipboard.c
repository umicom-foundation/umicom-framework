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
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_designer_clipboard_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_designer_clipboard_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiDesignerClipboardItemEdit
#define CONTRACT_EDIT_CURRENT umi_designer_clipboard_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_designer_clipboard_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiDesignerClipboardItemSnapshot ArchiveSample(void)
{
    UmiDesignerClipboardItemSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiDesignerClipboardItemSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->source_node_id) + 1U;
        memset(value->source_node_id + used, 0xa5, sizeof(value->source_node_id) - used);
    }
    {
        size_t used = strlen(value->component_type) + 1U;
        memset(value->component_type + used, 0xa5, sizeof(value->component_type) - used);
    }
    {
        size_t used = strlen(value->serialized) + 1U;
        memset(value->serialized + used, 0xa5, sizeof(value->serialized) - used);
    }
}
#define ARCHIVE_TYPE UmiDesignerClipboardItemSnapshot
#define ARCHIVE_ENCODE umi_designer_clipboard_snapshot_archive_encode
#define ARCHIVE_DECODE umi_designer_clipboard_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_designer_clipboard_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_designer_clipboard_registry_archive_restore
#include "snapshot_contract_cases.h"
