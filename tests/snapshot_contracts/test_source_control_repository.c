/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_source_control_repository.c
 * PURPOSE: Exercise source_control repository snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/source_control/repository.h"
#include "umicom/source_control/repository.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiSourceControlRepositorySnapshot
#define CONTRACT_REGISTRY UmiSourceControlRepositoryRegistry
#define CONTRACT_CAPACITY UMI_SOURCE_CONTROL_REPOSITORY_CAPACITY
#define CONTRACT_VALIDATE umi_source_control_repository_snapshot_validate
#define CONTRACT_BATCH umi_source_control_repository_registry_upsert_many
#define CONTRACT_CREATE umi_source_control_repository_registry_create
#define CONTRACT_DESTROY umi_source_control_repository_registry_destroy
#define CONTRACT_UPSERT umi_source_control_repository_registry_upsert
#define CONTRACT_REMOVE umi_source_control_repository_registry_remove
#define CONTRACT_FIND umi_source_control_repository_registry_find
#define CONTRACT_AT umi_source_control_repository_registry_at
#define CONTRACT_COUNT umi_source_control_repository_registry_count
#define CONTRACT_REVISION umi_source_control_repository_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiSourceControlRepositorySnapshot, id), sizeof(((UmiSourceControlRepositorySnapshot *)0)->id), 1 },
    {"root_uri", offsetof(UmiSourceControlRepositorySnapshot, root_uri), sizeof(((UmiSourceControlRepositorySnapshot *)0)->root_uri), 0 },
    {"provider", offsetof(UmiSourceControlRepositorySnapshot, provider), sizeof(((UmiSourceControlRepositorySnapshot *)0)->provider), 0 },
    {"branch", offsetof(UmiSourceControlRepositorySnapshot, branch), sizeof(((UmiSourceControlRepositorySnapshot *)0)->branch), 0 },
    {"head", offsetof(UmiSourceControlRepositorySnapshot, head), sizeof(((UmiSourceControlRepositorySnapshot *)0)->head), 0 }
};
static int ContractSnapshotEqual(const UmiSourceControlRepositorySnapshot *left,
    const UmiSourceControlRepositorySnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->root_uri, right->root_uri, sizeof(left->root_uri)) == 0 &&
        memcmp(left->provider, right->provider, sizeof(left->provider)) == 0 &&
        memcmp(left->branch, right->branch, sizeof(left->branch)) == 0 &&
        memcmp(left->head, right->head, sizeof(left->head)) == 0 &&
        left->clean == right->clean &&
        left->detached == right->detached &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiSourceControlRepositorySnapshot *item)
{
    item->root_uri[0] = 'v';
    item->provider[0] = 'v';
    item->branch[0] = 'v';
    item->head[0] = 'v';
    item->clean = (int)8U;
    item->detached = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_source_control_repository_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_source_control_repository_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiSourceControlRepositoryEdit
#define CONTRACT_EDIT_CURRENT umi_source_control_repository_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_source_control_repository_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiSourceControlRepositorySnapshot ArchiveSample(void)
{
    UmiSourceControlRepositorySnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiSourceControlRepositorySnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->root_uri) + 1U;
        memset(value->root_uri + used, 0xa5, sizeof(value->root_uri) - used);
    }
    {
        size_t used = strlen(value->provider) + 1U;
        memset(value->provider + used, 0xa5, sizeof(value->provider) - used);
    }
    {
        size_t used = strlen(value->branch) + 1U;
        memset(value->branch + used, 0xa5, sizeof(value->branch) - used);
    }
    {
        size_t used = strlen(value->head) + 1U;
        memset(value->head + used, 0xa5, sizeof(value->head) - used);
    }
}
#define ARCHIVE_TYPE UmiSourceControlRepositorySnapshot
#define ARCHIVE_ENCODE umi_source_control_repository_snapshot_archive_encode
#define ARCHIVE_DECODE umi_source_control_repository_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_source_control_repository_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_source_control_repository_registry_archive_restore
#include "snapshot_contract_cases.h"
