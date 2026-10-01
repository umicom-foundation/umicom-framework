/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_debug_source.c
 * PURPOSE: Exercise debug source snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/source.h"
#include "umicom/debug/source.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDebugSourceSnapshot
#define CONTRACT_REGISTRY UmiDebugSourceRegistry
#define CONTRACT_CAPACITY UMI_DEBUG_SOURCE_CAPACITY
#define CONTRACT_VALIDATE umi_debug_source_snapshot_validate
#define CONTRACT_BATCH umi_debug_source_registry_upsert_many
#define CONTRACT_CREATE umi_debug_source_registry_create
#define CONTRACT_DESTROY umi_debug_source_registry_destroy
#define CONTRACT_UPSERT umi_debug_source_registry_upsert
#define CONTRACT_REMOVE umi_debug_source_registry_remove
#define CONTRACT_FIND umi_debug_source_registry_find
#define CONTRACT_AT umi_debug_source_registry_at
#define CONTRACT_COUNT umi_debug_source_registry_count
#define CONTRACT_REVISION umi_debug_source_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDebugSourceSnapshot, id), sizeof(((UmiDebugSourceSnapshot *)0)->id), 1 },
    {"session_id", offsetof(UmiDebugSourceSnapshot, session_id), sizeof(((UmiDebugSourceSnapshot *)0)->session_id), 0 },
    {"uri", offsetof(UmiDebugSourceSnapshot, uri), sizeof(((UmiDebugSourceSnapshot *)0)->uri), 0 },
    {"name", offsetof(UmiDebugSourceSnapshot, name), sizeof(((UmiDebugSourceSnapshot *)0)->name), 0 }
};
static int ContractSnapshotEqual(const UmiDebugSourceSnapshot *left,
    const UmiDebugSourceSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->session_id, right->session_id, sizeof(left->session_id)) == 0 &&
        memcmp(left->uri, right->uri, sizeof(left->uri)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        left->source_reference == right->source_reference &&
        left->available == right->available &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiDebugSourceSnapshot *item)
{
    item->session_id[0] = 'v';
    item->uri[0] = 'v';
    item->name[0] = 'v';
    item->source_reference = (uint64_t)7U;
    item->available = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_debug_source_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_debug_source_registry_replace_if_current
#include "snapshot_contract_cases.h"
