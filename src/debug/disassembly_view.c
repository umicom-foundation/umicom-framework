/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/disassembly_view.c
 *
 * PURPOSE:
 *   Implement deterministic disassembly storage, selection, source correlation
 *   and instruction-breakpoint state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the has terminator operation used by this module and its client applications. */
static int has_terminator(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t destination_capacity,
                           const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || destination_capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= destination_capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the valid instruction operation used by this module and its client applications. */
static int valid_instruction(const UmiDebugInstruction *instruction)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Apply this operation only while the related capability or state is available. */
    if (instruction->valid && instruction->mnemonic[0] == '\0') return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (instruction->has_source && instruction->source_uri[0] == '\0') return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (instruction->breakpoint && !instruction->can_breakpoint) return 0;
    return 1;
}

/*
 * Provide the normalize instruction operation used by this module and its client
 * applications.
 */
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

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiDebugDisassemblyView *view,
                         const char *instruction_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || instruction_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < view->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(view->items[position].id, instruction_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

/*
 * Provide the find address index operation used by this module and its client
 * applications.
 */
static size_t find_address_index(const UmiDebugDisassemblyView *view,
                                 uint64_t address)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < view->count; ++position) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (view->items[position].address == address) return position;
    }
    return SIZE_MAX;
}

/* Provide the reserve operation used by this module and its client applications. */
static UmiStatus reserve(UmiDebugDisassemblyView *view,
                         size_t required_capacity)
{
    UmiDebugInstruction *replacement;
    size_t new_capacity;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required_capacity <= view->capacity) return UMI_STATUS_OK;
    /* Apply this branch only when its contract condition is satisfied. */
    if (required_capacity > UMI_DEBUG_DISASSEMBLY_MAXIMUM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = view->capacity;
    /* Apply this branch only when its contract condition is satisfied. */
    if (new_capacity == 0U) new_capacity = UMI_DEBUG_DISASSEMBLY_DEFAULT_CAPACITY;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (new_capacity < required_capacity) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (new_capacity > UMI_DEBUG_DISASSEMBLY_MAXIMUM_CAPACITY / 2U) {
            new_capacity = UMI_DEBUG_DISASSEMBLY_MAXIMUM_CAPACITY;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            new_capacity *= 2U;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (new_capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiDebugInstruction *)realloc(
        view->items, new_capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memset(&replacement[view->capacity], 0,
                 (new_capacity - view->capacity) * sizeof(*replacement));
    view->items = replacement;
    view->capacity = new_capacity;
    return UMI_STATUS_OK;
}

/*
 * Provide the compare instructions operation used by this module and its client
 * applications.
 */
static int compare_instructions(const void *left_pointer,
                                const void *right_pointer)
{
    const UmiDebugInstruction *left =
        (const UmiDebugInstruction *)left_pointer;
    const UmiDebugInstruction *right =
        (const UmiDebugInstruction *)right_pointer;
    int order;

    /* Apply this branch only when its contract condition is satisfied. */
    if (left->address < right->address) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->address > right->address) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->instruction_offset < right->instruction_offset) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->instruction_offset > right->instruction_offset) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->sequence < right->sequence) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->sequence > right->sequence) return 1;
    order = strcmp(left->id, right->id);
    return order < 0 ? -1 : (order > 0 ? 1 : 0);
}

/* Provide the duplicate ids operation used by this module and its client applications. */
static int duplicate_ids(const UmiDebugInstruction *items, size_t count)
{
    size_t left_position;
    size_t right_position;

    /* Visit each bounded item once so every record receives the same rule. */
    for (left_position = 0U; left_position < count; ++left_position) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (right_position = left_position + 1U; right_position < count;
             ++right_position) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(items[left_position].id, items[right_position].id) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

/*
 * Initialise debug disassembly view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_disassembly_view_create(
    UmiDebugDisassemblyView **out_view)
{
    UmiDebugDisassemblyView *view;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    view = (UmiDebugDisassemblyView *)calloc(1U, sizeof(*view));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    view->next_sequence = 1U;
    view->revision = 1U;
    *out_view = view;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by debug disassembly view so the same storage can be reused
 * safely.
 */
void umi_debug_disassembly_view_destroy(UmiDebugDisassemblyView *view)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return;
    free(view->items);
    (void)memset(view, 0, sizeof(*view));
    free(view);
}

/*
 * Release or reset state held by debug disassembly view so the same storage can be reused
 * safely.
 */
