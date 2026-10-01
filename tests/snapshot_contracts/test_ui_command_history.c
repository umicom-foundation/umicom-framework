/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_command_history.c
 * PURPOSE: Exercise ui command_history snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/command_history.h"
#include "umicom/ui/command_history.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiCommandHistorySnapshot
#define CONTRACT_REGISTRY UmiUiCommandHistoryRegistry
#define CONTRACT_CAPACITY UMI_UI_COMMAND_HISTORY_CAPACITY
#define CONTRACT_VALIDATE umi_ui_command_history_snapshot_validate
#define CONTRACT_BATCH umi_ui_command_history_registry_upsert_many
#define CONTRACT_CREATE umi_ui_command_history_registry_create
#define CONTRACT_DESTROY umi_ui_command_history_registry_destroy
#define CONTRACT_UPSERT umi_ui_command_history_registry_upsert
#define CONTRACT_REMOVE umi_ui_command_history_registry_remove
#define CONTRACT_FIND umi_ui_command_history_registry_find
#define CONTRACT_AT umi_ui_command_history_registry_at
#define CONTRACT_COUNT umi_ui_command_history_registry_count
#define CONTRACT_REVISION umi_ui_command_history_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiCommandHistorySnapshot, id), sizeof(((UmiUiCommandHistorySnapshot *)0)->id), 1 },
    {"command_id", offsetof(UmiUiCommandHistorySnapshot, command_id), sizeof(((UmiUiCommandHistorySnapshot *)0)->command_id), 0 },
    {"argument", offsetof(UmiUiCommandHistorySnapshot, argument), sizeof(((UmiUiCommandHistorySnapshot *)0)->argument), 0 },
    {"source", offsetof(UmiUiCommandHistorySnapshot, source), sizeof(((UmiUiCommandHistorySnapshot *)0)->source), 0 }
};
static int ContractSnapshotEqual(const UmiUiCommandHistorySnapshot *left,
    const UmiUiCommandHistorySnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->command_id, right->command_id, sizeof(left->command_id)) == 0 &&
        memcmp(left->argument, right->argument, sizeof(left->argument)) == 0 &&
        memcmp(left->source, right->source, sizeof(left->source)) == 0 &&
        left->executed_at == right->executed_at &&
        left->outcome == right->outcome &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiCommandHistorySnapshot *item)
{
    item->command_id[0] = 'v';
    item->argument[0] = 'v';
    item->source[0] = 'v';
    item->executed_at = (uint64_t)7U;
    item->outcome = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_command_history_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_command_history_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiCommandHistoryEdit
#define CONTRACT_EDIT_CURRENT umi_ui_command_history_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_ui_command_history_registry_read_page
#include "snapshot_contract_cases.h"
