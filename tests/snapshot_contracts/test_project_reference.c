/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_project_reference.c
 * PURPOSE: Exercise project reference snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/reference.h"
#include "umicom/project/reference.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProjectReferenceSnapshot
#define CONTRACT_REGISTRY UmiProjectReferenceRegistry
#define CONTRACT_CAPACITY UMI_PROJECT_REFERENCE_CAPACITY
#define CONTRACT_VALIDATE umi_project_reference_snapshot_validate
#define CONTRACT_BATCH umi_project_reference_registry_upsert_many
#define CONTRACT_CREATE umi_project_reference_registry_create
#define CONTRACT_DESTROY umi_project_reference_registry_destroy
#define CONTRACT_UPSERT umi_project_reference_registry_upsert
#define CONTRACT_REMOVE umi_project_reference_registry_remove
#define CONTRACT_FIND umi_project_reference_registry_find
#define CONTRACT_AT umi_project_reference_registry_at
#define CONTRACT_COUNT umi_project_reference_registry_count
#define CONTRACT_REVISION umi_project_reference_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProjectReferenceSnapshot, id), sizeof(((UmiProjectReferenceSnapshot *)0)->id), 1 },
    {"project_id", offsetof(UmiProjectReferenceSnapshot, project_id), sizeof(((UmiProjectReferenceSnapshot *)0)->project_id), 0 },
    {"target_project_id", offsetof(UmiProjectReferenceSnapshot, target_project_id), sizeof(((UmiProjectReferenceSnapshot *)0)->target_project_id), 0 },
    {"kind", offsetof(UmiProjectReferenceSnapshot, kind), sizeof(((UmiProjectReferenceSnapshot *)0)->kind), 0 }
};
static int ContractSnapshotEqual(const UmiProjectReferenceSnapshot *left,
    const UmiProjectReferenceSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->project_id, right->project_id, sizeof(left->project_id)) == 0 &&
        memcmp(left->target_project_id, right->target_project_id, sizeof(left->target_project_id)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        left->required == right->required &&
        left->available == right->available &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiProjectReferenceSnapshot *item)
{
    item->project_id[0] = 'v';
    item->target_project_id[0] = 'v';
    item->kind[0] = 'v';
    item->required = (int)7U;
    item->available = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_project_reference_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_project_reference_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiProjectReferenceEdit
#define CONTRACT_EDIT_CURRENT umi_project_reference_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_project_reference_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiProjectReferenceSnapshot ArchiveSample(void)
{
    UmiProjectReferenceSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiProjectReferenceSnapshot *value)
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
        size_t used = strlen(value->target_project_id) + 1U;
        memset(value->target_project_id + used, 0xa5, sizeof(value->target_project_id) - used);
    }
    {
        size_t used = strlen(value->kind) + 1U;
        memset(value->kind + used, 0xa5, sizeof(value->kind) - used);
    }
}
#define ARCHIVE_TYPE UmiProjectReferenceSnapshot
#define ARCHIVE_ENCODE umi_project_reference_snapshot_archive_encode
#define ARCHIVE_DECODE umi_project_reference_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_project_reference_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_project_reference_registry_archive_restore
#include "snapshot_contract_cases.h"
