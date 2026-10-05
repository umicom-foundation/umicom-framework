/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_debug_watch.c
 * PURPOSE: Exercise debug watch snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/watch.h"
#include "umicom/debug/watch.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDebugWatchSnapshot
#define CONTRACT_REGISTRY UmiDebugWatchRegistry
#define CONTRACT_CAPACITY UMI_DEBUG_WATCH_CAPACITY
#define CONTRACT_VALIDATE umi_debug_watch_snapshot_validate
#define CONTRACT_BATCH umi_debug_watch_registry_upsert_many
#define CONTRACT_CREATE umi_debug_watch_registry_create
#define CONTRACT_DESTROY umi_debug_watch_registry_destroy
#define CONTRACT_UPSERT umi_debug_watch_registry_upsert
#define CONTRACT_REMOVE umi_debug_watch_registry_remove
#define CONTRACT_FIND umi_debug_watch_registry_find
#define CONTRACT_AT umi_debug_watch_registry_at
#define CONTRACT_COUNT umi_debug_watch_registry_count
#define CONTRACT_REVISION umi_debug_watch_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDebugWatchSnapshot, id), sizeof(((UmiDebugWatchSnapshot *)0)->id), 1 },
    {"session_id", offsetof(UmiDebugWatchSnapshot, session_id), sizeof(((UmiDebugWatchSnapshot *)0)->session_id), 0 },
    {"expression", offsetof(UmiDebugWatchSnapshot, expression), sizeof(((UmiDebugWatchSnapshot *)0)->expression), 0 },
    {"value", offsetof(UmiDebugWatchSnapshot, value), sizeof(((UmiDebugWatchSnapshot *)0)->value), 0 },
    {"type", offsetof(UmiDebugWatchSnapshot, type), sizeof(((UmiDebugWatchSnapshot *)0)->type), 0 }
};
static int ContractSnapshotEqual(const UmiDebugWatchSnapshot *left,
    const UmiDebugWatchSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->session_id, right->session_id, sizeof(left->session_id)) == 0 &&
        memcmp(left->expression, right->expression, sizeof(left->expression)) == 0 &&
        memcmp(left->value, right->value, sizeof(left->value)) == 0 &&
        memcmp(left->type, right->type, sizeof(left->type)) == 0 &&
        left->enabled == right->enabled &&
        left->valid == right->valid &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiDebugWatchSnapshot *item)
{
    item->session_id[0] = 'v';
    item->expression[0] = 'v';
    item->value[0] = 'v';
    item->type[0] = 'v';
    item->enabled = (int)8U;
    item->valid = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_debug_watch_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_debug_watch_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
/* The previous binding selected the workspace handle's name. Retain it for
 * review while the active binding exercises the distinct registry value. */
#if 0
#define CONTRACT_EDIT UmiDebugWatchEdit
#endif
#define CONTRACT_EDIT UmiDebugWatchRegistryEdit
#define CONTRACT_EDIT_CURRENT umi_debug_watch_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_debug_watch_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiDebugWatchSnapshot ArchiveSample(void)
{
    UmiDebugWatchSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiDebugWatchSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
    {
        size_t used = strlen(value->expression) + 1U;
        memset(value->expression + used, 0xa5, sizeof(value->expression) - used);
    }
    {
        size_t used = strlen(value->value) + 1U;
        memset(value->value + used, 0xa5, sizeof(value->value) - used);
    }
    {
        size_t used = strlen(value->type) + 1U;
        memset(value->type + used, 0xa5, sizeof(value->type) - used);
    }
}
#define ARCHIVE_TYPE UmiDebugWatchSnapshot
#define ARCHIVE_ENCODE umi_debug_watch_snapshot_archive_encode
#define ARCHIVE_DECODE umi_debug_watch_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_debug_watch_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_debug_watch_registry_archive_restore
#include "snapshot_contract_cases.h"
