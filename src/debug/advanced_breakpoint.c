/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/advanced_breakpoint.c
 *
 * PURPOSE:
 *   Implement bounded, deterministic storage and filtering for function, data,
 *   instruction and exception breakpoints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/advanced_breakpoint.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiDebugAdvancedBreakpointRegistry {
    UmiDebugAdvancedBreakpoint *items;
    size_t count;
    size_t capacity;
    uint64_t next_sequence;
    uint64_t revision;
};

static int valid_kind(UmiDebugAdvancedBreakpointKind kind)
{
    return kind >= UMI_DEBUG_ADVANCED_BREAKPOINT_FUNCTION &&
           kind <= UMI_DEBUG_ADVANCED_BREAKPOINT_EXCEPTION;
}

static int valid_access(UmiDebugDataBreakpointAccess access)
{
    return access >= UMI_DEBUG_DATA_BREAKPOINT_ACCESS_READ &&
           access <= UMI_DEBUG_DATA_BREAKPOINT_ACCESS_READ_WRITE;
}

static int has_terminator(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static int valid_breakpoint(const UmiDebugAdvancedBreakpoint *breakpoint)
{
    if (breakpoint == NULL ||
        breakpoint->struct_size != (uint32_t)sizeof(*breakpoint) ||
        breakpoint->api_version != UMI_DEBUG_ADVANCED_BREAKPOINT_API_VERSION ||
        !valid_kind(breakpoint->kind) ||
        !has_terminator(breakpoint->id, sizeof(breakpoint->id)) ||
        breakpoint->id[0] == '\0' ||
        !has_terminator(breakpoint->session_id,
                        sizeof(breakpoint->session_id)) ||
        !has_terminator(breakpoint->name, sizeof(breakpoint->name)) ||
        !has_terminator(breakpoint->reference,
                        sizeof(breakpoint->reference)) ||
        !has_terminator(breakpoint->condition,
                        sizeof(breakpoint->condition)) ||
        !has_terminator(breakpoint->hit_condition,
                        sizeof(breakpoint->hit_condition)) ||
        !has_terminator(breakpoint->log_message,
                        sizeof(breakpoint->log_message)) ||
        !has_terminator(breakpoint->message,
                        sizeof(breakpoint->message)) ||
        breakpoint->sequence == UINT64_MAX) {
        return 0;
    }
    if (breakpoint->kind == UMI_DEBUG_ADVANCED_BREAKPOINT_FUNCTION &&
        breakpoint->name[0] == '\0') {
        return 0;
    }
    if (breakpoint->kind == UMI_DEBUG_ADVANCED_BREAKPOINT_DATA &&
        (breakpoint->reference[0] == '\0' ||
         !valid_access(breakpoint->access))) {
        return 0;
    }
    if (breakpoint->kind == UMI_DEBUG_ADVANCED_BREAKPOINT_INSTRUCTION &&
        breakpoint->reference[0] == '\0' && breakpoint->address == 0U) {
        return 0;
    }
    if (breakpoint->kind == UMI_DEBUG_ADVANCED_BREAKPOINT_EXCEPTION &&
        breakpoint->reference[0] == '\0') {
        return 0;
    }
    return 1;
}

static size_t find_index(
    const UmiDebugAdvancedBreakpointRegistry *registry,
    const char *breakpoint_id)
{
    size_t position;

    if (registry == NULL || breakpoint_id == NULL) return SIZE_MAX;
    for (position = 0U; position < registry->count; ++position) {
        if (strcmp(registry->items[position].id, breakpoint_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

static UmiStatus reserve(
    UmiDebugAdvancedBreakpointRegistry *registry,
    size_t required_capacity)
{
    UmiDebugAdvancedBreakpoint *replacement;
    size_t new_capacity;

    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (required_capacity <= registry->capacity) return UMI_STATUS_OK;
    if (required_capacity > UMI_DEBUG_ADVANCED_BREAKPOINT_MAXIMUM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    new_capacity = registry->capacity;
    if (new_capacity == 0U) {
        new_capacity = UMI_DEBUG_ADVANCED_BREAKPOINT_DEFAULT_CAPACITY;
    }
    while (new_capacity < required_capacity) {
        if (new_capacity >
            UMI_DEBUG_ADVANCED_BREAKPOINT_MAXIMUM_CAPACITY / 2U) {
            new_capacity = UMI_DEBUG_ADVANCED_BREAKPOINT_MAXIMUM_CAPACITY;
        } else {
            new_capacity *= 2U;
        }
    }
    if (new_capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiDebugAdvancedBreakpoint *)realloc(
        registry->items, new_capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    if (new_capacity > registry->capacity) {
        (void)memset(&replacement[registry->capacity], 0,
                     (new_capacity - registry->capacity) *
                         sizeof(*replacement));
    }
    registry->items = replacement;
    registry->capacity = new_capacity;
    return UMI_STATUS_OK;
}

static unsigned char lower_ascii(unsigned char value)
{
    return (unsigned char)tolower((int)value);
}

static int contains_text(const char *text, const char *needle)
{
    size_t text_length;
    size_t needle_length;
    size_t start;
    size_t offset;

    if (needle == NULL || needle[0] == '\0') return 1;
    if (text == NULL) return 0;
    text_length = strlen(text);
    needle_length = strlen(needle);
    if (needle_length > text_length) return 0;

    for (start = 0U; start + needle_length <= text_length; ++start) {
        for (offset = 0U; offset < needle_length; ++offset) {
            if (lower_ascii((unsigned char)text[start + offset]) !=
                lower_ascii((unsigned char)needle[offset])) {
                break;
            }
        }
        if (offset == needle_length) return 1;
    }
    return 0;
}

static int matches_filter(
    const UmiDebugAdvancedBreakpoint *breakpoint,
    const UmiDebugAdvancedBreakpointFilter *filter)
{
    if (filter == NULL) return 1;
    if (filter->struct_size != (uint32_t)sizeof(*filter) ||
        filter->api_version != UMI_DEBUG_ADVANCED_BREAKPOINT_API_VERSION) {
        return 0;
    }
    if (filter->kind != 0 && breakpoint->kind != filter->kind) return 0;
    if (filter->session_id != NULL && filter->session_id[0] != '\0' &&
        strcmp(breakpoint->session_id, filter->session_id) != 0) {
        return 0;
    }
    if (filter->enabled_only && !breakpoint->enabled) return 0;
    if (filter->verified_only && !breakpoint->verified) return 0;
    if (filter->text != NULL && filter->text[0] != '\0' &&
        !contains_text(breakpoint->id, filter->text) &&
        !contains_text(breakpoint->name, filter->text) &&
        !contains_text(breakpoint->reference, filter->text) &&
        !contains_text(breakpoint->condition, filter->text) &&
        !contains_text(breakpoint->message, filter->text)) {
        return 0;
    }
    return 1;
}

static int compare_breakpoints(const void *left_pointer,
                               const void *right_pointer)
{
    const UmiDebugAdvancedBreakpoint *left =
        (const UmiDebugAdvancedBreakpoint *)left_pointer;
    const UmiDebugAdvancedBreakpoint *right =
        (const UmiDebugAdvancedBreakpoint *)right_pointer;
    int text_order;

    if (left->kind < right->kind) return -1;
    if (left->kind > right->kind) return 1;
    if (left->sequence < right->sequence) return -1;
    if (left->sequence > right->sequence) return 1;
    text_order = strcmp(left->id, right->id);
    return text_order < 0 ? -1 : (text_order > 0 ? 1 : 0);
}

UmiStatus umi_debug_advanced_breakpoint_registry_create(
    size_t initial_capacity,
    UmiDebugAdvancedBreakpointRegistry **out_registry)
{
    UmiDebugAdvancedBreakpointRegistry *registry;
    UmiStatus status;

    if (out_registry == NULL ||
        initial_capacity > UMI_DEBUG_ADVANCED_BREAKPOINT_MAXIMUM_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_registry = NULL;
    registry = (UmiDebugAdvancedBreakpointRegistry *)calloc(
        1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    registry->next_sequence = 1U;

    if (initial_capacity != 0U) {
        status = reserve(registry, initial_capacity);
        if (status != UMI_STATUS_OK) {
            free(registry);
            return status;
        }
    }
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_debug_advanced_breakpoint_registry_destroy(
    UmiDebugAdvancedBreakpointRegistry *registry)
{
    if (registry == NULL) return;
    free(registry->items);
    (void)memset(registry, 0, sizeof(*registry));
    free(registry);
}

UmiStatus umi_debug_advanced_breakpoint_registry_clear(
    UmiDebugAdvancedBreakpointRegistry *registry)
{
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (registry->items != NULL) {
        (void)memset(registry->items, 0,
                     registry->capacity * sizeof(registry->items[0]));
    }
    registry->count = 0U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_advanced_breakpoint_registry_upsert(
    UmiDebugAdvancedBreakpointRegistry *registry,
    const UmiDebugAdvancedBreakpoint *breakpoint)
{
    UmiDebugAdvancedBreakpoint copy;
    size_t position;
    UmiStatus status;
    int existing;

    if (registry == NULL || !valid_breakpoint(breakpoint)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(registry, breakpoint->id);
    existing = position != SIZE_MAX;
    if (!existing && breakpoint->sequence == 0U &&
        registry->next_sequence == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (!existing) {
        status = reserve(registry, registry->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        position = registry->count;
        registry->count += 1U;
    }

    copy = *breakpoint;
    copy.struct_size = (uint32_t)sizeof(copy);
    copy.api_version = UMI_DEBUG_ADVANCED_BREAKPOINT_API_VERSION;
    copy.id[sizeof(copy.id) - 1U] = '\0';
    copy.session_id[sizeof(copy.session_id) - 1U] = '\0';
    copy.name[sizeof(copy.name) - 1U] = '\0';
    copy.reference[sizeof(copy.reference) - 1U] = '\0';
    copy.condition[sizeof(copy.condition) - 1U] = '\0';
    copy.hit_condition[sizeof(copy.hit_condition) - 1U] = '\0';
    copy.log_message[sizeof(copy.log_message) - 1U] = '\0';
    copy.message[sizeof(copy.message) - 1U] = '\0';
    if (copy.sequence == 0U && existing) {
        copy.sequence = registry->items[position].sequence;
    } else if (copy.sequence == 0U) {
        copy.sequence = registry->next_sequence;
        registry->next_sequence += 1U;
    } else if (copy.sequence >= registry->next_sequence) {
        registry->next_sequence = copy.sequence + 1U;
    }
    registry->revision += 1U;
    copy.revision = registry->revision;
    registry->items[position] = copy;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_advanced_breakpoint_registry_remove(
    UmiDebugAdvancedBreakpointRegistry *registry,
    const char *breakpoint_id)
{
    size_t position;

    if (registry == NULL || breakpoint_id == NULL ||
        breakpoint_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(registry, breakpoint_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;

    if (position + 1U < registry->count) {
        (void)memmove(&registry->items[position],
                      &registry->items[position + 1U],
                      (registry->count - position - 1U) *
                          sizeof(registry->items[0]));
    }
    registry->count -= 1U;
    (void)memset(&registry->items[registry->count], 0,
                 sizeof(registry->items[registry->count]));
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_advanced_breakpoint_registry_remove_session(
    UmiDebugAdvancedBreakpointRegistry *registry,
    const char *session_id,
    size_t *out_removed_count)
{
    size_t read_position;
    size_t write_position = 0U;
    size_t removed_count = 0U;

    if (registry == NULL || session_id == NULL || session_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (read_position = 0U; read_position < registry->count;
         ++read_position) {
        if (strcmp(registry->items[read_position].session_id, session_id) == 0) {
            removed_count += 1U;
            continue;
        }
        if (write_position != read_position) {
            registry->items[write_position] = registry->items[read_position];
        }
        write_position += 1U;
    }
    if (removed_count != 0U) {
        (void)memset(&registry->items[write_position], 0,
                     removed_count * sizeof(registry->items[0]));
        registry->count = write_position;
        registry->revision += 1U;
    }
    if (out_removed_count != NULL) *out_removed_count = removed_count;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_advanced_breakpoint_registry_set_enabled(
    UmiDebugAdvancedBreakpointRegistry *registry,
    const char *breakpoint_id,
    int enabled)
{
    size_t position;
    int normalized;

    if (registry == NULL || breakpoint_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(registry, breakpoint_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    normalized = enabled != 0;
    if (registry->items[position].enabled != normalized) {
        registry->items[position].enabled = normalized;
        registry->revision += 1U;
        registry->items[position].revision = registry->revision;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_advanced_breakpoint_registry_find(
    const UmiDebugAdvancedBreakpointRegistry *registry,
    const char *breakpoint_id,
    UmiDebugAdvancedBreakpoint *out_breakpoint)
{
    size_t position;

    if (registry == NULL || breakpoint_id == NULL || out_breakpoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(registry, breakpoint_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_breakpoint = registry->items[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_advanced_breakpoint_registry_at(
    const UmiDebugAdvancedBreakpointRegistry *registry,
    size_t index,
    UmiDebugAdvancedBreakpoint *out_breakpoint)
{
    if (registry == NULL || out_breakpoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_breakpoint = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_advanced_breakpoint_registry_query(
    const UmiDebugAdvancedBreakpointRegistry *registry,
    const UmiDebugAdvancedBreakpointFilter *filter,
    UmiDebugAdvancedBreakpoint *out_breakpoints,
    size_t breakpoint_capacity,
    size_t *out_breakpoint_count)
{
    size_t position;
    size_t matched_count = 0U;

    if (registry == NULL || out_breakpoint_count == NULL ||
        (breakpoint_capacity != 0U && out_breakpoints == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (filter != NULL &&
        (filter->struct_size != (uint32_t)sizeof(*filter) ||
         filter->api_version != UMI_DEBUG_ADVANCED_BREAKPOINT_API_VERSION ||
         (filter->kind != 0 && !valid_kind(filter->kind)))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (position = 0U; position < registry->count; ++position) {
        if (!matches_filter(&registry->items[position], filter)) continue;
        if (matched_count >= breakpoint_capacity) {
            *out_breakpoint_count = matched_count;
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        out_breakpoints[matched_count] = registry->items[position];
        matched_count += 1U;
    }
    if (matched_count > 1U) {
        qsort(out_breakpoints, matched_count, sizeof(out_breakpoints[0]),
              compare_breakpoints);
    }
    *out_breakpoint_count = matched_count;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_advanced_breakpoint_registry_snapshot(
    const UmiDebugAdvancedBreakpointRegistry *registry,
    UmiDebugAdvancedBreakpointSnapshot *out_snapshot)
{
    size_t position;

    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEBUG_ADVANCED_BREAKPOINT_API_VERSION;
    out_snapshot->breakpoint_count = registry->count;
    out_snapshot->revision = registry->revision;

    for (position = 0U; position < registry->count; ++position) {
        const UmiDebugAdvancedBreakpoint *breakpoint =
            &registry->items[position];
        switch (breakpoint->kind) {
            case UMI_DEBUG_ADVANCED_BREAKPOINT_FUNCTION:
                out_snapshot->function_count += 1U;
                break;
            case UMI_DEBUG_ADVANCED_BREAKPOINT_DATA:
                out_snapshot->data_count += 1U;
                break;
            case UMI_DEBUG_ADVANCED_BREAKPOINT_INSTRUCTION:
                out_snapshot->instruction_count += 1U;
                break;
            case UMI_DEBUG_ADVANCED_BREAKPOINT_EXCEPTION:
                out_snapshot->exception_count += 1U;
                break;
            default:
                break;
        }
        if (breakpoint->enabled) out_snapshot->enabled_count += 1U;
        if (breakpoint->verified) out_snapshot->verified_count += 1U;
    }
    return UMI_STATUS_OK;
}

size_t umi_debug_advanced_breakpoint_registry_count(
    const UmiDebugAdvancedBreakpointRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_debug_advanced_breakpoint_registry_revision(
    const UmiDebugAdvancedBreakpointRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
