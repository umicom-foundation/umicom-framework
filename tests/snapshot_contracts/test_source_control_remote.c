/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_source_control_remote.c
 * PURPOSE: Exercise source_control remote snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/source_control/remote.h"
#include "umicom/source_control/remote.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiSourceControlRemoteSnapshot
#define CONTRACT_REGISTRY UmiSourceControlRemoteRegistry
#define CONTRACT_CAPACITY UMI_SOURCE_CONTROL_REMOTE_CAPACITY
#define CONTRACT_VALIDATE umi_source_control_remote_snapshot_validate
#define CONTRACT_BATCH umi_source_control_remote_registry_upsert_many
#define CONTRACT_CREATE umi_source_control_remote_registry_create
#define CONTRACT_DESTROY umi_source_control_remote_registry_destroy
#define CONTRACT_UPSERT umi_source_control_remote_registry_upsert
#define CONTRACT_REMOVE umi_source_control_remote_registry_remove
#define CONTRACT_FIND umi_source_control_remote_registry_find
#define CONTRACT_AT umi_source_control_remote_registry_at
#define CONTRACT_COUNT umi_source_control_remote_registry_count
#define CONTRACT_REVISION umi_source_control_remote_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiSourceControlRemoteSnapshot, id), sizeof(((UmiSourceControlRemoteSnapshot *)0)->id), 1 },
    {"repository_id", offsetof(UmiSourceControlRemoteSnapshot, repository_id), sizeof(((UmiSourceControlRemoteSnapshot *)0)->repository_id), 0 },
    {"name", offsetof(UmiSourceControlRemoteSnapshot, name), sizeof(((UmiSourceControlRemoteSnapshot *)0)->name), 0 },
    {"fetch_url", offsetof(UmiSourceControlRemoteSnapshot, fetch_url), sizeof(((UmiSourceControlRemoteSnapshot *)0)->fetch_url), 0 },
    {"push_url", offsetof(UmiSourceControlRemoteSnapshot, push_url), sizeof(((UmiSourceControlRemoteSnapshot *)0)->push_url), 0 }
};
static int ContractSnapshotEqual(const UmiSourceControlRemoteSnapshot *left,
    const UmiSourceControlRemoteSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->repository_id, right->repository_id, sizeof(left->repository_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->fetch_url, right->fetch_url, sizeof(left->fetch_url)) == 0 &&
        memcmp(left->push_url, right->push_url, sizeof(left->push_url)) == 0 &&
        left->default_remote == right->default_remote &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiSourceControlRemoteSnapshot *item)
{
    item->repository_id[0] = 'v';
    item->name[0] = 'v';
    item->fetch_url[0] = 'v';
    item->push_url[0] = 'v';
    item->default_remote = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_source_control_remote_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_source_control_remote_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiSourceControlRemoteEdit
#define CONTRACT_EDIT_CURRENT umi_source_control_remote_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_source_control_remote_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiSourceControlRemoteSnapshot ArchiveSample(void)
{
    UmiSourceControlRemoteSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiSourceControlRemoteSnapshot *value)
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
        size_t used = strlen(value->fetch_url) + 1U;
        memset(value->fetch_url + used, 0xa5, sizeof(value->fetch_url) - used);
    }
    {
        size_t used = strlen(value->push_url) + 1U;
        memset(value->push_url + used, 0xa5, sizeof(value->push_url) - used);
    }
}
#define ARCHIVE_TYPE UmiSourceControlRemoteSnapshot
#define ARCHIVE_ENCODE umi_source_control_remote_snapshot_archive_encode
#define ARCHIVE_DECODE umi_source_control_remote_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_source_control_remote_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_source_control_remote_registry_archive_restore
#include "snapshot_contract_cases.h"
