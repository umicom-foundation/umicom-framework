/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_platform_bookmarks.c
 * PURPOSE: Exercise platform bookmarks snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/bookmarks.h"
#include "umicom/platform/bookmarks.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiBookmarkSnapshot
#define CONTRACT_REGISTRY UmiBookmarkRegistry
#define CONTRACT_CAPACITY UMI_PLATFORM_BOOKMARKS_CAPACITY
#define CONTRACT_VALIDATE umi_platform_bookmarks_snapshot_validate
#define CONTRACT_BATCH umi_platform_bookmarks_registry_upsert_many
#define CONTRACT_CREATE umi_platform_bookmarks_registry_create
#define CONTRACT_DESTROY umi_platform_bookmarks_registry_destroy
#define CONTRACT_UPSERT umi_platform_bookmarks_registry_upsert
#define CONTRACT_REMOVE umi_platform_bookmarks_registry_remove
#define CONTRACT_FIND umi_platform_bookmarks_registry_find
#define CONTRACT_AT umi_platform_bookmarks_registry_at
#define CONTRACT_COUNT umi_platform_bookmarks_registry_count
#define CONTRACT_REVISION umi_platform_bookmarks_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiBookmarkSnapshot, id), sizeof(((UmiBookmarkSnapshot *)0)->id), 1 },
    {"uri", offsetof(UmiBookmarkSnapshot, uri), sizeof(((UmiBookmarkSnapshot *)0)->uri), 0 },
    {"label", offsetof(UmiBookmarkSnapshot, label), sizeof(((UmiBookmarkSnapshot *)0)->label), 0 },
    {"group", offsetof(UmiBookmarkSnapshot, group), sizeof(((UmiBookmarkSnapshot *)0)->group), 0 },
    {"icon_name", offsetof(UmiBookmarkSnapshot, icon_name), sizeof(((UmiBookmarkSnapshot *)0)->icon_name), 0 }
};
static int ContractSnapshotEqual(const UmiBookmarkSnapshot *left,
    const UmiBookmarkSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->uri, right->uri, sizeof(left->uri)) == 0 &&
        memcmp(left->label, right->label, sizeof(left->label)) == 0 &&
        memcmp(left->group, right->group, sizeof(left->group)) == 0 &&
        memcmp(left->icon_name, right->icon_name, sizeof(left->icon_name)) == 0 &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiBookmarkSnapshot *item)
{
    item->uri[0] = 'v';
    item->label[0] = 'v';
    item->group[0] = 'v';
    item->icon_name[0] = 'v';
    item->order = (int32_t)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_platform_bookmarks_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_platform_bookmarks_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiBookmarkEdit
#define CONTRACT_EDIT_CURRENT umi_platform_bookmarks_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_platform_bookmarks_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiBookmarkSnapshot ArchiveSample(void)
{
    UmiBookmarkSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiBookmarkSnapshot *value)
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
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
    {
        size_t used = strlen(value->group) + 1U;
        memset(value->group + used, 0xa5, sizeof(value->group) - used);
    }
    {
        size_t used = strlen(value->icon_name) + 1U;
        memset(value->icon_name + used, 0xa5, sizeof(value->icon_name) - used);
    }
}
#define ARCHIVE_TYPE UmiBookmarkSnapshot
#define ARCHIVE_ENCODE umi_platform_bookmarks_snapshot_archive_encode
#define ARCHIVE_DECODE umi_platform_bookmarks_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_platform_bookmarks_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_platform_bookmarks_registry_archive_restore
#include "snapshot_contract_cases.h"
