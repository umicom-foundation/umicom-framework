/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_source_control_history_entry.c
 * PURPOSE: Exercise source_control history_entry snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/source_control/history_entry.h"
#include "umicom/source_control/history_entry.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiSourceControlHistoryEntrySnapshot
#define CONTRACT_REGISTRY UmiSourceControlHistoryEntryRegistry
#define CONTRACT_CAPACITY UMI_SOURCE_CONTROL_HISTORY_ENTRY_CAPACITY
#define CONTRACT_VALIDATE umi_source_control_history_entry_snapshot_validate
#define CONTRACT_BATCH umi_source_control_history_entry_registry_upsert_many
#define CONTRACT_CREATE umi_source_control_history_entry_registry_create
#define CONTRACT_DESTROY umi_source_control_history_entry_registry_destroy
#define CONTRACT_UPSERT umi_source_control_history_entry_registry_upsert
#define CONTRACT_REMOVE umi_source_control_history_entry_registry_remove
#define CONTRACT_FIND umi_source_control_history_entry_registry_find
#define CONTRACT_AT umi_source_control_history_entry_registry_at
#define CONTRACT_COUNT umi_source_control_history_entry_registry_count
#define CONTRACT_REVISION umi_source_control_history_entry_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiSourceControlHistoryEntrySnapshot, id), sizeof(((UmiSourceControlHistoryEntrySnapshot *)0)->id), 1 },
    {"repository_id", offsetof(UmiSourceControlHistoryEntrySnapshot, repository_id), sizeof(((UmiSourceControlHistoryEntrySnapshot *)0)->repository_id), 0 },
    {"revision_id", offsetof(UmiSourceControlHistoryEntrySnapshot, revision_id), sizeof(((UmiSourceControlHistoryEntrySnapshot *)0)->revision_id), 0 },
    {"summary", offsetof(UmiSourceControlHistoryEntrySnapshot, summary), sizeof(((UmiSourceControlHistoryEntrySnapshot *)0)->summary), 0 },
    {"author", offsetof(UmiSourceControlHistoryEntrySnapshot, author), sizeof(((UmiSourceControlHistoryEntrySnapshot *)0)->author), 0 }
};
static int ContractSnapshotEqual(const UmiSourceControlHistoryEntrySnapshot *left,
    const UmiSourceControlHistoryEntrySnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->repository_id, right->repository_id, sizeof(left->repository_id)) == 0 &&
        memcmp(left->revision_id, right->revision_id, sizeof(left->revision_id)) == 0 &&
        memcmp(left->summary, right->summary, sizeof(left->summary)) == 0 &&
        memcmp(left->author, right->author, sizeof(left->author)) == 0 &&
        left->timestamp == right->timestamp &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiSourceControlHistoryEntrySnapshot *item)
{
    item->repository_id[0] = 'v';
    item->revision_id[0] = 'v';
    item->summary[0] = 'v';
    item->author[0] = 'v';
    item->timestamp = (uint64_t)8U;
    item->order = (int32_t)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_source_control_history_entry_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_source_control_history_entry_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiSourceControlHistoryEntryEdit
#define CONTRACT_EDIT_CURRENT umi_source_control_history_entry_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_source_control_history_entry_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiSourceControlHistoryEntrySnapshot ArchiveSample(void)
{
    UmiSourceControlHistoryEntrySnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiSourceControlHistoryEntrySnapshot *value)
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
        size_t used = strlen(value->revision_id) + 1U;
        memset(value->revision_id + used, 0xa5, sizeof(value->revision_id) - used);
    }
    {
        size_t used = strlen(value->summary) + 1U;
        memset(value->summary + used, 0xa5, sizeof(value->summary) - used);
    }
    {
        size_t used = strlen(value->author) + 1U;
        memset(value->author + used, 0xa5, sizeof(value->author) - used);
    }
}
#define ARCHIVE_TYPE UmiSourceControlHistoryEntrySnapshot
#define ARCHIVE_ENCODE umi_source_control_history_entry_snapshot_archive_encode
#define ARCHIVE_DECODE umi_source_control_history_entry_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_source_control_history_entry_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_source_control_history_entry_registry_archive_restore
#include "snapshot_contract_cases.h"
