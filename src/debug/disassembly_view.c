/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/disassembly_view.c
 *
 * PURPOSE:
 *   Implement deterministic disassembly storage, selection, source correlation
 *   and instruction-breakpoint state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/disassembly_view.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiDebugDisassemblyView {
    UmiDebugInstruction *items;
    size_t count;
    size_t capacity;
    char selected_instruction_id[UMI_DEBUG_INSTRUCTION_ID_CAPACITY];
    char current_instruction_id[UMI_DEBUG_INSTRUCTION_ID_CAPACITY];
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

static int valid_instruction(const UmiDebugInstruction *instruction)
{
    if (instruction == NULL ||
        instruction->struct_size != (uint32_t)sizeof(*instruction) ||
        instruction->api_version != UMI_DEBUG_DISASSEMBLY_VIEW_API_VERSION ||
        !has_terminator(instruction->id, sizeof(instruction->id)) ||
        instruction->id[0] == '\0' ||
        !has_terminator(instruction->session_id,
                        sizeof(instruction->session_id)) ||
        !has_terminator(instruction->memory_reference,
                        sizeof(instruction->memory_reference)) ||
        !has_terminator(instruction->instruction_bytes,
                        sizeof(instruction->instruction_bytes)) ||
        !has_terminator(instruction->mnemonic,
                        sizeof(instruction->mnemonic)) ||
        !has_terminator(instruction->operands,
                        sizeof(instruction->operands)) ||
        !has_terminator(instruction->comment,
                        sizeof(instruction->comment)) ||
        !has_terminator(instruction->symbol, sizeof(instruction->symbol)) ||
        !has_terminator(instruction->source_uri,
                        sizeof(instruction->source_uri)) ||
        instruction->sequence == UINT64_MAX) {
        return 0;
    }
    if (instruction->valid && instruction->mnemonic[0] == '\0') return 0;
    if (instruction->has_source && instruction->source_uri[0] == '\0') return 0;
    if (instruction->breakpoint && !instruction->can_breakpoint) return 0;
    return 1;
}

static void normalize_instruction(UmiDebugInstruction *instruction)
{
    instruction->struct_size = (uint32_t)sizeof(*instruction);
    instruction->api_version = UMI_DEBUG_DISASSEMBLY_VIEW_API_VERSION;
    instruction->id[sizeof(instruction->id) - 1U] = '\0';
    instruction->session_id[sizeof(instruction->session_id) - 1U] = '\0';
    instruction->memory_reference[
        sizeof(instruction->memory_reference) - 1U] = '\0';
    instruction->instruction_bytes[
        sizeof(instruction->instruction_bytes) - 1U] = '\0';
    instruction->mnemonic[sizeof(instruction->mnemonic) - 1U] = '\0';
    instruction->operands[sizeof(instruction->operands) - 1U] = '\0';
    instruction->comment[sizeof(instruction->comment) - 1U] = '\0';
    instruction->symbol[sizeof(instruction->symbol) - 1U] = '\0';
    instruction->source_uri[sizeof(instruction->source_uri) - 1U] = '\0';
    instruction->has_source = instruction->has_source != 0;
    instruction->current = instruction->current != 0;
    instruction->can_breakpoint = instruction->can_breakpoint != 0;
    instruction->breakpoint = instruction->breakpoint != 0;
    instruction->valid = instruction->valid != 0;
}

static size_t find_index(const UmiDebugDisassemblyView *view,
                         const char *instruction_id)
{
    size_t position;

    if (view == NULL || instruction_id == NULL) return SIZE_MAX;
    for (position = 0U; position < view->count; ++position) {
        if (strcmp(view->items[position].id, instruction_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

static size_t find_address_index(const UmiDebugDisassemblyView *view,
                                 uint64_t address)
{
    size_t position;

    if (view == NULL) return SIZE_MAX;
    for (position = 0U; position < view->count; ++position) {
        if (view->items[position].address == address) return position;
    }
    return SIZE_MAX;
}

static UmiStatus reserve(UmiDebugDisassemblyView *view,
                         size_t required_capacity)
{
    UmiDebugInstruction *replacement;
    size_t new_capacity;

    if (required_capacity <= view->capacity) return UMI_STATUS_OK;
    if (required_capacity > UMI_DEBUG_DISASSEMBLY_MAXIMUM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = view->capacity;
    if (new_capacity == 0U) new_capacity = UMI_DEBUG_DISASSEMBLY_DEFAULT_CAPACITY;
    while (new_capacity < required_capacity) {
        if (new_capacity > UMI_DEBUG_DISASSEMBLY_MAXIMUM_CAPACITY / 2U) {
            new_capacity = UMI_DEBUG_DISASSEMBLY_MAXIMUM_CAPACITY;
        } else {
            new_capacity *= 2U;
        }
    }
    if (new_capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiDebugInstruction *)realloc(
        view->items, new_capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memset(&replacement[view->capacity], 0,
                 (new_capacity - view->capacity) * sizeof(*replacement));
    view->items = replacement;
    view->capacity = new_capacity;
    return UMI_STATUS_OK;
}

static int compare_instructions(const void *left_pointer,
                                const void *right_pointer)
{
    const UmiDebugInstruction *left =
        (const UmiDebugInstruction *)left_pointer;
    const UmiDebugInstruction *right =
        (const UmiDebugInstruction *)right_pointer;
    int order;

    if (left->address < right->address) return -1;
    if (left->address > right->address) return 1;
    if (left->instruction_offset < right->instruction_offset) return -1;
    if (left->instruction_offset > right->instruction_offset) return 1;
    if (left->sequence < right->sequence) return -1;
    if (left->sequence > right->sequence) return 1;
    order = strcmp(left->id, right->id);
    return order < 0 ? -1 : (order > 0 ? 1 : 0);
}

static int duplicate_ids(const UmiDebugInstruction *items, size_t count)
{
    size_t left_position;
    size_t right_position;

    for (left_position = 0U; left_position < count; ++left_position) {
        for (right_position = left_position + 1U; right_position < count;
             ++right_position) {
            if (strcmp(items[left_position].id, items[right_position].id) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

UmiStatus umi_debug_disassembly_view_create(
    UmiDebugDisassemblyView **out_view)
{
    UmiDebugDisassemblyView *view;

    if (out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    view = (UmiDebugDisassemblyView *)calloc(1U, sizeof(*view));
    if (view == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    view->next_sequence = 1U;
    view->revision = 1U;
    *out_view = view;
    return UMI_STATUS_OK;
}

void umi_debug_disassembly_view_destroy(UmiDebugDisassemblyView *view)
{
    if (view == NULL) return;
    free(view->items);
    (void)memset(view, 0, sizeof(*view));
    free(view);
}

UmiStatus umi_debug_disassembly_view_clear(UmiDebugDisassemblyView *view)
{
    if (view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (view->items != NULL) {
        (void)memset(view->items, 0,
                     view->capacity * sizeof(view->items[0]));
    }
    view->count = 0U;
    view->selected_instruction_id[0] = '\0';
    view->current_instruction_id[0] = '\0';
    view->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_disassembly_view_load(
    UmiDebugDisassemblyView *view,
    const UmiDebugInstruction *instructions,
    size_t instruction_count)
{
    UmiDebugInstruction *replacement = NULL;
    size_t position;
    uint64_t next_sequence = 1U;
    size_t current_count = 0U;

    if (view == NULL ||
        (instruction_count != 0U && instructions == NULL) ||
        instruction_count > UMI_DEBUG_DISASSEMBLY_MAXIMUM_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (instruction_count != 0U) {
        if (instruction_count > SIZE_MAX / sizeof(*replacement)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        replacement = (UmiDebugInstruction *)calloc(
            instruction_count, sizeof(*replacement));
        if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        for (position = 0U; position < instruction_count; ++position) {
            if (!valid_instruction(&instructions[position])) {
                free(replacement);
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            replacement[position] = instructions[position];
            normalize_instruction(&replacement[position]);
            if (replacement[position].sequence == 0U) {
                if (next_sequence == UINT64_MAX) {
                    free(replacement);
                    return UMI_STATUS_CAPACITY_EXCEEDED;
                }
                replacement[position].sequence = next_sequence;
                next_sequence += 1U;
            } else if (replacement[position].sequence >= next_sequence) {
                next_sequence = replacement[position].sequence + 1U;
            }
            if (replacement[position].current) current_count += 1U;
        }
        if (duplicate_ids(replacement, instruction_count) || current_count > 1U) {
            free(replacement);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        if (instruction_count > 1U) {
            qsort(replacement, instruction_count, sizeof(replacement[0]),
                  compare_instructions);
        }
    }

    free(view->items);
    view->items = replacement;
    view->count = instruction_count;
    view->capacity = instruction_count;
    view->next_sequence = next_sequence;
    view->selected_instruction_id[0] = '\0';
    view->current_instruction_id[0] = '\0';
    view->revision += 1U;
    for (position = 0U; position < view->count; ++position) {
        view->items[position].revision = view->revision;
        if (view->items[position].current) {
            (void)copy_text(view->current_instruction_id,
                            sizeof(view->current_instruction_id),
                            view->items[position].id);
        }
    }
    if (view->count != 0U) {
        const char *selected = view->current_instruction_id[0] != '\0'
                                   ? view->current_instruction_id
                                   : view->items[0].id;
        (void)copy_text(view->selected_instruction_id,
                        sizeof(view->selected_instruction_id), selected);
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_disassembly_view_upsert(
    UmiDebugDisassemblyView *view,
    const UmiDebugInstruction *instruction)
{
    UmiDebugInstruction copy;
    size_t position;
    size_t scan_position;
    UmiStatus status;
    int existing;

    if (view == NULL || !valid_instruction(instruction)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, instruction->id);
    existing = position != SIZE_MAX;
    if (!existing && instruction->sequence == 0U &&
        view->next_sequence == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (!existing) {
        status = reserve(view, view->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        position = view->count;
        view->count += 1U;
    }
    copy = *instruction;
    normalize_instruction(&copy);
    if (copy.sequence == 0U && existing) {
        copy.sequence = view->items[position].sequence;
    } else if (copy.sequence == 0U) {
        copy.sequence = view->next_sequence;
        view->next_sequence += 1U;
    } else if (copy.sequence >= view->next_sequence) {
        view->next_sequence = copy.sequence + 1U;
    }
    if (copy.current) {
        for (scan_position = 0U; scan_position < view->count; ++scan_position) {
            view->items[scan_position].current = 0;
        }
    }
    view->revision += 1U;
    copy.revision = view->revision;
    view->items[position] = copy;
    if (view->count > 1U) {
        qsort(view->items, view->count, sizeof(view->items[0]),
              compare_instructions);
    }
    if (copy.current) {
        (void)copy_text(view->current_instruction_id,
                        sizeof(view->current_instruction_id), copy.id);
    } else if (strcmp(view->current_instruction_id, copy.id) == 0) {
        view->current_instruction_id[0] = '\0';
    }
    if (view->selected_instruction_id[0] == '\0') {
        (void)copy_text(view->selected_instruction_id,
                        sizeof(view->selected_instruction_id), copy.id);
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_disassembly_view_remove(
    UmiDebugDisassemblyView *view,
    const char *instruction_id)
{
    size_t position;

    if (view == NULL || instruction_id == NULL || instruction_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, instruction_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (position + 1U < view->count) {
        (void)memmove(&view->items[position], &view->items[position + 1U],
                      (view->count - position - 1U) * sizeof(view->items[0]));
    }
    view->count -= 1U;
    (void)memset(&view->items[view->count], 0,
                 sizeof(view->items[view->count]));
    if (strcmp(view->selected_instruction_id, instruction_id) == 0) {
        view->selected_instruction_id[0] = '\0';
        if (view->count != 0U) {
            (void)copy_text(view->selected_instruction_id,
                            sizeof(view->selected_instruction_id),
                            view->items[0].id);
        }
    }
    if (strcmp(view->current_instruction_id, instruction_id) == 0) {
        view->current_instruction_id[0] = '\0';
    }
    view->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_disassembly_view_select(
    UmiDebugDisassemblyView *view,
    const char *instruction_id)
{
    UmiStatus status;

    if (view == NULL || instruction_id == NULL || instruction_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (find_index(view, instruction_id) == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(view->selected_instruction_id, instruction_id) != 0) {
        status = copy_text(view->selected_instruction_id,
                           sizeof(view->selected_instruction_id),
                           instruction_id);
        if (status != UMI_STATUS_OK) return status;
        view->revision += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_disassembly_view_select_address(
    UmiDebugDisassemblyView *view,
    uint64_t address)
{
    size_t position;

    if (view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    position = find_address_index(view, address);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    return umi_debug_disassembly_view_select(view, view->items[position].id);
}

UmiStatus umi_debug_disassembly_view_set_current(
    UmiDebugDisassemblyView *view,
    const char *instruction_id)
{
    size_t selected_position;
    size_t position;

    if (view == NULL || instruction_id == NULL || instruction_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    selected_position = find_index(view, instruction_id);
    if (selected_position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    for (position = 0U; position < view->count; ++position) {
        view->items[position].current = position == selected_position;
    }
    (void)copy_text(view->current_instruction_id,
                    sizeof(view->current_instruction_id), instruction_id);
    (void)copy_text(view->selected_instruction_id,
                    sizeof(view->selected_instruction_id), instruction_id);
    view->revision += 1U;
    view->items[selected_position].revision = view->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_disassembly_view_set_breakpoint(
    UmiDebugDisassemblyView *view,
    const char *instruction_id,
    int has_breakpoint)
{
    size_t position;
    int normalized;

    if (view == NULL || instruction_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, instruction_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (!view->items[position].can_breakpoint) {
        return UMI_STATUS_UNAVAILABLE;
    }
    normalized = has_breakpoint != 0;
    if (view->items[position].breakpoint != normalized) {
        view->items[position].breakpoint = normalized;
        view->revision += 1U;
        view->items[position].revision = view->revision;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_disassembly_view_find(
    const UmiDebugDisassemblyView *view,
    const char *instruction_id,
    UmiDebugInstruction *out_instruction)
{
    size_t position;

    if (view == NULL || instruction_id == NULL || out_instruction == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, instruction_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_instruction = view->items[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_disassembly_view_find_address(
    const UmiDebugDisassemblyView *view,
    uint64_t address,
    UmiDebugInstruction *out_instruction)
{
    size_t position;

    if (view == NULL || out_instruction == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_address_index(view, address);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_instruction = view->items[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_disassembly_view_at(
    const UmiDebugDisassemblyView *view,
    size_t index,
    UmiDebugInstruction *out_instruction)
{
    if (view == NULL || out_instruction == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= view->count) return UMI_STATUS_NOT_FOUND;
    *out_instruction = view->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_disassembly_view_selected(
    const UmiDebugDisassemblyView *view,
    UmiDebugInstruction *out_instruction)
{
    if (view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_debug_disassembly_view_find(
        view, view->selected_instruction_id, out_instruction);
}

UmiStatus umi_debug_disassembly_view_current(
    const UmiDebugDisassemblyView *view,
    UmiDebugInstruction *out_instruction)
{
    if (view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_debug_disassembly_view_find(
        view, view->current_instruction_id, out_instruction);
}

UmiStatus umi_debug_disassembly_view_snapshot(
    const UmiDebugDisassemblyView *view,
    UmiDebugDisassemblySnapshot *out_snapshot)
{
    size_t position;

    if (view == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEBUG_DISASSEMBLY_VIEW_API_VERSION;
    out_snapshot->instruction_count = view->count;
    out_snapshot->revision = view->revision;
    (void)copy_text(out_snapshot->selected_instruction_id,
                    sizeof(out_snapshot->selected_instruction_id),
                    view->selected_instruction_id);
    (void)copy_text(out_snapshot->current_instruction_id,
                    sizeof(out_snapshot->current_instruction_id),
                    view->current_instruction_id);
    out_snapshot->has_selection = view->selected_instruction_id[0] != '\0';
    out_snapshot->has_current_instruction =
        view->current_instruction_id[0] != '\0';
    if (view->count != 0U) {
        out_snapshot->minimum_address = view->items[0].address;
        out_snapshot->maximum_address = view->items[0].address;
    }
    for (position = 0U; position < view->count; ++position) {
        const UmiDebugInstruction *instruction = &view->items[position];
        if (instruction->has_source) {
            out_snapshot->source_correlated_count += 1U;
        }
        if (instruction->breakpoint) out_snapshot->breakpoint_count += 1U;
        if (!instruction->valid) {
            out_snapshot->invalid_instruction_count += 1U;
        }
        if (instruction->address < out_snapshot->minimum_address) {
            out_snapshot->minimum_address = instruction->address;
        }
        if (instruction->address > out_snapshot->maximum_address) {
            out_snapshot->maximum_address = instruction->address;
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_debug_disassembly_view_count(const UmiDebugDisassemblyView *view)
{
    return view != NULL ? view->count : 0U;
}

uint64_t umi_debug_disassembly_view_revision(
    const UmiDebugDisassemblyView *view)
{
    return view != NULL ? view->revision : 0U;
}
