/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_project_descriptor.c
 * PURPOSE: Exercise project descriptor snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/descriptor.h"
#include "umicom/project/descriptor.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProjectDescriptorSnapshot
#define CONTRACT_REGISTRY UmiProjectDescriptorRegistry
#define CONTRACT_CAPACITY UMI_PROJECT_DESCRIPTOR_CAPACITY
#define CONTRACT_VALIDATE umi_project_descriptor_snapshot_validate
#define CONTRACT_BATCH umi_project_descriptor_registry_upsert_many
#define CONTRACT_CREATE umi_project_descriptor_registry_create
#define CONTRACT_DESTROY umi_project_descriptor_registry_destroy
#define CONTRACT_UPSERT umi_project_descriptor_registry_upsert
#define CONTRACT_REMOVE umi_project_descriptor_registry_remove
#define CONTRACT_FIND umi_project_descriptor_registry_find
#define CONTRACT_AT umi_project_descriptor_registry_at
#define CONTRACT_COUNT umi_project_descriptor_registry_count
#define CONTRACT_REVISION umi_project_descriptor_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProjectDescriptorSnapshot, id), sizeof(((UmiProjectDescriptorSnapshot *)0)->id), 1 },
    {"name", offsetof(UmiProjectDescriptorSnapshot, name), sizeof(((UmiProjectDescriptorSnapshot *)0)->name), 0 },
    {"root_uri", offsetof(UmiProjectDescriptorSnapshot, root_uri), sizeof(((UmiProjectDescriptorSnapshot *)0)->root_uri), 0 },
    {"kind", offsetof(UmiProjectDescriptorSnapshot, kind), sizeof(((UmiProjectDescriptorSnapshot *)0)->kind), 0 },
    {"primary_language", offsetof(UmiProjectDescriptorSnapshot, primary_language), sizeof(((UmiProjectDescriptorSnapshot *)0)->primary_language), 0 },
    {"version", offsetof(UmiProjectDescriptorSnapshot, version), sizeof(((UmiProjectDescriptorSnapshot *)0)->version), 0 },
    {"description", offsetof(UmiProjectDescriptorSnapshot, description), sizeof(((UmiProjectDescriptorSnapshot *)0)->description), 0 }
};
static int ContractSnapshotEqual(const UmiProjectDescriptorSnapshot *left,
    const UmiProjectDescriptorSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->root_uri, right->root_uri, sizeof(left->root_uri)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->primary_language, right->primary_language, sizeof(left->primary_language)) == 0 &&
        memcmp(left->version, right->version, sizeof(left->version)) == 0 &&
        memcmp(left->description, right->description, sizeof(left->description)) == 0 &&
        left->enabled == right->enabled &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiProjectDescriptorSnapshot *item)
{
    item->name[0] = 'v';
    item->root_uri[0] = 'v';
    item->kind[0] = 'v';
    item->primary_language[0] = 'v';
    item->version[0] = 'v';
    item->description[0] = 'v';
    item->enabled = (int)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_project_descriptor_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_project_descriptor_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiProjectDescriptorEdit
#define CONTRACT_EDIT_CURRENT umi_project_descriptor_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_project_descriptor_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiProjectDescriptorSnapshot ArchiveSample(void)
{
    UmiProjectDescriptorSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiProjectDescriptorSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->root_uri) + 1U;
        memset(value->root_uri + used, 0xa5, sizeof(value->root_uri) - used);
    }
    {
        size_t used = strlen(value->kind) + 1U;
        memset(value->kind + used, 0xa5, sizeof(value->kind) - used);
    }
    {
        size_t used = strlen(value->primary_language) + 1U;
        memset(value->primary_language + used, 0xa5, sizeof(value->primary_language) - used);
    }
    {
        size_t used = strlen(value->version) + 1U;
        memset(value->version + used, 0xa5, sizeof(value->version) - used);
    }
    {
        size_t used = strlen(value->description) + 1U;
        memset(value->description + used, 0xa5, sizeof(value->description) - used);
    }
}
#define ARCHIVE_TYPE UmiProjectDescriptorSnapshot
#define ARCHIVE_ENCODE umi_project_descriptor_snapshot_archive_encode
#define ARCHIVE_DECODE umi_project_descriptor_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_project_descriptor_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_project_descriptor_registry_archive_restore
#include "snapshot_contract_cases.h"
