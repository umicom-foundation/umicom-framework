/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_project_configuration.c
 * PURPOSE: Exercise project configuration snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/configuration.h"
#include "umicom/project/configuration.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProjectConfigurationSnapshot
#define CONTRACT_REGISTRY UmiProjectConfigurationRegistry
#define CONTRACT_CAPACITY UMI_PROJECT_CONFIGURATION_CAPACITY
#define CONTRACT_VALIDATE umi_project_configuration_snapshot_validate
#define CONTRACT_BATCH umi_project_configuration_registry_upsert_many
#define CONTRACT_CREATE umi_project_configuration_registry_create
#define CONTRACT_DESTROY umi_project_configuration_registry_destroy
#define CONTRACT_UPSERT umi_project_configuration_registry_upsert
#define CONTRACT_REMOVE umi_project_configuration_registry_remove
#define CONTRACT_FIND umi_project_configuration_registry_find
#define CONTRACT_AT umi_project_configuration_registry_at
#define CONTRACT_COUNT umi_project_configuration_registry_count
#define CONTRACT_REVISION umi_project_configuration_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProjectConfigurationSnapshot, id), sizeof(((UmiProjectConfigurationSnapshot *)0)->id), 1 },
    {"project_id", offsetof(UmiProjectConfigurationSnapshot, project_id), sizeof(((UmiProjectConfigurationSnapshot *)0)->project_id), 0 },
    {"name", offsetof(UmiProjectConfigurationSnapshot, name), sizeof(((UmiProjectConfigurationSnapshot *)0)->name), 0 },
    {"build_type", offsetof(UmiProjectConfigurationSnapshot, build_type), sizeof(((UmiProjectConfigurationSnapshot *)0)->build_type), 0 },
    {"toolchain_id", offsetof(UmiProjectConfigurationSnapshot, toolchain_id), sizeof(((UmiProjectConfigurationSnapshot *)0)->toolchain_id), 0 },
    {"platform", offsetof(UmiProjectConfigurationSnapshot, platform), sizeof(((UmiProjectConfigurationSnapshot *)0)->platform), 0 }
};
static int ContractSnapshotEqual(const UmiProjectConfigurationSnapshot *left,
    const UmiProjectConfigurationSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->project_id, right->project_id, sizeof(left->project_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->build_type, right->build_type, sizeof(left->build_type)) == 0 &&
        memcmp(left->toolchain_id, right->toolchain_id, sizeof(left->toolchain_id)) == 0 &&
        memcmp(left->platform, right->platform, sizeof(left->platform)) == 0 &&
        left->active == right->active &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiProjectConfigurationSnapshot *item)
{
    item->project_id[0] = 'v';
    item->name[0] = 'v';
    item->build_type[0] = 'v';
    item->toolchain_id[0] = 'v';
    item->platform[0] = 'v';
    item->active = (int)9U;
}
#include "snapshot_contract_cases.h"
