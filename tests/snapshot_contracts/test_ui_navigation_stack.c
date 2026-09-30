/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_navigation_stack.c
 * PURPOSE: Exercise ui navigation_stack snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/navigation_stack.h"
#include "umicom/ui/navigation_stack.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiNavigationEntrySnapshot
#define CONTRACT_REGISTRY UmiUiNavigationEntryRegistry
#define CONTRACT_CAPACITY UMI_UI_NAVIGATION_STACK_CAPACITY
#define CONTRACT_VALIDATE umi_ui_navigation_stack_snapshot_validate
#define CONTRACT_BATCH umi_ui_navigation_stack_registry_upsert_many
#define CONTRACT_CREATE umi_ui_navigation_stack_registry_create
#define CONTRACT_DESTROY umi_ui_navigation_stack_registry_destroy
#define CONTRACT_UPSERT umi_ui_navigation_stack_registry_upsert
#define CONTRACT_REMOVE umi_ui_navigation_stack_registry_remove
#define CONTRACT_FIND umi_ui_navigation_stack_registry_find
#define CONTRACT_AT umi_ui_navigation_stack_registry_at
#define CONTRACT_COUNT umi_ui_navigation_stack_registry_count
#define CONTRACT_REVISION umi_ui_navigation_stack_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiNavigationEntrySnapshot, id), sizeof(((UmiUiNavigationEntrySnapshot *)0)->id), 1 },
    {"uri", offsetof(UmiUiNavigationEntrySnapshot, uri), sizeof(((UmiUiNavigationEntrySnapshot *)0)->uri), 0 },
    {"label", offsetof(UmiUiNavigationEntrySnapshot, label), sizeof(((UmiUiNavigationEntrySnapshot *)0)->label), 0 }
};
static int ContractSnapshotEqual(const UmiUiNavigationEntrySnapshot *left,
    const UmiUiNavigationEntrySnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->uri, right->uri, sizeof(left->uri)) == 0 &&
        memcmp(left->label, right->label, sizeof(left->label)) == 0 &&
        left->line == right->line &&
        left->column == right->column &&
        left->visited_at == right->visited_at &&
        left->current == right->current &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiNavigationEntrySnapshot *item)
{
    item->uri[0] = 'v';
    item->label[0] = 'v';
    item->line = (uint32_t)6U;
    item->column = (uint32_t)7U;
    item->visited_at = (uint64_t)8U;
    item->current = (int)9U;
}
#include "snapshot_contract_cases.h"
