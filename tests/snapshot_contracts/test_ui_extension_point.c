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
#include "snapshot_contract_cases.h"
