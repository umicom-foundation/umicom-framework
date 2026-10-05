/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_project_variable.c
 * PURPOSE: Exercise project variable snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/variable.h"
#include "umicom/project/variable.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiProjectVariableSnapshot
#define CONTRACT_REGISTRY UmiProjectVariableRegistry
#define CONTRACT_CAPACITY UMI_PROJECT_VARIABLE_CAPACITY
#define CONTRACT_VALIDATE umi_project_variable_snapshot_validate
#define CONTRACT_BATCH umi_project_variable_registry_upsert_many
#define CONTRACT_CREATE umi_project_variable_registry_create
#define CONTRACT_DESTROY umi_project_variable_registry_destroy
#define CONTRACT_UPSERT umi_project_variable_registry_upsert
#define CONTRACT_REMOVE umi_project_variable_registry_remove
#define CONTRACT_FIND umi_project_variable_registry_find
#define CONTRACT_AT umi_project_variable_registry_at
#define CONTRACT_COUNT umi_project_variable_registry_count
#define CONTRACT_REVISION umi_project_variable_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiProjectVariableSnapshot, id), sizeof(((UmiProjectVariableSnapshot *)0)->id), 1 },
    {"project_id", offsetof(UmiProjectVariableSnapshot, project_id), sizeof(((UmiProjectVariableSnapshot *)0)->project_id), 0 },
    {"name", offsetof(UmiProjectVariableSnapshot, name), sizeof(((UmiProjectVariableSnapshot *)0)->name), 0 },
    {"value", offsetof(UmiProjectVariableSnapshot, value), sizeof(((UmiProjectVariableSnapshot *)0)->value), 0 },
    {"scope", offsetof(UmiProjectVariableSnapshot, scope), sizeof(((UmiProjectVariableSnapshot *)0)->scope), 0 }
};
static int ContractSnapshotEqual(const UmiProjectVariableSnapshot *left,
    const UmiProjectVariableSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->project_id, right->project_id, sizeof(left->project_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->value, right->value, sizeof(left->value)) == 0 &&
        memcmp(left->scope, right->scope, sizeof(left->scope)) == 0 &&
        left->secret == right->secret &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiProjectVariableSnapshot *item)
{
    item->project_id[0] = 'v';
    item->name[0] = 'v';
    item->value[0] = 'v';
    item->scope[0] = 'v';
    item->secret = (int)8U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_project_variable_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_project_variable_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiProjectVariableEdit
#define CONTRACT_EDIT_CURRENT umi_project_variable_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_project_variable_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiProjectVariableSnapshot ArchiveSample(void)
{
    UmiProjectVariableSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiProjectVariableSnapshot *value)
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
        size_t used = strlen(value->value) + 1U;
        memset(value->value + used, 0xa5, sizeof(value->value) - used);
    }
    {
        size_t used = strlen(value->scope) + 1U;
        memset(value->scope + used, 0xa5, sizeof(value->scope) - used);
    }
}
#define ARCHIVE_TYPE UmiProjectVariableSnapshot
#define ARCHIVE_ENCODE umi_project_variable_snapshot_archive_encode
#define ARCHIVE_DECODE umi_project_variable_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_project_variable_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_project_variable_registry_archive_restore
#include "snapshot_contract_cases.h"
