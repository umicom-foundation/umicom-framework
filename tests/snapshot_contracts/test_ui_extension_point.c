/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_extension_point.c
 * PURPOSE: Exercise ui extension_point snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/extension_point.h"
#include "umicom/ui/extension_point.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiExtensionPointSnapshot
#define CONTRACT_REGISTRY UmiUiExtensionPointRegistry
#define CONTRACT_CAPACITY UMI_UI_EXTENSION_POINT_CAPACITY
#define CONTRACT_VALIDATE umi_ui_extension_point_snapshot_validate
#define CONTRACT_BATCH umi_ui_extension_point_registry_upsert_many
#define CONTRACT_CREATE umi_ui_extension_point_registry_create
#define CONTRACT_DESTROY umi_ui_extension_point_registry_destroy
#define CONTRACT_UPSERT umi_ui_extension_point_registry_upsert
#define CONTRACT_REMOVE umi_ui_extension_point_registry_remove
#define CONTRACT_FIND umi_ui_extension_point_registry_find
#define CONTRACT_AT umi_ui_extension_point_registry_at
#define CONTRACT_COUNT umi_ui_extension_point_registry_count
#define CONTRACT_REVISION umi_ui_extension_point_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiExtensionPointSnapshot, id), sizeof(((UmiUiExtensionPointSnapshot *)0)->id), 1 },
    {"owner", offsetof(UmiUiExtensionPointSnapshot, owner), sizeof(((UmiUiExtensionPointSnapshot *)0)->owner), 0 },
    {"kind", offsetof(UmiUiExtensionPointSnapshot, kind), sizeof(((UmiUiExtensionPointSnapshot *)0)->kind), 0 },
    {"location", offsetof(UmiUiExtensionPointSnapshot, location), sizeof(((UmiUiExtensionPointSnapshot *)0)->location), 0 },
    {"schema_id", offsetof(UmiUiExtensionPointSnapshot, schema_id), sizeof(((UmiUiExtensionPointSnapshot *)0)->schema_id), 0 }
};
static int ContractSnapshotEqual(const UmiUiExtensionPointSnapshot *left,
    const UmiUiExtensionPointSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->owner, right->owner, sizeof(left->owner)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->location, right->location, sizeof(left->location)) == 0 &&
        memcmp(left->schema_id, right->schema_id, sizeof(left->schema_id)) == 0 &&
        left->enabled == right->enabled &&
        left->multiple == right->multiple &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiExtensionPointSnapshot *item)
{
    item->owner[0] = 'v';
    item->kind[0] = 'v';
    item->location[0] = 'v';
    item->schema_id[0] = 'v';
    item->enabled = (int)8U;
    item->multiple = (int)9U;
    item->order = (int32_t)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_extension_point_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_extension_point_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiExtensionPointEdit
#define CONTRACT_EDIT_CURRENT umi_ui_extension_point_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_ui_extension_point_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiUiExtensionPointSnapshot ArchiveSample(void)
{
    UmiUiExtensionPointSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiUiExtensionPointSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->owner) + 1U;
        memset(value->owner + used, 0xa5, sizeof(value->owner) - used);
    }
    {
        size_t used = strlen(value->kind) + 1U;
        memset(value->kind + used, 0xa5, sizeof(value->kind) - used);
    }
    {
        size_t used = strlen(value->location) + 1U;
        memset(value->location + used, 0xa5, sizeof(value->location) - used);
    }
    {
        size_t used = strlen(value->schema_id) + 1U;
        memset(value->schema_id + used, 0xa5, sizeof(value->schema_id) - used);
    }
}
#define ARCHIVE_TYPE UmiUiExtensionPointSnapshot
#define ARCHIVE_ENCODE umi_ui_extension_point_snapshot_archive_encode
#define ARCHIVE_DECODE umi_ui_extension_point_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_ui_extension_point_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_ui_extension_point_registry_archive_restore
#include "snapshot_contract_cases.h"