UmiStatus umi_debug_disassembly_view_clear(UmiDebugDisassemblyView *view)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Read debug disassembly view into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_debug_disassembly_view_load(
    UmiDebugDisassemblyView *view,
    const UmiDebugInstruction *instructions,
    size_t instruction_count)
{
    UmiDebugInstruction *replacement = NULL;
    size_t position;
    uint64_t next_sequence = 1U;
    size_t current_count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL ||
        (instruction_count != 0U && instructions == NULL) ||
        instruction_count > UMI_DEBUG_DISASSEMBLY_MAXIMUM_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (instruction_count != 0U) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (instruction_count > SIZE_MAX / sizeof(*replacement)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        replacement = (UmiDebugInstruction *)calloc(
            instruction_count, sizeof(*replacement));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        /* Visit each bounded item once so every record receives the same rule. */
        for (position = 0U; position < instruction_count; ++position) {
            /* Apply this operation only while the related capability or state is available. */
            if (!valid_instruction(&instructions[position])) {
                free(replacement);
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            replacement[position] = instructions[position];
            normalize_instruction(&replacement[position]);
            /* Apply this branch only when its contract condition is satisfied. */
            if (replacement[position].sequence == 0U) {
                /* Apply this branch only when its contract condition is satisfied. */
                if (next_sequence == UINT64_MAX) {
                    free(replacement);
                    return UMI_STATUS_CAPACITY_EXCEEDED;
                }
                replacement[position].sequence = next_sequence;
                next_sequence += 1U;
            } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (replacement[position].sequence >= next_sequence) {
                next_sequence = replacement[position].sequence + 1U;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (replacement[position].current) current_count += 1U;
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (duplicate_ids(replacement, instruction_count) || current_count > 1U) {
            free(replacement);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        /* Apply this branch only when its contract condition is satisfied. */
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
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < view->count; ++position) {
        view->items[position].revision = view->revision;
        /* Apply this branch only when its contract condition is satisfied. */
        if (view->items[position].current) {
            (void)copy_text(view->current_instruction_id,
                            sizeof(view->current_instruction_id),
                            view->items[position].id);
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (view->count != 0U) {
        const char *selected = view->current_instruction_id[0] != '\0'
                                   ? view->current_instruction_id
                                   : view->items[0].id;
        (void)copy_text(view->selected_instruction_id,
                        sizeof(view->selected_instruction_id), selected);
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the debug disassembly view upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_disassembly_view_upsert(
    UmiDebugDisassemblyView *view,
    const UmiDebugInstruction *instruction)
{
    UmiDebugInstruction copy;
    size_t position;
    size_t scan_position;
    UmiStatus status;
    int existing;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || !valid_instruction(instruction)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, instruction->id);
    existing = position != SIZE_MAX;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!existing && instruction->sequence == 0U &&
        view->next_sequence == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!existing) {
        status = reserve(view, view->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        position = view->count;
        view->count += 1U;
    }
    copy = *instruction;
    normalize_instruction(&copy);
    /* Apply this branch only when its contract condition is satisfied. */
    if (copy.sequence == 0U && existing) {
        copy.sequence = view->items[position].sequence;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (copy.sequence == 0U) {
        copy.sequence = view->next_sequence;
        view->next_sequence += 1U;
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (copy.sequence >= view->next_sequence) {
        view->next_sequence = copy.sequence + 1U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (copy.current) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (scan_position = 0U; scan_position < view->count; ++scan_position) {
            view->items[scan_position].current = 0;
        }
    }
    view->revision += 1U;
    copy.revision = view->revision;
    view->items[position] = copy;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (view->count > 1U) {
        qsort(view->items, view->count, sizeof(view->items[0]),
              compare_instructions);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (copy.current) {
        (void)copy_text(view->current_instruction_id,
                        sizeof(view->current_instruction_id), copy.id);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(view->current_instruction_id, copy.id) == 0) {
        view->current_instruction_id[0] = '\0';
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (view->selected_instruction_id[0] == '\0') {
        (void)copy_text(view->selected_instruction_id,
                        sizeof(view->selected_instruction_id), copy.id);
    }
    return UMI_STATUS_OK;
}

/*
 * Remove debug disassembly view while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_disassembly_view_remove(
    UmiDebugDisassemblyView *view,
    const char *instruction_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || instruction_id == NULL || instruction_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, instruction_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position + 1U < view->count) {
        (void)memmove(&view->items[position], &view->items[position + 1U],
                      (view->count - position - 1U) * sizeof(view->items[0]));
    }
    view->count -= 1U;
    (void)memset(&view->items[view->count], 0,
                 sizeof(view->items[view->count]));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(view->selected_instruction_id, instruction_id) == 0) {
        view->selected_instruction_id[0] = '\0';
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (view->count != 0U) {
            (void)copy_text(view->selected_instruction_id,
                            sizeof(view->selected_instruction_id),
                            view->items[0].id);
        }
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(view->current_instruction_id, instruction_id) == 0) {
        view->current_instruction_id[0] = '\0';
    }
    view->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug disassembly view select operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_disassembly_view_select(
    UmiDebugDisassemblyView *view,
    const char *instruction_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || instruction_id == NULL || instruction_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_index(view, instruction_id) == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(view->selected_instruction_id, instruction_id) != 0) {
        status = copy_text(view->selected_instruction_id,
                           sizeof(view->selected_instruction_id),
                           instruction_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        view->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the debug disassembly view select address operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_disassembly_view_select_address(
    UmiDebugDisassemblyView *view,
    uint64_t address)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    position = find_address_index(view, address);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    return umi_debug_disassembly_view_select(view, view->items[position].id);
}

/*
 * Provide the debug disassembly view set current operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_disassembly_view_set_current(
    UmiDebugDisassemblyView *view,
    const char *instruction_id)
{
    size_t selected_position;
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || instruction_id == NULL || instruction_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    selected_position = find_index(view, instruction_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (selected_position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
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

/*
 * Provide the debug disassembly view set breakpoint operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_disassembly_view_set_breakpoint(
    UmiDebugDisassemblyView *view,
    const char *instruction_id,
    int has_breakpoint)
{
    size_t position;
    int normalized;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || instruction_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, instruction_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!view->items[position].can_breakpoint) {
        return UMI_STATUS_UNAVAILABLE;
    }
    normalized = has_breakpoint != 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (view->items[position].breakpoint != normalized) {
        view->items[position].breakpoint = normalized;
        view->revision += 1U;
        view->items[position].revision = view->revision;
    }
    return UMI_STATUS_OK;
}

/*
 * Find debug disassembly view while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_disassembly_view_find(
    const UmiDebugDisassemblyView *view,
    const char *instruction_id,
    UmiDebugInstruction *out_instruction)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || instruction_id == NULL || out_instruction == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(view, instruction_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_instruction = view->items[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the debug disassembly view find address operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_disassembly_view_find_address(
    const UmiDebugDisassemblyView *view,
    uint64_t address,
    UmiDebugInstruction *out_instruction)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || out_instruction == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_address_index(view, address);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_instruction = view->items[position];
    return UMI_STATUS_OK;
}

/*
 * Find debug disassembly view while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_disassembly_view_at(
    const UmiDebugDisassemblyView *view,
    size_t index,
    UmiDebugInstruction *out_instruction)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || out_instruction == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= view->count) return UMI_STATUS_NOT_FOUND;
    *out_instruction = view->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find debug disassembly view while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_disassembly_view_selected(
    const UmiDebugDisassemblyView *view,
    UmiDebugInstruction *out_instruction)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_debug_disassembly_view_find(
        view, view->selected_instruction_id, out_instruction);
}

/*
 * Provide the debug disassembly view current operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_disassembly_view_current(
    const UmiDebugDisassemblyView *view,
    UmiDebugInstruction *out_instruction)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_debug_disassembly_view_find(
        view, view->current_instruction_id, out_instruction);
}

/*
 * Provide the debug disassembly view snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_disassembly_view_snapshot(
    const UmiDebugDisassemblyView *view,
    UmiDebugDisassemblySnapshot *out_snapshot)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (view->count != 0U) {
        out_snapshot->minimum_address = view->items[0].address;
        out_snapshot->maximum_address = view->items[0].address;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < view->count; ++position) {
        const UmiDebugInstruction *instruction = &view->items[position];
        /* Apply this branch only when its contract condition is satisfied. */
        if (instruction->has_source) {
            out_snapshot->source_correlated_count += 1U;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (instruction->breakpoint) out_snapshot->breakpoint_count += 1U;
        /* Apply this operation only while the related capability or state is available. */
        if (!instruction->valid) {
            out_snapshot->invalid_instruction_count += 1U;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (instruction->address < out_snapshot->minimum_address) {
            out_snapshot->minimum_address = instruction->address;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (instruction->address > out_snapshot->maximum_address) {
            out_snapshot->maximum_address = instruction->address;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by debug disassembly view without changing
 * their state.
 */
size_t umi_debug_disassembly_view_count(const UmiDebugDisassemblyView *view)
{
    return view != NULL ? view->count : 0U;
}

/*
 * Provide the debug disassembly view revision operation used by this module and its client
 * applications.
 */
uint64_t umi_debug_disassembly_view_revision(
    const UmiDebugDisassemblyView *view)
{
    return view != NULL ? view->revision : 0U;
}
