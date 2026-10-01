/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_source_control_change_set.c
 * PURPOSE: Exercise source_control change_set snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/source_control/change_set.h"
#include "umicom/source_control/change_set.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiSourceControlChangeSetSnapshot
#define CONTRACT_REGISTRY UmiSourceControlChangeSetRegistry
#define CONTRACT_CAPACITY UMI_SOURCE_CONTROL_CHANGE_SET_CAPACITY
#define CONTRACT_VALIDATE umi_source_control_change_set_snapshot_validate
#define CONTRACT_BATCH umi_source_control_change_set_registry_upsert_many
#define CONTRACT_CREATE umi_source_control_change_set_registry_create
#define CONTRACT_DESTROY umi_source_control_change_set_registry_destroy
#define CONTRACT_UPSERT umi_source_control_change_set_registry_upsert
#define CONTRACT_REMOVE umi_source_control_change_set_registry_remove
#define CONTRACT_FIND umi_source_control_change_set_registry_find
#define CONTRACT_AT umi_source_control_change_set_registry_at
#define CONTRACT_COUNT umi_source_control_change_set_registry_count
#define CONTRACT_REVISION umi_source_control_change_set_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiSourceControlChangeSetSnapshot, id), sizeof(((UmiSourceControlChangeSetSnapshot *)0)->id), 1 },
    {"repository_id", offsetof(UmiSourceControlChangeSetSnapshot, repository_id), sizeof(((UmiSourceControlChangeSetSnapshot *)0)->repository_id), 0 },
    {"name", offsetof(UmiSourceControlChangeSetSnapshot, name), sizeof(((UmiSourceControlChangeSetSnapshot *)0)->name), 0 },
    {"description", offsetof(UmiSourceControlChangeSetSnapshot, description), sizeof(((UmiSourceControlChangeSetSnapshot *)0)->description), 0 }
};
static int ContractSnapshotEqual(const UmiSourceControlChangeSetSnapshot *left,
    const UmiSourceControlChangeSetSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->repository_id, right->repository_id, sizeof(left->repository_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->description, right->description, sizeof(left->description)) == 0 &&
        left->change_count == right->change_count &&
        left->active == right->active &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiSourceControlChangeSetSnapshot *item)
{
    item->repository_id[0] = 'v';
    item->name[0] = 'v';
    item->description[0] = 'v';
    item->change_count = (size_t)7U;
    item->active = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_source_control_change_set_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_source_control_change_set_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiSourceControlChangeSetEdit
#define CONTRACT_EDIT_CURRENT umi_source_control_change_set_registry_edit_if_current
#include "snapshot_contract_cases.h"
