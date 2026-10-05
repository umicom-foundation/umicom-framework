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
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_project_task_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_project_task_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiProjectTaskEdit
#define CONTRACT_EDIT_CURRENT umi_project_task_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_project_task_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiProjectTaskSnapshot ArchiveSample(void)
{
    UmiProjectTaskSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiProjectTaskSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->project_id) + 1U;
        memset(value->project_id + used, 0xa5, sizeof(value->project_id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
    {
        size_t used = strlen(value->command) + 1U;
        memset(value->command + used, 0xa5, sizeof(value->command) - used);
    }
    {
        size_t used = strlen(value->working_directory) + 1U;
        memset(value->working_directory + used, 0xa5, sizeof(value->working_directory) - used);
    }
    {
        size_t used = strlen(value->group) + 1U;
        memset(value->group + used, 0xa5, sizeof(value->group) - used);
    }
}
#define ARCHIVE_TYPE UmiProjectTaskSnapshot
#define ARCHIVE_ENCODE umi_project_task_snapshot_archive_encode
#define ARCHIVE_DECODE umi_project_task_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_project_task_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_project_task_registry_archive_restore
#include "snapshot_contract_cases.h"
