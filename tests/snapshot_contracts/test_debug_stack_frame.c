/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_debug_stack_frame.c
 * PURPOSE: Exercise debug stack_frame snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/stack_frame.h"
#include "umicom/debug/stack_frame.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDebugStackFrameSnapshot
#define CONTRACT_REGISTRY UmiDebugStackFrameRegistry
#define CONTRACT_CAPACITY UMI_DEBUG_STACK_FRAME_CAPACITY
#define CONTRACT_VALIDATE umi_debug_stack_frame_snapshot_validate
#define CONTRACT_BATCH umi_debug_stack_frame_registry_upsert_many
#define CONTRACT_CREATE umi_debug_stack_frame_registry_create
#define CONTRACT_DESTROY umi_debug_stack_frame_registry_destroy
#define CONTRACT_UPSERT umi_debug_stack_frame_registry_upsert
#define CONTRACT_REMOVE umi_debug_stack_frame_registry_remove
#define CONTRACT_FIND umi_debug_stack_frame_registry_find
#define CONTRACT_AT umi_debug_stack_frame_registry_at
#define CONTRACT_COUNT umi_debug_stack_frame_registry_count
#define CONTRACT_REVISION umi_debug_stack_frame_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDebugStackFrameSnapshot, id), sizeof(((UmiDebugStackFrameSnapshot *)0)->id), 1 },
    {"thread_id", offsetof(UmiDebugStackFrameSnapshot, thread_id), sizeof(((UmiDebugStackFrameSnapshot *)0)->thread_id), 0 },
    {"name", offsetof(UmiDebugStackFrameSnapshot, name), sizeof(((UmiDebugStackFrameSnapshot *)0)->name), 0 },
    {"source_uri", offsetof(UmiDebugStackFrameSnapshot, source_uri), sizeof(((UmiDebugStackFrameSnapshot *)0)->source_uri), 0 }
};
static int ContractSnapshotEqual(const UmiDebugStackFrameSnapshot *left,
    const UmiDebugStackFrameSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->thread_id, right->thread_id, sizeof(left->thread_id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->source_uri, right->source_uri, sizeof(left->source_uri)) == 0 &&
        left->line == right->line &&
        left->column == right->column &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiDebugStackFrameSnapshot *item)
{
    item->thread_id[0] = 'v';
    item->name[0] = 'v';
    item->source_uri[0] = 'v';
    item->line = (uint32_t)7U;
    item->column = (uint32_t)8U;
    item->order = (int32_t)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_debug_stack_frame_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_debug_stack_frame_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiDebugStackFrameEdit
#define CONTRACT_EDIT_CURRENT umi_debug_stack_frame_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_debug_stack_frame_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiDebugStackFrameSnapshot ArchiveSample(void)
{
    UmiDebugStackFrameSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiDebugStackFrameSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->thread_id) + 1U;
        memset(value->thread_id + used, 0xa5, sizeof(value->thread_id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->source_uri) + 1U;
        memset(value->source_uri + used, 0xa5, sizeof(value->source_uri) - used);
    }
}
#define ARCHIVE_TYPE UmiDebugStackFrameSnapshot
#define ARCHIVE_ENCODE umi_debug_stack_frame_snapshot_archive_encode
#define ARCHIVE_DECODE umi_debug_stack_frame_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_debug_stack_frame_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_debug_stack_frame_registry_archive_restore
#include "snapshot_contract_cases.h"
