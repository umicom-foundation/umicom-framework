/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_debug_console_entry.c
 * PURPOSE: Exercise debug console_entry snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/console_entry.h"
#include "umicom/debug/console_entry.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDebugConsoleEntrySnapshot
#define CONTRACT_REGISTRY UmiDebugConsoleEntryRegistry
#define CONTRACT_CAPACITY UMI_DEBUG_CONSOLE_ENTRY_CAPACITY
#define CONTRACT_VALIDATE umi_debug_console_entry_snapshot_validate
#define CONTRACT_BATCH umi_debug_console_entry_registry_upsert_many
#define CONTRACT_CREATE umi_debug_console_entry_registry_create
#define CONTRACT_DESTROY umi_debug_console_entry_registry_destroy
#define CONTRACT_UPSERT umi_debug_console_entry_registry_upsert
#define CONTRACT_REMOVE umi_debug_console_entry_registry_remove
#define CONTRACT_FIND umi_debug_console_entry_registry_find
#define CONTRACT_AT umi_debug_console_entry_registry_at
#define CONTRACT_COUNT umi_debug_console_entry_registry_count
#define CONTRACT_REVISION umi_debug_console_entry_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDebugConsoleEntrySnapshot, id), sizeof(((UmiDebugConsoleEntrySnapshot *)0)->id), 1 },
    {"session_id", offsetof(UmiDebugConsoleEntrySnapshot, session_id), sizeof(((UmiDebugConsoleEntrySnapshot *)0)->session_id), 0 },
    {"category", offsetof(UmiDebugConsoleEntrySnapshot, category), sizeof(((UmiDebugConsoleEntrySnapshot *)0)->category), 0 },
    {"text", offsetof(UmiDebugConsoleEntrySnapshot, text), sizeof(((UmiDebugConsoleEntrySnapshot *)0)->text), 0 }
};
static int ContractSnapshotEqual(const UmiDebugConsoleEntrySnapshot *left,
    const UmiDebugConsoleEntrySnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->session_id, right->session_id, sizeof(left->session_id)) == 0 &&
        memcmp(left->category, right->category, sizeof(left->category)) == 0 &&
        memcmp(left->text, right->text, sizeof(left->text)) == 0 &&
        left->timestamp == right->timestamp &&
        left->severity == right->severity &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiDebugConsoleEntrySnapshot *item)
{
    item->session_id[0] = 'v';
    item->category[0] = 'v';
    item->text[0] = 'v';
    item->timestamp = (uint64_t)7U;
    item->severity = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_debug_console_entry_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_debug_console_entry_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiDebugConsoleEntryEdit
#define CONTRACT_EDIT_CURRENT umi_debug_console_entry_registry_edit_if_current
#include "snapshot_contract_cases.h"
