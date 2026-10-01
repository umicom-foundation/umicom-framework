/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_debug_event.c
 * PURPOSE: Exercise debug event snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/event.h"
#include "umicom/debug/event.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDebugEventSnapshot
#define CONTRACT_REGISTRY UmiDebugEventRegistry
#define CONTRACT_CAPACITY UMI_DEBUG_EVENT_CAPACITY
#define CONTRACT_VALIDATE umi_debug_event_snapshot_validate
#define CONTRACT_BATCH umi_debug_event_registry_upsert_many
#define CONTRACT_CREATE umi_debug_event_registry_create
#define CONTRACT_DESTROY umi_debug_event_registry_destroy
#define CONTRACT_UPSERT umi_debug_event_registry_upsert
#define CONTRACT_REMOVE umi_debug_event_registry_remove
#define CONTRACT_FIND umi_debug_event_registry_find
#define CONTRACT_AT umi_debug_event_registry_at
#define CONTRACT_COUNT umi_debug_event_registry_count
#define CONTRACT_REVISION umi_debug_event_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDebugEventSnapshot, id), sizeof(((UmiDebugEventSnapshot *)0)->id), 1 },
    {"session_id", offsetof(UmiDebugEventSnapshot, session_id), sizeof(((UmiDebugEventSnapshot *)0)->session_id), 0 },
    {"kind", offsetof(UmiDebugEventSnapshot, kind), sizeof(((UmiDebugEventSnapshot *)0)->kind), 0 },
    {"detail", offsetof(UmiDebugEventSnapshot, detail), sizeof(((UmiDebugEventSnapshot *)0)->detail), 0 }
};
static int ContractSnapshotEqual(const UmiDebugEventSnapshot *left,
    const UmiDebugEventSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->session_id, right->session_id, sizeof(left->session_id)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->detail, right->detail, sizeof(left->detail)) == 0 &&
        left->timestamp == right->timestamp &&
        left->important == right->important &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiDebugEventSnapshot *item)
{
    item->session_id[0] = 'v';
    item->kind[0] = 'v';
    item->detail[0] = 'v';
    item->timestamp = (uint64_t)7U;
    item->important = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_debug_event_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_debug_event_registry_replace_if_current
#include "snapshot_contract_cases.h"
