/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/memory_view.c
 *
 * PURPOSE:
 *   Implement safe memory snapshots, deterministic presentation and optimistic
 *   write plans without directly accessing debuggee address space.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/memory_view.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDebugMemoryView {
    UmiDebugMemoryBlock *blocks;
    size_t count;
    size_t capacity;
    char selected_block_id[UMI_DEBUG_MEMORY_BLOCK_ID_CAPACITY];
    uint64_t next_sequence;
    uint64_t revision;
};

static int has_terminator(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static UmiStatus copy_text(char *destination,
                           size_t destination_capacity,
                           const char *source)
{
    size_t length;

    if (destination == NULL || destination_capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= destination_capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static int valid_block(const UmiDebugMemoryBlock *block)
{
    size_t accounted;

    if (block == NULL ||
        block->struct_size != (uint32_t)sizeof(*block) ||
        block->api_version != UMI_DEBUG_MEMORY_VIEW_API_VERSION ||
        !has_terminator(block->id, sizeof(block->id)) ||
        block->id[0] == '\0' ||
        !has_terminator(block->session_id, sizeof(block->session_id)) ||
        !has_terminator(block->memory_reference,
                        sizeof(block->memory_reference)) ||
        (block->memory_reference[0] == '\0' && block->address == 0U) ||
        block->length > sizeof(block->bytes) ||
        block->requested_length > sizeof(block->bytes) ||
        block->length > block->requested_length ||
        block->unreadable_bytes > block->requested_length ||
        block->sequence == UINT64_MAX) {
        return 0;
    }
    if (block->length > SIZE_MAX - block->unreadable_bytes) return 0;
    accounted = block->length + block->unreadable_bytes;
    if (accounted > block->requested_length) return 0;
    return 1;
}

static int valid_plan(const UmiDebugMemoryWritePlan *plan)
{
    return plan != NULL &&
           plan->struct_size == (uint32_t)sizeof(*plan) &&
           plan->api_version == UMI_DEBUG_MEMORY_VIEW_API_VERSION &&
           has_terminator(plan->block_id, sizeof(plan->block_id)) &&
           plan->block_id[0] != '\0' &&
           has_terminator(plan->memory_reference,
                          sizeof(plan->memory_reference)) &&
           plan->length != 0U &&
           plan->length <= sizeof(plan->replacement) && plan->valid;
}

static size_t find_index(const UmiDebugMemoryView *view,
                         const char *block_id)
{
    size_t position;

    if (view == NULL || block_id == NULL) return SIZE_MAX;
    for (position = 0U; position < view->count; ++position) {
        if (strcmp(view->blocks[position].id, block_id) == 0) return position;
    }
    return SIZE_MAX;
}

static UmiStatus reserve(UmiDebugMemoryView *view, size_t required_capacity)
{
    UmiDebugMemoryBlock *replacement;
    size_t new_capacity;

    if (required_capacity <= view->capacity) return UMI_STATUS_OK;
    if (required_capacity > UMI_DEBUG_MEMORY_BLOCK_MAXIMUM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = view->capacity;
    if (new_capacity == 0U) new_capacity = UMI_DEBUG_MEMORY_BLOCK_DEFAULT_CAPACITY;
    while (new_capacity < required_capacity) {
        if (new_capacity > UMI_DEBUG_MEMORY_BLOCK_MAXIMUM_CAPACITY / 2U) {
            new_capacity = UMI_DEBUG_MEMORY_BLOCK_MAXIMUM_CAPACITY;
        } else {
            new_capacity *= 2U;
        }
    }
    if (new_capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiDebugMemoryBlock *)realloc(
        view->blocks, new_capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memset(&replacement[view->capacity], 0,
                 (new_capacity - view->capacity) * sizeof(*replacement));
    view->blocks = replacement;
    view->capacity = new_capacity;
    return UMI_STATUS_OK;
}

static int compare_blocks(const void *left_pointer,
                          const void *right_pointer)
{
    const UmiDebugMemoryBlock *left =
        (const UmiDebugMemoryBlock *)left_pointer;
    const UmiDebugMemoryBlock *right =
        (const UmiDebugMemoryBlock *)right_pointer;
    int order;

    if (left->address < right->address) return -1;
    if (left->address > right->address) return 1;
    order = strcmp(left->memory_reference, right->memory_reference);
    if (order != 0) return order < 0 ? -1 : 1;
    if (left->reference_offset < right->reference_offset) return -1;
    if (left->reference_offset > right->reference_offset) return 1;
    if (left->sequence < right->sequence) return -1;
    if (left->sequence > right->sequence) return 1;
    order = strcmp(left->id, right->id);
    return order < 0 ? -1 : (order > 0 ? 1 : 0);
}

static UmiStatus append_text(char *out_text,
                             size_t text_capacity,
                             size_t *in_out_length,
                             const char *text)
{
    size_t text_length;

    if (out_text == NULL || in_out_length == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    text_length = strlen(text);
    if (*in_out_length > text_capacity ||
        text_length >= text_capacity - *in_out_length) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(&out_text[*in_out_length], text, text_length + 1U);
    *in_out_length += text_length;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_memory_view_create(UmiDebugMemoryView **out_view)
{
    UmiDebugMemoryView *view;

    if (out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    view = (UmiDebugMemoryView *)calloc(1U, sizeof(*view));
    if (view == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    view->next_sequence = 1U;
    view->revision = 1U;
    *out_view = view;
    return UMI_STATUS_OK;
}

void umi_debug_memory_view_destroy(UmiDebugMemoryView *view)
{
    if (view == NULL) return;
    free(view->blocks);
    (void)memset(view, 0, sizeof(*view));
    free(view);
}

UmiStatus umi_debug_memory_view_clear(UmiDebugMemoryView *view)
{
    if (view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (view->blocks != NULL) {
        (void)memset(view->blocks, 0,
                     view->capacity * sizeof(view->blocks[0]));
    }
    view->count = 0U;
    view->selected_block_id[0] = '\0';
    view->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_memory_view_upsert(
    UmiDebugMemoryView *view,
    const UmiDebugMemoryBlock *block)
{
    UmiDebugMemoryBlock copy;
    size_t position;
    UmiStatus status;
    int existing;

    if (view == NULL || !valid_block(block)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, block->id);
    existing = position != SIZE_MAX;
    if (!existing && block->sequence == 0U &&
        view->next_sequence == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (!existing) {
        status = reserve(view, view->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        position = view->count;
        view->count += 1U;
    }
    copy = *block;
    copy.struct_size = (uint32_t)sizeof(copy);
    copy.api_version = UMI_DEBUG_MEMORY_VIEW_API_VERSION;
    copy.id[sizeof(copy.id) - 1U] = '\0';
    copy.session_id[sizeof(copy.session_id) - 1U] = '\0';
    copy.memory_reference[sizeof(copy.memory_reference) - 1U] = '\0';
    copy.writable = copy.writable != 0;
    copy.stale = copy.stale != 0;
    copy.complete = copy.complete != 0;
    if (copy.sequence == 0U && existing) {
        copy.sequence = view->blocks[position].sequence;
    } else if (copy.sequence == 0U) {
        copy.sequence = view->next_sequence;
        view->next_sequence += 1U;
    } else if (copy.sequence >= view->next_sequence) {
        view->next_sequence = copy.sequence + 1U;
    }
    view->revision += 1U;
    copy.revision = view->revision;
    view->blocks[position] = copy;
    if (view->count > 1U) {
        qsort(view->blocks, view->count, sizeof(view->blocks[0]),
              compare_blocks);
    }
    if (view->selected_block_id[0] == '\0') {
        (void)copy_text(view->selected_block_id,
                        sizeof(view->selected_block_id), block->id);
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_memory_view_remove(
    UmiDebugMemoryView *view,
    const char *block_id)
{
    size_t position;

    if (view == NULL || block_id == NULL || block_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, block_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (position + 1U < view->count) {
        (void)memmove(&view->blocks[position], &view->blocks[position + 1U],
                      (view->count - position - 1U) *
                          sizeof(view->blocks[0]));
    }
    view->count -= 1U;
    (void)memset(&view->blocks[view->count], 0,
                 sizeof(view->blocks[view->count]));
    if (strcmp(view->selected_block_id, block_id) == 0) {
        view->selected_block_id[0] = '\0';
        if (view->count != 0U) {
            (void)copy_text(view->selected_block_id,
                            sizeof(view->selected_block_id),
                            view->blocks[0].id);
        }
    }
    view->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_memory_view_select(
    UmiDebugMemoryView *view,
    const char *block_id)
{
    UmiStatus status;

    if (view == NULL || block_id == NULL || block_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (find_index(view, block_id) == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (strcmp(view->selected_block_id, block_id) != 0) {
        status = copy_text(view->selected_block_id,
                           sizeof(view->selected_block_id), block_id);
        if (status != UMI_STATUS_OK) return status;
        view->revision += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_memory_view_mark_stale(
    UmiDebugMemoryView *view,
    const char *block_id,
    int stale)
{
    size_t position;
    int normalized;

    if (view == NULL || block_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    position = find_index(view, block_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    normalized = stale != 0;
    if (view->blocks[position].stale != normalized) {
        view->blocks[position].stale = normalized;
        view->revision += 1U;
        view->blocks[position].revision = view->revision;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_memory_view_find(
    const UmiDebugMemoryView *view,
    const char *block_id,
    UmiDebugMemoryBlock *out_block)
{
    size_t position;

    if (view == NULL || block_id == NULL || out_block == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, block_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_block = view->blocks[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_memory_view_at(
    const UmiDebugMemoryView *view,
    size_t index,
    UmiDebugMemoryBlock *out_block)
{
    if (view == NULL || out_block == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= view->count) return UMI_STATUS_NOT_FOUND;
    *out_block = view->blocks[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_memory_view_selected(
    const UmiDebugMemoryView *view,
    UmiDebugMemoryBlock *out_block)
{
    if (view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_debug_memory_view_find(view, view->selected_block_id, out_block);
}

UmiStatus umi_debug_memory_view_read_byte(
    const UmiDebugMemoryView *view,
    const char *block_id,
    size_t block_offset,
    unsigned char *out_byte)
{
    size_t position;

    if (view == NULL || block_id == NULL || out_byte == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, block_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (view->blocks[position].stale) return UMI_STATUS_INVALID_STATE;
    if (block_offset >= view->blocks[position].length) {
        return UMI_STATUS_UNAVAILABLE;
    }
    *out_byte = view->blocks[position].bytes[block_offset];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_memory_view_format_line(
    const UmiDebugMemoryView *view,
    const char *block_id,
    size_t block_offset,
    size_t bytes_per_line,
    char *out_text,
    size_t text_capacity)
{
    const UmiDebugMemoryBlock *block;
    size_t position;
    size_t line_length;
    size_t output_length = 0U;
    size_t byte_position;
    char fragment[64];
    int written;
    UmiStatus status;

    if (view == NULL || block_id == NULL || out_text == NULL ||
        text_capacity == 0U || bytes_per_line == 0U ||
        bytes_per_line > 64U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, block_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    block = &view->blocks[position];
    if (block->stale) return UMI_STATUS_INVALID_STATE;
    if (block_offset >= block->length) return UMI_STATUS_NOT_FOUND;
    line_length = block->length - block_offset;
    if (line_length > bytes_per_line) line_length = bytes_per_line;
    if (block_offset > (size_t)INT64_MAX ||
        block->address > UINT64_MAX - (uint64_t)block_offset) {
        return UMI_STATUS_INVALID_STATE;
    }

    out_text[0] = '\0';
    written = snprintf(fragment, sizeof(fragment), "%016" PRIx64 "  ",
                       block->address + (uint64_t)block_offset);
    if (written < 0 || (size_t)written >= sizeof(fragment)) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    status = append_text(out_text, text_capacity, &output_length, fragment);
    if (status != UMI_STATUS_OK) return status;

    for (byte_position = 0U; byte_position < bytes_per_line; ++byte_position) {
        if (byte_position < line_length) {
            written = snprintf(fragment, sizeof(fragment), "%02x ",
                               (unsigned int)block->bytes[
                                   block_offset + byte_position]);
        } else {
            written = snprintf(fragment, sizeof(fragment), "   ");
        }
        if (written < 0 || (size_t)written >= sizeof(fragment)) {
            return UMI_STATUS_INTERNAL_ERROR;
        }
        status = append_text(out_text, text_capacity, &output_length, fragment);
        if (status != UMI_STATUS_OK) return status;
    }
    status = append_text(out_text, text_capacity, &output_length, " |");
    if (status != UMI_STATUS_OK) return status;
    for (byte_position = 0U; byte_position < line_length; ++byte_position) {
        unsigned char value = block->bytes[block_offset + byte_position];
        fragment[0] = isprint((int)value) != 0 ? (char)value : '.';
        fragment[1] = '\0';
        status = append_text(out_text, text_capacity, &output_length, fragment);
        if (status != UMI_STATUS_OK) return status;
    }
    return append_text(out_text, text_capacity, &output_length, "|");
}

UmiStatus umi_debug_memory_view_prepare_write(
    const UmiDebugMemoryView *view,
    const char *block_id,
    size_t block_offset,
    const unsigned char *replacement,
    size_t replacement_length,
    UmiDebugMemoryWritePlan *out_plan)
{
    const UmiDebugMemoryBlock *block;
    size_t position;
    UmiStatus status;

    if (view == NULL || block_id == NULL || replacement == NULL ||
        out_plan == NULL || replacement_length == 0U ||
        replacement_length > sizeof(out_plan->replacement)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, block_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    block = &view->blocks[position];
    if (!block->writable) return UMI_STATUS_PERMISSION_DENIED;
    if (block->stale) return UMI_STATUS_INVALID_STATE;
    if (block_offset > block->length ||
        replacement_length > block->length - block_offset) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (block_offset > (size_t)INT64_MAX ||
        block->address > UINT64_MAX - (uint64_t)block_offset) {
        return UMI_STATUS_INVALID_STATE;
    }
    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->struct_size = (uint32_t)sizeof(*out_plan);
    out_plan->api_version = UMI_DEBUG_MEMORY_VIEW_API_VERSION;
    status = copy_text(out_plan->block_id, sizeof(out_plan->block_id), block->id);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_plan->memory_reference,
                       sizeof(out_plan->memory_reference),
                       block->memory_reference);
    if (status != UMI_STATUS_OK) return status;
    out_plan->absolute_address = block->address + (uint64_t)block_offset;
    if (block->reference_offset > INT64_MAX - (int64_t)block_offset) {
        return UMI_STATUS_INVALID_STATE;
    }
    out_plan->reference_offset =
        block->reference_offset + (int64_t)block_offset;
    out_plan->block_revision = block->revision;
    out_plan->block_offset = block_offset;
    out_plan->length = replacement_length;
    (void)memcpy(out_plan->expected, &block->bytes[block_offset],
                 replacement_length);
    (void)memcpy(out_plan->replacement, replacement, replacement_length);
    out_plan->valid = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_memory_view_validate_write(
    const UmiDebugMemoryView *view,
    const UmiDebugMemoryWritePlan *plan)
{
    const UmiDebugMemoryBlock *block;
    size_t position;

    if (view == NULL || !valid_plan(plan)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, plan->block_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    block = &view->blocks[position];
    if (!block->writable) return UMI_STATUS_PERMISSION_DENIED;
    if (block->stale || block->revision != plan->block_revision) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(block->memory_reference, plan->memory_reference) != 0 ||
        plan->block_offset > block->length ||
        plan->length > block->length - plan->block_offset) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (memcmp(&block->bytes[plan->block_offset], plan->expected,
               plan->length) != 0) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_memory_view_commit_write(
    UmiDebugMemoryView *view,
    const UmiDebugMemoryWritePlan *plan)
{
    size_t position;
    UmiStatus status;

    status = umi_debug_memory_view_validate_write(view, plan);
    if (status != UMI_STATUS_OK) return status;
    position = find_index(view, plan->block_id);
    (void)memcpy(&view->blocks[position].bytes[plan->block_offset],
                 plan->replacement, plan->length);
    view->revision += 1U;
    view->blocks[position].revision = view->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_memory_view_snapshot(
    const UmiDebugMemoryView *view,
    UmiDebugMemoryViewSnapshot *out_snapshot)
{
    size_t position;

    if (view == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEBUG_MEMORY_VIEW_API_VERSION;
    out_snapshot->block_count = view->count;
    out_snapshot->revision = view->revision;
    (void)copy_text(out_snapshot->selected_block_id,
                    sizeof(out_snapshot->selected_block_id),
                    view->selected_block_id);
    out_snapshot->has_selection = view->selected_block_id[0] != '\0';
    for (position = 0U; position < view->count; ++position) {
        out_snapshot->readable_byte_count += view->blocks[position].length;
        out_snapshot->unreadable_byte_count +=
            view->blocks[position].unreadable_bytes;
        if (view->blocks[position].writable) {
            out_snapshot->writable_block_count += 1U;
        }
        if (view->blocks[position].stale) {
            out_snapshot->stale_block_count += 1U;
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_debug_memory_view_count(const UmiDebugMemoryView *view)
{
    return view != NULL ? view->count : 0U;
}

uint64_t umi_debug_memory_view_revision(const UmiDebugMemoryView *view)
{
    return view != NULL ? view->revision : 0U;
}
