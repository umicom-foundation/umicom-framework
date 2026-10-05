/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_debug_variable.c
 * PURPOSE: Exercise debug variable snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/variable.h"
#include "umicom/debug/variable.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDebugVariableSnapshot
#define CONTRACT_REGISTRY UmiDebugVariableRegistry
#define CONTRACT_CAPACITY UMI_DEBUG_VARIABLE_CAPACITY
#define CONTRACT_VALIDATE umi_debug_variable_snapshot_validate
#define CONTRACT_BATCH umi_debug_variable_registry_upsert_many
#define CONTRACT_CREATE umi_debug_variable_registry_create
#define CONTRACT_DESTROY umi_debug_variable_registry_destroy
#define CONTRACT_UPSERT umi_debug_variable_registry_upsert
#define CONTRACT_REMOVE umi_debug_variable_registry_remove
#define CONTRACT_FIND umi_debug_variable_registry_find
#define CONTRACT_AT umi_debug_variable_registry_at
#define CONTRACT_COUNT umi_debug_variable_registry_count
#define CONTRACT_REVISION umi_debug_variable_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDebugVariableSnapshot, id), sizeof(((UmiDebugVariableSnapshot *)0)->id), 1 },
    {"scope_id", offsetof(UmiDebugVariableSnapshot, scope_id), sizeof(((UmiDebugVariableSnapshot *)0)->scope_id), 0 },
    {"name", offsetof(UmiDebugVariableSnapshot, name), sizeof(((UmiDebugVariableSnapshot *)0)->name), 0 },
    {"value", offsetof(UmiDebugVariableSnapshot, value), sizeof(((UmiDebugVariableSnapshot *)0)->value), 0 },
    {"type", offsetof(UmiDebugVariableSnapshot, type), sizeof(((UmiDebugVariableSnapshot *)0)->type), 0 },
    {"evaluate_name", offsetof(UmiDebugVariableSnapshot, evaluate_name), sizeof(((UmiDebugVariableSnapshot *)0)->evaluate_name), 0 }
};
static int ContractSnapshotEqual(const UmiDebugVariableSnapshot *left,
    const UmiDebugVariableSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->scope_id, right->scope_id, sizeof(left->scope_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->value, right->value, sizeof(left->value)) == 0 &&
        memcmp(left->type, right->type, sizeof(left->type)) == 0 &&
        memcmp(left->evaluate_name, right->evaluate_name, sizeof(left->evaluate_name)) == 0 &&
        left->variables_reference == right->variables_reference &&
        left->changed == right->changed &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiDebugVariableSnapshot *item)
{
    item->scope_id[0] = 'v';
    item->name[0] = 'v';
    item->value[0] = 'v';
    item->type[0] = 'v';
    item->evaluate_name[0] = 'v';
    item->variables_reference = (uint64_t)9U;
    item->changed = (int)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_debug_variable_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_debug_variable_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiDebugVariableEdit
#define CONTRACT_EDIT_CURRENT umi_debug_variable_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_debug_variable_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiDebugVariableSnapshot ArchiveSample(void)
{
    UmiDebugVariableSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiDebugVariableSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->scope_id) + 1U;
        memset(value->scope_id + used, 0xa5, sizeof(value->scope_id) - used);
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
        size_t used = strlen(value->type) + 1U;
        memset(value->type + used, 0xa5, sizeof(value->type) - used);
    }
    {
        size_t used = strlen(value->evaluate_name) + 1U;
        memset(value->evaluate_name + used, 0xa5, sizeof(value->evaluate_name) - used);
    }
}
#define ARCHIVE_TYPE UmiDebugVariableSnapshot
#define ARCHIVE_ENCODE umi_debug_variable_snapshot_archive_encode
#define ARCHIVE_DECODE umi_debug_variable_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_debug_variable_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_debug_variable_registry_archive_restore
#include "snapshot_contract_cases.h"
