/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_memory_view.c
 *
 * PURPOSE:
 *   Implement the test debug memory view behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework memory view and revision-safe write-plan tests.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/debug/memory_view.h"

static UmiDebugMemoryBlock make_block(void)
{
    UmiDebugMemoryBlock block;
    size_t position;
    (void)memset(&block, 0, sizeof(block));
    block.struct_size = (uint32_t)sizeof(block);
    block.api_version = UMI_DEBUG_MEMORY_VIEW_API_VERSION;
    (void)snprintf(block.id, sizeof(block.id), "%s", "stack-page");
    (void)snprintf(block.session_id, sizeof(block.session_id), "%s", "session-1");
    (void)snprintf(block.memory_reference,
                   sizeof(block.memory_reference), "%s", "stack");
    block.address = 0x1000U;
    block.requested_length = 16U;
    block.length = 16U;
    block.writable = 1;
    block.complete = 1;
    for (position = 0U; position < block.length; ++position) {
        block.bytes[position] = (unsigned char)(position + 0x20U);
    }
    return block;
}

int main(void)
{
    UmiDebugMemoryView *view = NULL;
    UmiDebugMemoryBlock block;
    UmiDebugMemoryBlock changed;
    UmiDebugMemoryWritePlan plan;
    UmiDebugMemoryViewSnapshot snapshot;
    unsigned char replacement[2] = {0xaaU, 0xbbU};
    unsigned char value = 0U;
    char line[256];

    assert(umi_debug_memory_view_create(&view) == UMI_STATUS_OK);
    block = make_block();
    assert(umi_debug_memory_view_upsert(view, &block) == UMI_STATUS_OK);
    assert(umi_debug_memory_view_find(view, "stack-page", &changed) ==
           UMI_STATUS_OK);
    {
        const uint64_t block_sequence = changed.sequence;
        changed.sequence = 0U;
        assert(umi_debug_memory_view_upsert(view, &changed) == UMI_STATUS_OK);
        assert(umi_debug_memory_view_find(view, "stack-page", &changed) ==
               UMI_STATUS_OK);
        assert(changed.sequence == block_sequence);
    }
    assert(umi_debug_memory_view_read_byte(view, "stack-page", 0U, &value) ==
           UMI_STATUS_OK);
    assert(value == 0x20U);
    assert(umi_debug_memory_view_format_line(
               view, "stack-page", 0U, 8U, line, sizeof(line)) == UMI_STATUS_OK);
    assert(strstr(line, "0000000000001000") != NULL);

    assert(umi_debug_memory_view_prepare_write(
               view, "stack-page", 4U, replacement, 2U, &plan) ==
           UMI_STATUS_OK);
    assert(plan.absolute_address == 0x1004U);
    assert(plan.expected[0] == 0x24U);
    assert(umi_debug_memory_view_validate_write(view, &plan) == UMI_STATUS_OK);
    assert(umi_debug_memory_view_commit_write(view, &plan) == UMI_STATUS_OK);
    assert(umi_debug_memory_view_read_byte(view, "stack-page", 4U, &value) ==
           UMI_STATUS_OK);
    assert(value == 0xaaU);
    assert(umi_debug_memory_view_validate_write(view, &plan) ==
           UMI_STATUS_INVALID_STATE);

    assert(umi_debug_memory_view_find(view, "stack-page", &changed) ==
           UMI_STATUS_OK);
    assert(changed.revision != plan.block_revision);
    assert(umi_debug_memory_view_mark_stale(view, "stack-page", 1) ==
           UMI_STATUS_OK);
    assert(umi_debug_memory_view_read_byte(view, "stack-page", 0U, &value) ==
           UMI_STATUS_INVALID_STATE);

    assert(umi_debug_memory_view_snapshot(view, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.block_count == 1U);
    assert(snapshot.readable_byte_count == 16U);
    assert(snapshot.writable_block_count == 1U);
    assert(snapshot.stale_block_count == 1U);

    block = make_block();
    (void)snprintf(block.id, sizeof(block.id), "%s", "sequence-invalid");
    block.sequence = UINT64_MAX;
    assert(umi_debug_memory_view_upsert(view, &block) ==
           UMI_STATUS_INVALID_ARGUMENT);
    (void)snprintf(block.id, sizeof(block.id), "%s", "sequence-limit");
    block.sequence = UINT64_MAX - 1U;
    assert(umi_debug_memory_view_upsert(view, &block) == UMI_STATUS_OK);
    (void)snprintf(block.id, sizeof(block.id), "%s", "sequence-overflow");
    block.sequence = 0U;
    assert(umi_debug_memory_view_upsert(view, &block) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    umi_debug_memory_view_destroy(view);
    return 0;
}
