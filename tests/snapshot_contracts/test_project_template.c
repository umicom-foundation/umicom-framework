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
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_project_template_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_project_template_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiProjectTemplateEdit
#define CONTRACT_EDIT_CURRENT umi_project_template_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_project_template_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiProjectTemplateSnapshot ArchiveSample(void)
{
    UmiProjectTemplateSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiProjectTemplateSnapshot *value)
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
        size_t used = strlen(value->category) + 1U;
        memset(value->category + used, 0xa5, sizeof(value->category) - used);
    }
    {
        size_t used = strlen(value->description) + 1U;
        memset(value->description + used, 0xa5, sizeof(value->description) - used);
    }
    {
        size_t used = strlen(value->source_uri) + 1U;
        memset(value->source_uri + used, 0xa5, sizeof(value->source_uri) - used);
    }
    {
        size_t used = strlen(value->language) + 1U;
        memset(value->language + used, 0xa5, sizeof(value->language) - used);
    }
    {
        size_t used = strlen(value->frontends) + 1U;
        memset(value->frontends + used, 0xa5, sizeof(value->frontends) - used);
    }
}
#define ARCHIVE_TYPE UmiProjectTemplateSnapshot
#define ARCHIVE_ENCODE umi_project_template_snapshot_archive_encode
#define ARCHIVE_DECODE umi_project_template_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_project_template_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_project_template_registry_archive_restore
#include "snapshot_contract_cases.h"
