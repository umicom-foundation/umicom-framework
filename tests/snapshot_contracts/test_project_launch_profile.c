/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_project_launch_profile.c
 * PURPOSE: Exercise project launch_profile snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/launch_profile.h"
#include "umicom/project/launch_profile.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProjectLaunchProfileSnapshot
#define CONTRACT_REGISTRY UmiProjectLaunchProfileRegistry
#define CONTRACT_CAPACITY UMI_PROJECT_LAUNCH_PROFILE_CAPACITY
#define CONTRACT_VALIDATE umi_project_launch_profile_snapshot_validate
#define CONTRACT_BATCH umi_project_launch_profile_registry_upsert_many
#define CONTRACT_CREATE umi_project_launch_profile_registry_create
#define CONTRACT_DESTROY umi_project_launch_profile_registry_destroy
#define CONTRACT_UPSERT umi_project_launch_profile_registry_upsert
#define CONTRACT_REMOVE umi_project_launch_profile_registry_remove
#define CONTRACT_FIND umi_project_launch_profile_registry_find
#define CONTRACT_AT umi_project_launch_profile_registry_at
#define CONTRACT_COUNT umi_project_launch_profile_registry_count
#define CONTRACT_REVISION umi_project_launch_profile_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProjectLaunchProfileSnapshot, id), sizeof(((UmiProjectLaunchProfileSnapshot *)0)->id), 1 },
    {"project_id", offsetof(UmiProjectLaunchProfileSnapshot, project_id), sizeof(((UmiProjectLaunchProfileSnapshot *)0)->project_id), 0 },
    {"name", offsetof(UmiProjectLaunchProfileSnapshot, name), sizeof(((UmiProjectLaunchProfileSnapshot *)0)->name), 0 },
    {"program", offsetof(UmiProjectLaunchProfileSnapshot, program), sizeof(((UmiProjectLaunchProfileSnapshot *)0)->program), 0 },
    {"arguments", offsetof(UmiProjectLaunchProfileSnapshot, arguments), sizeof(((UmiProjectLaunchProfileSnapshot *)0)->arguments), 0 },
    {"working_directory", offsetof(UmiProjectLaunchProfileSnapshot, working_directory), sizeof(((UmiProjectLaunchProfileSnapshot *)0)->working_directory), 0 },
    {"environment_id", offsetof(UmiProjectLaunchProfileSnapshot, environment_id), sizeof(((UmiProjectLaunchProfileSnapshot *)0)->environment_id), 0 }
};
static int ContractSnapshotEqual(const UmiProjectLaunchProfileSnapshot *left,
    const UmiProjectLaunchProfileSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->project_id, right->project_id, sizeof(left->project_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->program, right->program, sizeof(left->program)) == 0 &&
        memcmp(left->arguments, right->arguments, sizeof(left->arguments)) == 0 &&
        memcmp(left->working_directory, right->working_directory, sizeof(left->working_directory)) == 0 &&
        memcmp(left->environment_id, right->environment_id, sizeof(left->environment_id)) == 0 &&
        left->debug == right->debug &&
        left->default_profile == right->default_profile &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiProjectLaunchProfileSnapshot *item)
{
    item->project_id[0] = 'v';
    item->name[0] = 'v';
    item->program[0] = 'v';
    item->arguments[0] = 'v';
    item->working_directory[0] = 'v';
    item->environment_id[0] = 'v';
    item->debug = (int)10U;
    item->default_profile = (int)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_project_launch_profile_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_project_launch_profile_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiProjectLaunchProfileEdit
#define CONTRACT_EDIT_CURRENT umi_project_launch_profile_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_project_launch_profile_registry_read_page
#include "snapshot_contract_cases.h"
