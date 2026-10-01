/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_debug_breakpoint.c
 * PURPOSE: Exercise debug breakpoint snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/breakpoint.h"
#include "umicom/debug/breakpoint.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDebugBreakpointSnapshot
#define CONTRACT_REGISTRY UmiDebugBreakpointRegistry
#define CONTRACT_CAPACITY UMI_DEBUG_BREAKPOINT_CAPACITY
#define CONTRACT_VALIDATE umi_debug_breakpoint_snapshot_validate
#define CONTRACT_BATCH umi_debug_breakpoint_registry_upsert_many
#define CONTRACT_CREATE umi_debug_breakpoint_registry_create
#define CONTRACT_DESTROY umi_debug_breakpoint_registry_destroy
#define CONTRACT_UPSERT umi_debug_breakpoint_registry_upsert
#define CONTRACT_REMOVE umi_debug_breakpoint_registry_remove
#define CONTRACT_FIND umi_debug_breakpoint_registry_find
#define CONTRACT_AT umi_debug_breakpoint_registry_at
#define CONTRACT_COUNT umi_debug_breakpoint_registry_count
#define CONTRACT_REVISION umi_debug_breakpoint_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDebugBreakpointSnapshot, id), sizeof(((UmiDebugBreakpointSnapshot *)0)->id), 1 },
    {"session_id", offsetof(UmiDebugBreakpointSnapshot, session_id), sizeof(((UmiDebugBreakpointSnapshot *)0)->session_id), 0 },
    {"uri", offsetof(UmiDebugBreakpointSnapshot, uri), sizeof(((UmiDebugBreakpointSnapshot *)0)->uri), 0 },
    {"condition", offsetof(UmiDebugBreakpointSnapshot, condition), sizeof(((UmiDebugBreakpointSnapshot *)0)->condition), 0 },
    {"log_message", offsetof(UmiDebugBreakpointSnapshot, log_message), sizeof(((UmiDebugBreakpointSnapshot *)0)->log_message), 0 }
};
static int ContractSnapshotEqual(const UmiDebugBreakpointSnapshot *left,
    const UmiDebugBreakpointSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->session_id, right->session_id, sizeof(left->session_id)) == 0 &&
        memcmp(left->uri, right->uri, sizeof(left->uri)) == 0 &&
        memcmp(left->condition, right->condition, sizeof(left->condition)) == 0 &&
        memcmp(left->log_message, right->log_message, sizeof(left->log_message)) == 0 &&
        left->line == right->line &&
        left->column == right->column &&
        left->enabled == right->enabled &&
        left->verified == right->verified &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiDebugBreakpointSnapshot *item)
{
    item->session_id[0] = 'v';
    item->uri[0] = 'v';
    item->condition[0] = 'v';
    item->log_message[0] = 'v';
    item->line = (uint32_t)8U;
    item->column = (uint32_t)9U;
    item->enabled = (int)10U;
    item->verified = (int)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_debug_breakpoint_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_debug_breakpoint_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiDebugBreakpointEdit
#define CONTRACT_EDIT_CURRENT umi_debug_breakpoint_registry_edit_if_current
#include "snapshot_contract_cases.h"
