/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_project_target.c
 * PURPOSE: Exercise project target snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/target.h"
#include "umicom/project/target.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProjectTargetSnapshot
#define CONTRACT_REGISTRY UmiProjectTargetRegistry
#define CONTRACT_CAPACITY UMI_PROJECT_TARGET_CAPACITY
#define CONTRACT_VALIDATE umi_project_target_snapshot_validate
#define CONTRACT_BATCH umi_project_target_registry_upsert_many
#define CONTRACT_CREATE umi_project_target_registry_create
#define CONTRACT_DESTROY umi_project_target_registry_destroy
#define CONTRACT_UPSERT umi_project_target_registry_upsert
#define CONTRACT_REMOVE umi_project_target_registry_remove
#define CONTRACT_FIND umi_project_target_registry_find
#define CONTRACT_AT umi_project_target_registry_at
#define CONTRACT_COUNT umi_project_target_registry_count
#define CONTRACT_REVISION umi_project_target_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProjectTargetSnapshot, id), sizeof(((UmiProjectTargetSnapshot *)0)->id), 1 },
    {"project_id", offsetof(UmiProjectTargetSnapshot, project_id), sizeof(((UmiProjectTargetSnapshot *)0)->project_id), 0 },
    {"name", offsetof(UmiProjectTargetSnapshot, name), sizeof(((UmiProjectTargetSnapshot *)0)->name), 0 },
    {"kind", offsetof(UmiProjectTargetSnapshot, kind), sizeof(((UmiProjectTargetSnapshot *)0)->kind), 0 },
    {"output_uri", offsetof(UmiProjectTargetSnapshot, output_uri), sizeof(((UmiProjectTargetSnapshot *)0)->output_uri), 0 }
};
static int ContractSnapshotEqual(const UmiProjectTargetSnapshot *left,
    const UmiProjectTargetSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->project_id, right->project_id, sizeof(left->project_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->output_uri, right->output_uri, sizeof(left->output_uri)) == 0 &&
        left->enabled == right->enabled &&
        left->default_target == right->default_target &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiProjectTargetSnapshot *item)
{
    item->project_id[0] = 'v';
    item->name[0] = 'v';
    item->kind[0] = 'v';
    item->output_uri[0] = 'v';
    item->enabled = (int)8U;
    item->default_target = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_project_target_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_project_target_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiProjectTargetEdit
#define CONTRACT_EDIT_CURRENT umi_project_target_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_project_target_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiProjectTargetSnapshot ArchiveSample(void)
{
    UmiProjectTargetSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiProjectTargetSnapshot *value)
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
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->kind) + 1U;
        memset(value->kind + used, 0xa5, sizeof(value->kind) - used);
    }
    {
        size_t used = strlen(value->output_uri) + 1U;
        memset(value->output_uri + used, 0xa5, sizeof(value->output_uri) - used);
    }
}
#define ARCHIVE_TYPE UmiProjectTargetSnapshot
#define ARCHIVE_ENCODE umi_project_target_snapshot_archive_encode
#define ARCHIVE_DECODE umi_project_target_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_project_target_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_project_target_registry_archive_restore
#include "snapshot_contract_cases.h"
