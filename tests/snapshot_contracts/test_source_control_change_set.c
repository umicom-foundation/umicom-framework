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
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_source_control_change_set_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiSourceControlChangeSetSnapshot ArchiveSample(void)
{
    UmiSourceControlChangeSetSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiSourceControlChangeSetSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->repository_id) + 1U;
        memset(value->repository_id + used, 0xa5, sizeof(value->repository_id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->description) + 1U;
        memset(value->description + used, 0xa5, sizeof(value->description) - used);
    }
}
#define ARCHIVE_TYPE UmiSourceControlChangeSetSnapshot
#define ARCHIVE_ENCODE umi_source_control_change_set_snapshot_archive_encode
#define ARCHIVE_DECODE umi_source_control_change_set_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_source_control_change_set_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_source_control_change_set_registry_archive_restore
#include "snapshot_contract_cases.h"
