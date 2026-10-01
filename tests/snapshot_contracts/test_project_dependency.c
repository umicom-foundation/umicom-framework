/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_project_dependency.c
 * PURPOSE: Exercise project dependency snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/dependency.h"
#include "umicom/project/dependency.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProjectDependencySnapshot
#define CONTRACT_REGISTRY UmiProjectDependencyRegistry
#define CONTRACT_CAPACITY UMI_PROJECT_DEPENDENCY_CAPACITY
#define CONTRACT_VALIDATE umi_project_dependency_snapshot_validate
#define CONTRACT_BATCH umi_project_dependency_registry_upsert_many
#define CONTRACT_CREATE umi_project_dependency_registry_create
#define CONTRACT_DESTROY umi_project_dependency_registry_destroy
#define CONTRACT_UPSERT umi_project_dependency_registry_upsert
#define CONTRACT_REMOVE umi_project_dependency_registry_remove
#define CONTRACT_FIND umi_project_dependency_registry_find
#define CONTRACT_AT umi_project_dependency_registry_at
#define CONTRACT_COUNT umi_project_dependency_registry_count
#define CONTRACT_REVISION umi_project_dependency_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProjectDependencySnapshot, id), sizeof(((UmiProjectDependencySnapshot *)0)->id), 1 },
    {"project_id", offsetof(UmiProjectDependencySnapshot, project_id), sizeof(((UmiProjectDependencySnapshot *)0)->project_id), 0 },
    {"name", offsetof(UmiProjectDependencySnapshot, name), sizeof(((UmiProjectDependencySnapshot *)0)->name), 0 },
    {"version", offsetof(UmiProjectDependencySnapshot, version), sizeof(((UmiProjectDependencySnapshot *)0)->version), 0 },
    {"source", offsetof(UmiProjectDependencySnapshot, source), sizeof(((UmiProjectDependencySnapshot *)0)->source), 0 },
    {"scope", offsetof(UmiProjectDependencySnapshot, scope), sizeof(((UmiProjectDependencySnapshot *)0)->scope), 0 }
};
static int ContractSnapshotEqual(const UmiProjectDependencySnapshot *left,
    const UmiProjectDependencySnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->project_id, right->project_id, sizeof(left->project_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->version, right->version, sizeof(left->version)) == 0 &&
        memcmp(left->source, right->source, sizeof(left->source)) == 0 &&
        memcmp(left->scope, right->scope, sizeof(left->scope)) == 0 &&
        left->optional == right->optional &&
        left->resolved == right->resolved &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiProjectDependencySnapshot *item)
{
    item->project_id[0] = 'v';
    item->name[0] = 'v';
    item->version[0] = 'v';
    item->source[0] = 'v';
    item->scope[0] = 'v';
    item->optional = (int)9U;
    item->resolved = (int)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_project_dependency_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_project_dependency_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiProjectDependencyEdit
#define CONTRACT_EDIT_CURRENT umi_project_dependency_registry_edit_if_current
#include "snapshot_contract_cases.h"
