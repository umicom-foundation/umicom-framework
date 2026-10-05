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
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiResourceLocationEdit
#define CONTRACT_EDIT_CURRENT umi_platform_resource_location_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_platform_resource_location_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiResourceLocationSnapshot ArchiveSample(void)
{
    UmiResourceLocationSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiResourceLocationSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->uri) + 1U;
        memset(value->uri + used, 0xa5, sizeof(value->uri) - used);
    }
    {
        size_t used = strlen(value->display_name) + 1U;
        memset(value->display_name + used, 0xa5, sizeof(value->display_name) - used);
    }
    {
        size_t used = strlen(value->scheme) + 1U;
        memset(value->scheme + used, 0xa5, sizeof(value->scheme) - used);
    }
    {
        size_t used = strlen(value->authority) + 1U;
        memset(value->authority + used, 0xa5, sizeof(value->authority) - used);
    }
    {
        size_t used = strlen(value->path) + 1U;
        memset(value->path + used, 0xa5, sizeof(value->path) - used);
    }
}
#define ARCHIVE_TYPE UmiResourceLocationSnapshot
#define ARCHIVE_ENCODE umi_platform_resource_location_snapshot_archive_encode
#define ARCHIVE_DECODE umi_platform_resource_location_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_platform_resource_location_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_platform_resource_location_registry_archive_restore
#include "snapshot_contract_cases.h"
