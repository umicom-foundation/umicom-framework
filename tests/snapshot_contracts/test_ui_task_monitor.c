/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_task_monitor.c
 * PURPOSE: Exercise ui task_monitor snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/task_monitor.h"
#include "umicom/ui/task_monitor.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiTaskMonitorSnapshot
#define CONTRACT_REGISTRY UmiUiTaskMonitorRegistry
#define CONTRACT_CAPACITY UMI_UI_TASK_MONITOR_CAPACITY
#define CONTRACT_VALIDATE umi_ui_task_monitor_snapshot_validate
#define CONTRACT_BATCH umi_ui_task_monitor_registry_upsert_many
#define CONTRACT_CREATE umi_ui_task_monitor_registry_create
#define CONTRACT_DESTROY umi_ui_task_monitor_registry_destroy
#define CONTRACT_UPSERT umi_ui_task_monitor_registry_upsert
#define CONTRACT_REMOVE umi_ui_task_monitor_registry_remove
#define CONTRACT_FIND umi_ui_task_monitor_registry_find
#define CONTRACT_AT umi_ui_task_monitor_registry_at
#define CONTRACT_COUNT umi_ui_task_monitor_registry_count
#define CONTRACT_REVISION umi_ui_task_monitor_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiTaskMonitorSnapshot, id), sizeof(((UmiUiTaskMonitorSnapshot *)0)->id), 1 },
    {"label", offsetof(UmiUiTaskMonitorSnapshot, label), sizeof(((UmiUiTaskMonitorSnapshot *)0)->label), 0 },
    {"group", offsetof(UmiUiTaskMonitorSnapshot, group), sizeof(((UmiUiTaskMonitorSnapshot *)0)->group), 0 },
    {"detail", offsetof(UmiUiTaskMonitorSnapshot, detail), sizeof(((UmiUiTaskMonitorSnapshot *)0)->detail), 0 }
};
static int ContractSnapshotEqual(const UmiUiTaskMonitorSnapshot *left,
    const UmiUiTaskMonitorSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->label, right->label, sizeof(left->label)) == 0 &&
        memcmp(left->group, right->group, sizeof(left->group)) == 0 &&
        memcmp(left->detail, right->detail, sizeof(left->detail)) == 0 &&
        left->started_at == right->started_at &&
        left->finished_at == right->finished_at &&
        left->state == right->state &&
        left->background == right->background &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiTaskMonitorSnapshot *item)
{
    item->label[0] = 'v';
    item->group[0] = 'v';
    item->detail[0] = 'v';
    item->started_at = (uint64_t)7U;
    item->finished_at = (uint64_t)8U;
    item->state = (int)9U;
    item->background = (int)10U;
}
#include "snapshot_contract_cases.h"
