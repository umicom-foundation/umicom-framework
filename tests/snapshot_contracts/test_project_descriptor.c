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
#include "snapshot_contract_cases.h"
