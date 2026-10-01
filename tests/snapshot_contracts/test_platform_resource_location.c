/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_platform_resource_location.c
 * PURPOSE: Exercise platform resource_location snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/resource_location.h"
#include "umicom/platform/resource_location.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiResourceLocationSnapshot
#define CONTRACT_REGISTRY UmiResourceLocationRegistry
#define CONTRACT_CAPACITY UMI_PLATFORM_RESOURCE_LOCATION_CAPACITY
#define CONTRACT_VALIDATE umi_platform_resource_location_snapshot_validate
#define CONTRACT_BATCH umi_platform_resource_location_registry_upsert_many
#define CONTRACT_CREATE umi_platform_resource_location_registry_create
#define CONTRACT_DESTROY umi_platform_resource_location_registry_destroy
#define CONTRACT_UPSERT umi_platform_resource_location_registry_upsert
#define CONTRACT_REMOVE umi_platform_resource_location_registry_remove
#define CONTRACT_FIND umi_platform_resource_location_registry_find
#define CONTRACT_AT umi_platform_resource_location_registry_at
#define CONTRACT_COUNT umi_platform_resource_location_registry_count
#define CONTRACT_REVISION umi_platform_resource_location_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiResourceLocationSnapshot, id), sizeof(((UmiResourceLocationSnapshot *)0)->id), 1 },
    {"uri", offsetof(UmiResourceLocationSnapshot, uri), sizeof(((UmiResourceLocationSnapshot *)0)->uri), 0 },
    {"display_name", offsetof(UmiResourceLocationSnapshot, display_name), sizeof(((UmiResourceLocationSnapshot *)0)->display_name), 0 },
    {"scheme", offsetof(UmiResourceLocationSnapshot, scheme), sizeof(((UmiResourceLocationSnapshot *)0)->scheme), 0 },
    {"authority", offsetof(UmiResourceLocationSnapshot, authority), sizeof(((UmiResourceLocationSnapshot *)0)->authority), 0 },
    {"path", offsetof(UmiResourceLocationSnapshot, path), sizeof(((UmiResourceLocationSnapshot *)0)->path), 0 }
};
static int ContractSnapshotEqual(const UmiResourceLocationSnapshot *left,
    const UmiResourceLocationSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->uri, right->uri, sizeof(left->uri)) == 0 &&
        memcmp(left->display_name, right->display_name, sizeof(left->display_name)) == 0 &&
        memcmp(left->scheme, right->scheme, sizeof(left->scheme)) == 0 &&
        memcmp(left->authority, right->authority, sizeof(left->authority)) == 0 &&
        memcmp(left->path, right->path, sizeof(left->path)) == 0 &&
        left->local == right->local &&
        left->writable == right->writable &&
        left->available == right->available &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiResourceLocationSnapshot *item)
{
    item->uri[0] = 'v';
    item->display_name[0] = 'v';
    item->scheme[0] = 'v';
    item->authority[0] = 'v';
    item->path[0] = 'v';
    item->local = (int)9U;
    item->writable = (int)10U;
    item->available = (int)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_platform_resource_location_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_platform_resource_location_registry_replace_if_current
#include "snapshot_contract_cases.h"
