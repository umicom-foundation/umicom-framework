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
/* The previous binding selected the workspace handle's name. Retain it for
 * review while the active binding exercises the distinct registry value. */
#if 0
#define CONTRACT_EDIT UmiDebugBreakpointEdit
#endif
#define CONTRACT_EDIT UmiDebugBreakpointRegistryEdit
#define CONTRACT_EDIT_CURRENT umi_debug_breakpoint_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_debug_breakpoint_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiDebugBreakpointSnapshot ArchiveSample(void)
{
    UmiDebugBreakpointSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiDebugBreakpointSnapshot *value)
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
        size_t used = strlen(value->uri) + 1U;
        memset(value->uri + used, 0xa5, sizeof(value->uri) - used);
    }
    {
        size_t used = strlen(value->condition) + 1U;
        memset(value->condition + used, 0xa5, sizeof(value->condition) - used);
    }
    {
        size_t used = strlen(value->log_message) + 1U;
        memset(value->log_message + used, 0xa5, sizeof(value->log_message) - used);
    }
}
#define ARCHIVE_TYPE UmiDebugBreakpointSnapshot
#define ARCHIVE_ENCODE umi_debug_breakpoint_snapshot_archive_encode
#define ARCHIVE_DECODE umi_debug_breakpoint_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_debug_breakpoint_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_debug_breakpoint_registry_archive_restore
#include "snapshot_contract_cases.h"
