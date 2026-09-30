/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_project_template.c
 * PURPOSE: Exercise project template snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/template.h"
#include "umicom/project/template.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProjectTemplateSnapshot
#define CONTRACT_REGISTRY UmiProjectTemplateRegistry
#define CONTRACT_CAPACITY UMI_PROJECT_TEMPLATE_CAPACITY
#define CONTRACT_VALIDATE umi_project_template_snapshot_validate
#define CONTRACT_BATCH umi_project_template_registry_upsert_many
#define CONTRACT_CREATE umi_project_template_registry_create
#define CONTRACT_DESTROY umi_project_template_registry_destroy
#define CONTRACT_UPSERT umi_project_template_registry_upsert
#define CONTRACT_REMOVE umi_project_template_registry_remove
#define CONTRACT_FIND umi_project_template_registry_find
#define CONTRACT_AT umi_project_template_registry_at
#define CONTRACT_COUNT umi_project_template_registry_count
#define CONTRACT_REVISION umi_project_template_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProjectTemplateSnapshot, id), sizeof(((UmiProjectTemplateSnapshot *)0)->id), 1 },
    {"name", offsetof(UmiProjectTemplateSnapshot, name), sizeof(((UmiProjectTemplateSnapshot *)0)->name), 0 },
    {"category", offsetof(UmiProjectTemplateSnapshot, category), sizeof(((UmiProjectTemplateSnapshot *)0)->category), 0 },
    {"description", offsetof(UmiProjectTemplateSnapshot, description), sizeof(((UmiProjectTemplateSnapshot *)0)->description), 0 },
    {"source_uri", offsetof(UmiProjectTemplateSnapshot, source_uri), sizeof(((UmiProjectTemplateSnapshot *)0)->source_uri), 0 },
    {"language", offsetof(UmiProjectTemplateSnapshot, language), sizeof(((UmiProjectTemplateSnapshot *)0)->language), 0 },
    {"frontends", offsetof(UmiProjectTemplateSnapshot, frontends), sizeof(((UmiProjectTemplateSnapshot *)0)->frontends), 0 }
};
static int ContractSnapshotEqual(const UmiProjectTemplateSnapshot *left,
    const UmiProjectTemplateSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->category, right->category, sizeof(left->category)) == 0 &&
        memcmp(left->description, right->description, sizeof(left->description)) == 0 &&
        memcmp(left->source_uri, right->source_uri, sizeof(left->source_uri)) == 0 &&
        memcmp(left->language, right->language, sizeof(left->language)) == 0 &&
        memcmp(left->frontends, right->frontends, sizeof(left->frontends)) == 0 &&
        left->trusted == right->trusted &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiProjectTemplateSnapshot *item)
{
    item->name[0] = 'v';
    item->category[0] = 'v';
    item->description[0] = 'v';
    item->source_uri[0] = 'v';
    item->language[0] = 'v';
    item->frontends[0] = 'v';
    item->trusted = (int)10U;
}
#include "snapshot_contract_cases.h"
