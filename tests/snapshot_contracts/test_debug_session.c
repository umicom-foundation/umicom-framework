/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_debug_session.c
 * PURPOSE: Exercise debug session snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/session.h"
#include "umicom/debug/session.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDebugSessionSnapshot
#define CONTRACT_REGISTRY UmiDebugSessionRegistry
#define CONTRACT_CAPACITY UMI_DEBUG_SESSION_CAPACITY
#define CONTRACT_VALIDATE umi_debug_session_snapshot_validate
#define CONTRACT_BATCH umi_debug_session_registry_upsert_many
#define CONTRACT_CREATE umi_debug_session_registry_create
#define CONTRACT_DESTROY umi_debug_session_registry_destroy
#define CONTRACT_UPSERT umi_debug_session_registry_upsert
#define CONTRACT_REMOVE umi_debug_session_registry_remove
#define CONTRACT_FIND umi_debug_session_registry_find
#define CONTRACT_AT umi_debug_session_registry_at
#define CONTRACT_COUNT umi_debug_session_registry_count
#define CONTRACT_REVISION umi_debug_session_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDebugSessionSnapshot, id), sizeof(((UmiDebugSessionSnapshot *)0)->id), 1 },
    {"configuration_id", offsetof(UmiDebugSessionSnapshot, configuration_id), sizeof(((UmiDebugSessionSnapshot *)0)->configuration_id), 0 },
    {"adapter", offsetof(UmiDebugSessionSnapshot, adapter), sizeof(((UmiDebugSessionSnapshot *)0)->adapter), 0 },
    {"state_text", offsetof(UmiDebugSessionSnapshot, state_text), sizeof(((UmiDebugSessionSnapshot *)0)->state_text), 0 }
};
static int ContractSnapshotEqual(const UmiDebugSessionSnapshot *left,
    const UmiDebugSessionSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->configuration_id, right->configuration_id, sizeof(left->configuration_id)) == 0 &&
        memcmp(left->adapter, right->adapter, sizeof(left->adapter)) == 0 &&
        memcmp(left->state_text, right->state_text, sizeof(left->state_text)) == 0 &&
        left->started_at == right->started_at &&
        left->state == right->state &&
        left->attached == right->attached &&
        left->supports_restart == right->supports_restart &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiDebugSessionSnapshot *item)
{
    item->configuration_id[0] = 'v';
    item->adapter[0] = 'v';
    item->state_text[0] = 'v';
    item->started_at = (uint64_t)7U;
    item->state = (int)8U;
    item->attached = (int)9U;
    item->supports_restart = (int)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_debug_session_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_debug_session_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiDebugSessionEdit
#define CONTRACT_EDIT_CURRENT umi_debug_session_registry_edit_if_current
#include "snapshot_contract_cases.h"
