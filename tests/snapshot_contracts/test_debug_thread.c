/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_debug_thread.c
 * PURPOSE: Exercise debug thread snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/thread.h"
#include "umicom/debug/thread.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDebugThreadSnapshot
#define CONTRACT_REGISTRY UmiDebugThreadRegistry
#define CONTRACT_CAPACITY UMI_DEBUG_THREAD_CAPACITY
#define CONTRACT_VALIDATE umi_debug_thread_snapshot_validate
#define CONTRACT_BATCH umi_debug_thread_registry_upsert_many
#define CONTRACT_CREATE umi_debug_thread_registry_create
#define CONTRACT_DESTROY umi_debug_thread_registry_destroy
#define CONTRACT_UPSERT umi_debug_thread_registry_upsert
#define CONTRACT_REMOVE umi_debug_thread_registry_remove
#define CONTRACT_FIND umi_debug_thread_registry_find
#define CONTRACT_AT umi_debug_thread_registry_at
#define CONTRACT_COUNT umi_debug_thread_registry_count
#define CONTRACT_REVISION umi_debug_thread_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDebugThreadSnapshot, id), sizeof(((UmiDebugThreadSnapshot *)0)->id), 1 },
    {"session_id", offsetof(UmiDebugThreadSnapshot, session_id), sizeof(((UmiDebugThreadSnapshot *)0)->session_id), 0 },
    {"name", offsetof(UmiDebugThreadSnapshot, name), sizeof(((UmiDebugThreadSnapshot *)0)->name), 0 },
    {"detail", offsetof(UmiDebugThreadSnapshot, detail), sizeof(((UmiDebugThreadSnapshot *)0)->detail), 0 }
};
static int ContractSnapshotEqual(const UmiDebugThreadSnapshot *left,
    const UmiDebugThreadSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->session_id, right->session_id, sizeof(left->session_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->detail, right->detail, sizeof(left->detail)) == 0 &&
        left->native_id == right->native_id &&
        left->stopped == right->stopped &&
        left->current == right->current &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiDebugThreadSnapshot *item)
{
    item->session_id[0] = 'v';
    item->name[0] = 'v';
    item->detail[0] = 'v';
    item->native_id = (uint64_t)7U;
    item->stopped = (int)8U;
    item->current = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_debug_thread_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_debug_thread_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiDebugThreadEdit
#define CONTRACT_EDIT_CURRENT umi_debug_thread_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_debug_thread_registry_read_page
#include "snapshot_contract_cases.h"
