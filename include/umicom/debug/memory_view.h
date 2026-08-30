/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/memory_view.h
 *
 * PURPOSE:
 *   Provide a bounded, revision-safe memory inspection and edit-planning model
 *   for native, managed, embedded and remote debugger adapters.
 *
 * SAFETY MODEL:
 *   The Framework never dereferences a debuggee address. Adapters supply copied
 *   bytes. Writes are prepared as explicit plans containing the block revision
 *   and expected bytes. The caller invokes the adapter and commits the plan to
 *   the local model only after the debuggee confirms the write.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_MEMORY_VIEW_H
#define UMICOM_DEBUG_MEMORY_VIEW_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_MEMORY_VIEW_API_VERSION 1U
#define UMI_DEBUG_MEMORY_BLOCK_DEFAULT_CAPACITY 16U
#define UMI_DEBUG_MEMORY_BLOCK_MAXIMUM_CAPACITY 1024U
#define UMI_DEBUG_MEMORY_BLOCK_ID_CAPACITY 128U
#define UMI_DEBUG_MEMORY_SESSION_ID_CAPACITY 128U
#define UMI_DEBUG_MEMORY_REFERENCE_CAPACITY 512U
#define UMI_DEBUG_MEMORY_BLOCK_BYTE_CAPACITY 4096U
#define UMI_DEBUG_MEMORY_WRITE_BYTE_CAPACITY 256U

typedef struct UmiDebugMemoryBlock {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DEBUG_MEMORY_BLOCK_ID_CAPACITY];
    char session_id[UMI_DEBUG_MEMORY_SESSION_ID_CAPACITY];
    char memory_reference[UMI_DEBUG_MEMORY_REFERENCE_CAPACITY];
    uint64_t address;
    int64_t reference_offset;
    size_t requested_length;
    size_t length;
    size_t unreadable_bytes;
    uint64_t sequence;
    uint64_t revision;
    int writable;
    int stale;
    int complete;
    unsigned char bytes[UMI_DEBUG_MEMORY_BLOCK_BYTE_CAPACITY];
} UmiDebugMemoryBlock;

typedef struct UmiDebugMemoryWritePlan {
    uint32_t struct_size;
    uint32_t api_version;
    char block_id[UMI_DEBUG_MEMORY_BLOCK_ID_CAPACITY];
    char memory_reference[UMI_DEBUG_MEMORY_REFERENCE_CAPACITY];
    uint64_t absolute_address;
    int64_t reference_offset;
    uint64_t block_revision;
    size_t block_offset;
    size_t length;
    int valid;
    unsigned char expected[UMI_DEBUG_MEMORY_WRITE_BYTE_CAPACITY];
    unsigned char replacement[UMI_DEBUG_MEMORY_WRITE_BYTE_CAPACITY];
} UmiDebugMemoryWritePlan;

typedef struct UmiDebugMemoryViewSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t block_count;
    size_t readable_byte_count;
    size_t unreadable_byte_count;
    size_t writable_block_count;
    size_t stale_block_count;
    char selected_block_id[UMI_DEBUG_MEMORY_BLOCK_ID_CAPACITY];
    uint64_t revision;
    int has_selection;
} UmiDebugMemoryViewSnapshot;

typedef struct UmiDebugMemoryView UmiDebugMemoryView;

UmiStatus umi_debug_memory_view_create(UmiDebugMemoryView **out_view);
void umi_debug_memory_view_destroy(UmiDebugMemoryView *view);
UmiStatus umi_debug_memory_view_clear(UmiDebugMemoryView *view);
UmiStatus umi_debug_memory_view_upsert(
    UmiDebugMemoryView *view,
    const UmiDebugMemoryBlock *block);
UmiStatus umi_debug_memory_view_remove(
    UmiDebugMemoryView *view,
    const char *block_id);
UmiStatus umi_debug_memory_view_select(
    UmiDebugMemoryView *view,
    const char *block_id);
UmiStatus umi_debug_memory_view_mark_stale(
    UmiDebugMemoryView *view,
    const char *block_id,
    int stale);
UmiStatus umi_debug_memory_view_find(
    const UmiDebugMemoryView *view,
    const char *block_id,
    UmiDebugMemoryBlock *out_block);
UmiStatus umi_debug_memory_view_at(
    const UmiDebugMemoryView *view,
    size_t index,
    UmiDebugMemoryBlock *out_block);
UmiStatus umi_debug_memory_view_selected(
    const UmiDebugMemoryView *view,
    UmiDebugMemoryBlock *out_block);
UmiStatus umi_debug_memory_view_read_byte(
    const UmiDebugMemoryView *view,
    const char *block_id,
    size_t block_offset,
    unsigned char *out_byte);
UmiStatus umi_debug_memory_view_format_line(
    const UmiDebugMemoryView *view,
    const char *block_id,
    size_t block_offset,
    size_t bytes_per_line,
    char *out_text,
    size_t text_capacity);
UmiStatus umi_debug_memory_view_prepare_write(
    const UmiDebugMemoryView *view,
    const char *block_id,
    size_t block_offset,
    const unsigned char *replacement,
    size_t replacement_length,
    UmiDebugMemoryWritePlan *out_plan);
UmiStatus umi_debug_memory_view_validate_write(
    const UmiDebugMemoryView *view,
    const UmiDebugMemoryWritePlan *plan);
UmiStatus umi_debug_memory_view_commit_write(
    UmiDebugMemoryView *view,
    const UmiDebugMemoryWritePlan *plan);
UmiStatus umi_debug_memory_view_snapshot(
    const UmiDebugMemoryView *view,
    UmiDebugMemoryViewSnapshot *out_snapshot);
size_t umi_debug_memory_view_count(const UmiDebugMemoryView *view);
uint64_t umi_debug_memory_view_revision(const UmiDebugMemoryView *view);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_MEMORY_VIEW_H */
