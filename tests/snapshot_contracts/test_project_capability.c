/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_project_capability.c
 * PURPOSE: Exercise project capability snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/capability.h"
#include "umicom/project/capability.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProjectCapabilitySnapshot
#define CONTRACT_REGISTRY UmiProjectCapabilityRegistry
#define CONTRACT_CAPACITY UMI_PROJECT_CAPABILITY_CAPACITY
#define CONTRACT_VALIDATE umi_project_capability_snapshot_validate
#define CONTRACT_BATCH umi_project_capability_registry_upsert_many
#define CONTRACT_CREATE umi_project_capability_registry_create
#define CONTRACT_DESTROY umi_project_capability_registry_destroy
#define CONTRACT_UPSERT umi_project_capability_registry_upsert
#define CONTRACT_REMOVE umi_project_capability_registry_remove
#define CONTRACT_FIND umi_project_capability_registry_find
#define CONTRACT_AT umi_project_capability_registry_at
#define CONTRACT_COUNT umi_project_capability_registry_count
#define CONTRACT_REVISION umi_project_capability_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProjectCapabilitySnapshot, id), sizeof(((UmiProjectCapabilitySnapshot *)0)->id), 1 },
    {"project_id", offsetof(UmiProjectCapabilitySnapshot, project_id), sizeof(((UmiProjectCapabilitySnapshot *)0)->project_id), 0 },
    {"capability_id", offsetof(UmiProjectCapabilitySnapshot, capability_id), sizeof(((UmiProjectCapabilitySnapshot *)0)->capability_id), 0 },
    {"version", offsetof(UmiProjectCapabilitySnapshot, version), sizeof(((UmiProjectCapabilitySnapshot *)0)->version), 0 }
};
static int ContractSnapshotEqual(const UmiProjectCapabilitySnapshot *left,
    const UmiProjectCapabilitySnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->project_id, right->project_id, sizeof(left->project_id)) == 0 &&
        memcmp(left->capability_id, right->capability_id, sizeof(left->capability_id)) == 0 &&
        memcmp(left->version, right->version, sizeof(left->version)) == 0 &&
        left->required == right->required &&
        left->available == right->available &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiProjectCapabilitySnapshot *item)
{
    item->project_id[0] = 'v';
    item->capability_id[0] = 'v';
    item->version[0] = 'v';
    item->required = (int)7U;
    item->available = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_project_capability_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_project_capability_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiProjectCapabilityEdit
#define CONTRACT_EDIT_CURRENT umi_project_capability_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_project_capability_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiProjectCapabilitySnapshot ArchiveSample(void)
{
    UmiProjectCapabilitySnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiProjectCapabilitySnapshot *value)
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
        size_t used = strlen(value->capability_id) + 1U;
        memset(value->capability_id + used, 0xa5, sizeof(value->capability_id) - used);
    }
    {
        size_t used = strlen(value->version) + 1U;
        memset(value->version + used, 0xa5, sizeof(value->version) - used);
    }
}
#define ARCHIVE_TYPE UmiProjectCapabilitySnapshot
#define ARCHIVE_ENCODE umi_project_capability_snapshot_archive_encode
#define ARCHIVE_DECODE umi_project_capability_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_project_capability_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_project_capability_registry_archive_restore
#include "snapshot_contract_cases.h"
