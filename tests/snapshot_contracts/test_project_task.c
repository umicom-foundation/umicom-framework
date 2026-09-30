/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_project_task.c
 * PURPOSE: Exercise project task snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/task.h"
#include "umicom/project/task.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProjectTaskSnapshot
#define CONTRACT_REGISTRY UmiProjectTaskRegistry
#define CONTRACT_CAPACITY UMI_PROJECT_TASK_CAPACITY
#define CONTRACT_VALIDATE umi_project_task_snapshot_validate
#define CONTRACT_BATCH umi_project_task_registry_upsert_many
#define CONTRACT_CREATE umi_project_task_registry_create
#define CONTRACT_DESTROY umi_project_task_registry_destroy
#define CONTRACT_UPSERT umi_project_task_registry_upsert
#define CONTRACT_REMOVE umi_project_task_registry_remove
#define CONTRACT_FIND umi_project_task_registry_find
#define CONTRACT_AT umi_project_task_registry_at
#define CONTRACT_COUNT umi_project_task_registry_count
#define CONTRACT_REVISION umi_project_task_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProjectTaskSnapshot, id), sizeof(((UmiProjectTaskSnapshot *)0)->id), 1 },
    {"project_id", offsetof(UmiProjectTaskSnapshot, project_id), sizeof(((UmiProjectTaskSnapshot *)0)->project_id), 0 },
    {"label", offsetof(UmiProjectTaskSnapshot, label), sizeof(((UmiProjectTaskSnapshot *)0)->label), 0 },
    {"command", offsetof(UmiProjectTaskSnapshot, command), sizeof(((UmiProjectTaskSnapshot *)0)->command), 0 },
    {"working_directory", offsetof(UmiProjectTaskSnapshot, working_directory), sizeof(((UmiProjectTaskSnapshot *)0)->working_directory), 0 },
    {"group", offsetof(UmiProjectTaskSnapshot, group), sizeof(((UmiProjectTaskSnapshot *)0)->group), 0 }
};
static int ContractSnapshotEqual(const UmiProjectTaskSnapshot *left,
    const UmiProjectTaskSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->project_id, right->project_id, sizeof(left->project_id)) == 0 &&
        memcmp(left->label, right->label, sizeof(left->label)) == 0 &&
        memcmp(left->command, right->command, sizeof(left->command)) == 0 &&
        memcmp(left->working_directory, right->working_directory, sizeof(left->working_directory)) == 0 &&
        memcmp(left->group, right->group, sizeof(left->group)) == 0 &&
        left->default_task == right->default_task &&
        left->background == right->background &&
        left->enabled == right->enabled &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiProjectTaskSnapshot *item)
{
    item->project_id[0] = 'v';
    item->label[0] = 'v';
    item->command[0] = 'v';
    item->working_directory[0] = 'v';
    item->group[0] = 'v';
    item->default_task = (int)9U;
    item->background = (int)10U;
    item->enabled = (int)11U;
}
#include "snapshot_contract_cases.h"
