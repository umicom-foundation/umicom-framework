/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_debug_module.c
 * PURPOSE: Exercise debug module snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/module.h"
#include "umicom/debug/module.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDebugModuleSnapshot
#define CONTRACT_REGISTRY UmiDebugModuleRegistry
#define CONTRACT_CAPACITY UMI_DEBUG_MODULE_CAPACITY
#define CONTRACT_VALIDATE umi_debug_module_snapshot_validate
#define CONTRACT_BATCH umi_debug_module_registry_upsert_many
#define CONTRACT_CREATE umi_debug_module_registry_create
#define CONTRACT_DESTROY umi_debug_module_registry_destroy
#define CONTRACT_UPSERT umi_debug_module_registry_upsert
#define CONTRACT_REMOVE umi_debug_module_registry_remove
#define CONTRACT_FIND umi_debug_module_registry_find
#define CONTRACT_AT umi_debug_module_registry_at
#define CONTRACT_COUNT umi_debug_module_registry_count
#define CONTRACT_REVISION umi_debug_module_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDebugModuleSnapshot, id), sizeof(((UmiDebugModuleSnapshot *)0)->id), 1 },
    {"session_id", offsetof(UmiDebugModuleSnapshot, session_id), sizeof(((UmiDebugModuleSnapshot *)0)->session_id), 0 },
    {"name", offsetof(UmiDebugModuleSnapshot, name), sizeof(((UmiDebugModuleSnapshot *)0)->name), 0 },
    {"path", offsetof(UmiDebugModuleSnapshot, path), sizeof(((UmiDebugModuleSnapshot *)0)->path), 0 },
    {"version", offsetof(UmiDebugModuleSnapshot, version), sizeof(((UmiDebugModuleSnapshot *)0)->version), 0 },
    {"symbol_status", offsetof(UmiDebugModuleSnapshot, symbol_status), sizeof(((UmiDebugModuleSnapshot *)0)->symbol_status), 0 }
};
static int ContractSnapshotEqual(const UmiDebugModuleSnapshot *left,
    const UmiDebugModuleSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->session_id, right->session_id, sizeof(left->session_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->path, right->path, sizeof(left->path)) == 0 &&
        memcmp(left->version, right->version, sizeof(left->version)) == 0 &&
        memcmp(left->symbol_status, right->symbol_status, sizeof(left->symbol_status)) == 0 &&
        left->optimised == right->optimised &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiDebugModuleSnapshot *item)
{
    item->session_id[0] = 'v';
    item->name[0] = 'v';
    item->path[0] = 'v';
    item->version[0] = 'v';
    item->symbol_status[0] = 'v';
    item->optimised = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_debug_module_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_debug_module_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiDebugModuleEdit
#define CONTRACT_EDIT_CURRENT umi_debug_module_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_debug_module_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiDebugModuleSnapshot ArchiveSample(void)
{
    UmiDebugModuleSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiDebugModuleSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->path) + 1U;
        memset(value->path + used, 0xa5, sizeof(value->path) - used);
    }
    {
        size_t used = strlen(value->version) + 1U;
        memset(value->version + used, 0xa5, sizeof(value->version) - used);
    }
    {
        size_t used = strlen(value->symbol_status) + 1U;
        memset(value->symbol_status + used, 0xa5, sizeof(value->symbol_status) - used);
    }
}
#define ARCHIVE_TYPE UmiDebugModuleSnapshot
#define ARCHIVE_ENCODE umi_debug_module_snapshot_archive_encode
#define ARCHIVE_DECODE umi_debug_module_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_debug_module_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_debug_module_registry_archive_restore
#include "snapshot_contract_cases.h"
