/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_debug_exception.c
 * PURPOSE: Exercise debug exception snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/exception.h"
#include "umicom/debug/exception.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDebugExceptionSnapshot
#define CONTRACT_REGISTRY UmiDebugExceptionRegistry
#define CONTRACT_CAPACITY UMI_DEBUG_EXCEPTION_CAPACITY
#define CONTRACT_VALIDATE umi_debug_exception_snapshot_validate
#define CONTRACT_BATCH umi_debug_exception_registry_upsert_many
#define CONTRACT_CREATE umi_debug_exception_registry_create
#define CONTRACT_DESTROY umi_debug_exception_registry_destroy
#define CONTRACT_UPSERT umi_debug_exception_registry_upsert
#define CONTRACT_REMOVE umi_debug_exception_registry_remove
#define CONTRACT_FIND umi_debug_exception_registry_find
#define CONTRACT_AT umi_debug_exception_registry_at
#define CONTRACT_COUNT umi_debug_exception_registry_count
#define CONTRACT_REVISION umi_debug_exception_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDebugExceptionSnapshot, id), sizeof(((UmiDebugExceptionSnapshot *)0)->id), 1 },
    {"session_id", offsetof(UmiDebugExceptionSnapshot, session_id), sizeof(((UmiDebugExceptionSnapshot *)0)->session_id), 0 },
    {"type", offsetof(UmiDebugExceptionSnapshot, type), sizeof(((UmiDebugExceptionSnapshot *)0)->type), 0 },
    {"message", offsetof(UmiDebugExceptionSnapshot, message), sizeof(((UmiDebugExceptionSnapshot *)0)->message), 0 },
    {"break_mode", offsetof(UmiDebugExceptionSnapshot, break_mode), sizeof(((UmiDebugExceptionSnapshot *)0)->break_mode), 0 }
};
static int ContractSnapshotEqual(const UmiDebugExceptionSnapshot *left,
    const UmiDebugExceptionSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->session_id, right->session_id, sizeof(left->session_id)) == 0 &&
        memcmp(left->type, right->type, sizeof(left->type)) == 0 &&
        memcmp(left->message, right->message, sizeof(left->message)) == 0 &&
        memcmp(left->break_mode, right->break_mode, sizeof(left->break_mode)) == 0 &&
        left->caught == right->caught &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiDebugExceptionSnapshot *item)
{
    item->session_id[0] = 'v';
    item->type[0] = 'v';
    item->message[0] = 'v';
    item->break_mode[0] = 'v';
    item->caught = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_debug_exception_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_debug_exception_registry_replace_if_current
#include "snapshot_contract_cases.h"
