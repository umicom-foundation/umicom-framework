/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/thread_inspector.c
 *
 * PURPOSE:
 *   Implement a coherent thread, call-stack, scope and variable projection over
 *   the existing debugger service registries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/thread_inspector.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiDebugThreadInspector {
    UmiDebugThreadSnapshot *threads;
    size_t thread_count;
    size_t thread_capacity;
    UmiDebugStackFrameSnapshot *frames;
    size_t frame_count;
    size_t frame_capacity;
    UmiDebugScopeSnapshot *scopes;
    size_t scope_count;
    size_t scope_capacity;
    UmiDebugVariableSnapshot *variables;
    size_t variable_count;
    size_t variable_capacity;
    UmiDebugThreadInspectorFilter filter;
    char selected_thread_id[UMI_DEBUG_THREAD_INSPECTOR_SELECTION_ID_CAPACITY];
    char selected_frame_id[UMI_DEBUG_THREAD_INSPECTOR_SELECTION_ID_CAPACITY];
    char selected_scope_id[UMI_DEBUG_THREAD_INSPECTOR_SELECTION_ID_CAPACITY];
    uint64_t thread_registry_revision;
    uint64_t frame_registry_revision;
    uint64_t scope_registry_revision;
    uint64_t variable_registry_revision;
    uint64_t revision;
    int follow_current_thread;
};

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

/* Provide the lower ascii operation used by this module and its client applications. */
static unsigned char lower_ascii(unsigned char value)
{
    return (unsigned char)tolower((int)value);
}

/* Provide the contains text operation used by this module and its client applications. */
static int contains_text(const char *text, const char *needle)
{
    size_t text_length;
    size_t needle_length;
    size_t start;
    size_t offset;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (needle == NULL || needle[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    text_length = strlen(text);
    needle_length = strlen(needle);
    /* Apply this branch only when its contract condition is satisfied. */
    if (needle_length > text_length) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (start = 0U; start + needle_length <= text_length; ++start) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (offset = 0U; offset < needle_length; ++offset) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (lower_ascii((unsigned char)text[start + offset]) !=
                lower_ascii((unsigned char)needle[offset])) {
                break;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (offset == needle_length) return 1;
    }
    return 0;
}

/* Provide the reserve items operation used by this module and its client applications. */
static UmiStatus reserve_items(void **items,
                               size_t *capacity,
                               size_t required_capacity,
                               size_t item_size)
{
    void *replacement;
    size_t new_capacity;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL || capacity == NULL || item_size == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required_capacity <= *capacity) return UMI_STATUS_OK;
    new_capacity = *capacity == 0U ? 16U : *capacity;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (new_capacity < required_capacity) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (new_capacity > SIZE_MAX / 2U) {
            new_capacity = required_capacity;
            break;
        }
        new_capacity *= 2U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (new_capacity < required_capacity || new_capacity > SIZE_MAX / item_size) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = realloc(*items, new_capacity * item_size);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (new_capacity > *capacity) {
        (void)memset((unsigned char *)replacement + (*capacity * item_size), 0,
                     (new_capacity - *capacity) * item_size);
    }
    *items = replacement;
    *capacity = new_capacity;
    return UMI_STATUS_OK;
}

/* Provide the compare threads operation used by this module and its client applications. */
static int compare_threads(const void *left_pointer,
                           const void *right_pointer)
{
    const UmiDebugThreadSnapshot *left =
        (const UmiDebugThreadSnapshot *)left_pointer;
    const UmiDebugThreadSnapshot *right =
        (const UmiDebugThreadSnapshot *)right_pointer;
    int order;

    /* Apply this branch only when its contract condition is satisfied. */
    if (left->current != right->current) return left->current ? -1 : 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->stopped != right->stopped) return left->stopped ? -1 : 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (left->native_id < right->native_id) return -1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (left->native_id > right->native_id) return 1;
    order = strcmp(left->name, right->name);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order < 0 ? -1 : 1;
    order = strcmp(left->id, right->id);
    return order < 0 ? -1 : (order > 0 ? 1 : 0);
}

/* Provide the compare frames operation used by this module and its client applications. */
static int compare_frames(const void *left_pointer,
                          const void *right_pointer)
{
    const UmiDebugStackFrameSnapshot *left =
        (const UmiDebugStackFrameSnapshot *)left_pointer;
    const UmiDebugStackFrameSnapshot *right =
        (const UmiDebugStackFrameSnapshot *)right_pointer;
    int order;

    order = strcmp(left->thread_id, right->thread_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order < 0 ? -1 : 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->order < right->order) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->order > right->order) return 1;
    order = strcmp(left->id, right->id);
    return order < 0 ? -1 : (order > 0 ? 1 : 0);
}

/* Provide the compare scopes operation used by this module and its client applications. */
static int compare_scopes(const void *left_pointer,
                          const void *right_pointer)
{
    const UmiDebugScopeSnapshot *left =
        (const UmiDebugScopeSnapshot *)left_pointer;
    const UmiDebugScopeSnapshot *right =
        (const UmiDebugScopeSnapshot *)right_pointer;
    int order;

    order = strcmp(left->frame_id, right->frame_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order < 0 ? -1 : 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->order < right->order) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->order > right->order) return 1;
    order = strcmp(left->name, right->name);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order < 0 ? -1 : 1;
    order = strcmp(left->id, right->id);
    return order < 0 ? -1 : (order > 0 ? 1 : 0);
}

/* Provide the compare variables operation used by this module and its client applications. */
static int compare_variables(const void *left_pointer,
                             const void *right_pointer)
{
    const UmiDebugVariableSnapshot *left =
        (const UmiDebugVariableSnapshot *)left_pointer;
    const UmiDebugVariableSnapshot *right =
        (const UmiDebugVariableSnapshot *)right_pointer;
    int order;

    order = strcmp(left->scope_id, right->scope_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order < 0 ? -1 : 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->changed != right->changed) return left->changed ? -1 : 1;
    order = strcmp(left->name, right->name);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order < 0 ? -1 : 1;
    order = strcmp(left->id, right->id);
    return order < 0 ? -1 : (order > 0 ? 1 : 0);
}

/* Provide the thread index operation used by this module and its client applications. */
static size_t thread_index(const UmiDebugThreadInspector *inspector,
                           const char *thread_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || thread_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < inspector->thread_count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(inspector->threads[position].id, thread_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

/* Provide the frame index operation used by this module and its client applications. */
static size_t frame_index(const UmiDebugThreadInspector *inspector,
                          const char *frame_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || frame_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < inspector->frame_count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(inspector->frames[position].id, frame_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

/* Provide the scope index operation used by this module and its client applications. */
static size_t scope_index(const UmiDebugThreadInspector *inspector,
                          const char *scope_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || scope_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < inspector->scope_count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(inspector->scopes[position].id, scope_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

/* Provide the has thread operation used by this module and its client applications. */
static int has_thread(const UmiDebugThreadInspector *inspector,
                      const char *thread_id)
{
    return thread_index(inspector, thread_id) != SIZE_MAX;
}

/* Provide the has frame operation used by this module and its client applications. */
static int has_frame(const UmiDebugThreadInspector *inspector,
                     const char *frame_id)
{
    return frame_index(inspector, frame_id) != SIZE_MAX;
}

/* Provide the has scope operation used by this module and its client applications. */
static int has_scope(const UmiDebugThreadInspector *inspector,
                     const char *scope_id)
{
    return scope_index(inspector, scope_id) != SIZE_MAX;
}

/* Provide the thread matches operation used by this module and its client applications. */
static int thread_matches(const UmiDebugThreadInspector *inspector,
                          const UmiDebugThreadSnapshot *thread)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (inspector->filter.session_id[0] != '\0' &&
        strcmp(inspector->filter.session_id, thread->session_id) != 0) {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (inspector->filter.stopped_only && !thread->stopped) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (inspector->filter.current_only && !thread->current) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (inspector->filter.text[0] != '\0' &&
        !contains_text(thread->id, inspector->filter.text) &&
        !contains_text(thread->name, inspector->filter.text) &&
        !contains_text(thread->detail, inspector->filter.text)) {
        return 0;
    }
    return 1;
}

/*
 * Provide the frame belongs to included thread operation used by this module and its
 * client applications.
 */
static int frame_belongs_to_included_thread(
    const UmiDebugThreadInspector *inspector,
    const UmiDebugStackFrameSnapshot *frame)
{
    return has_thread(inspector, frame->thread_id);
}

/*
 * Provide the scope belongs to included frame operation used by this module and its client
 * applications.
 */
static int scope_belongs_to_included_frame(
    const UmiDebugThreadInspector *inspector,
    const UmiDebugScopeSnapshot *scope)
{
    return has_frame(inspector, scope->frame_id) &&
           (inspector->filter.include_expensive_scopes || !scope->expensive);
}

/*
 * Provide the variable belongs to included scope operation used by this module and its
 * client applications.
 */
static int variable_belongs_to_included_scope(
    const UmiDebugThreadInspector *inspector,
    const UmiDebugVariableSnapshot *variable)
{
    return has_scope(inspector, variable->scope_id);
}

/*
 * Provide the choose scope for selected frame operation used by this module and its client
 * applications.
 */
static void choose_scope_for_selected_frame(UmiDebugThreadInspector *inspector)
{
    size_t position;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (inspector->selected_scope_id[0] != '\0') {
        size_t selected = scope_index(inspector, inspector->selected_scope_id);
        /* Apply this branch only when its contract condition is satisfied. */
        if (selected != SIZE_MAX &&
            strcmp(inspector->scopes[selected].frame_id,
                   inspector->selected_frame_id) == 0) {
            return;
        }
    }
    inspector->selected_scope_id[0] = '\0';
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < inspector->scope_count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(inspector->scopes[position].frame_id,
                   inspector->selected_frame_id) == 0) {
            (void)copy_text(inspector->selected_scope_id,
                            sizeof(inspector->selected_scope_id),
                            inspector->scopes[position].id);
            break;
        }
    }
}

/*
 * Provide the choose frame for selected thread operation used by this module and its
 * client applications.
 */
static void choose_frame_for_selected_thread(UmiDebugThreadInspector *inspector)
{
    size_t position;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (inspector->selected_frame_id[0] != '\0') {
        size_t selected = frame_index(inspector, inspector->selected_frame_id);
        /* Apply this branch only when its contract condition is satisfied. */
        if (selected != SIZE_MAX &&
            strcmp(inspector->frames[selected].thread_id,
                   inspector->selected_thread_id) == 0) {
            choose_scope_for_selected_frame(inspector);
            return;
        }
    }
    inspector->selected_frame_id[0] = '\0';
    inspector->selected_scope_id[0] = '\0';
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < inspector->frame_count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(inspector->frames[position].thread_id,
                   inspector->selected_thread_id) == 0) {
            (void)copy_text(inspector->selected_frame_id,
                            sizeof(inspector->selected_frame_id),
                            inspector->frames[position].id);
            break;
        }
    }
    choose_scope_for_selected_frame(inspector);
}

/* Provide the choose thread operation used by this module and its client applications. */
static void choose_thread(UmiDebugThreadInspector *inspector)
{
    size_t position;

    /* Apply this branch only when its contract condition is satisfied. */
    if (inspector->follow_current_thread) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (position = 0U; position < inspector->thread_count; ++position) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (inspector->threads[position].current) {
                (void)copy_text(inspector->selected_thread_id,
                                sizeof(inspector->selected_thread_id),
                                inspector->threads[position].id);
                choose_frame_for_selected_thread(inspector);
                return;
            }
        }
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (inspector->selected_thread_id[0] != '\0' &&
        thread_index(inspector, inspector->selected_thread_id) != SIZE_MAX) {
        choose_frame_for_selected_thread(inspector);
        return;
    }
    inspector->selected_thread_id[0] = '\0';
    inspector->selected_frame_id[0] = '\0';
    inspector->selected_scope_id[0] = '\0';
    /* Apply this branch only when its contract condition is satisfied. */
    if (inspector->thread_count != 0U) {
        (void)copy_text(inspector->selected_thread_id,
                        sizeof(inspector->selected_thread_id),
                        inspector->threads[0].id);
        choose_frame_for_selected_thread(inspector);
    }
}

/*
 * Initialise debug thread inspector from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_thread_inspector_create(
    UmiDebugThreadInspector **out_inspector)
{
    UmiDebugThreadInspector *inspector;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_inspector == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_inspector = NULL;
    inspector = (UmiDebugThreadInspector *)calloc(1U, sizeof(*inspector));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    inspector->filter.struct_size = (uint32_t)sizeof(inspector->filter);
    inspector->filter.api_version = UMI_DEBUG_THREAD_INSPECTOR_API_VERSION;
    inspector->filter.include_expensive_scopes = 1;
    inspector->follow_current_thread = 1;
    inspector->revision = 1U;
    *out_inspector = inspector;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by debug thread inspector so the same storage can be reused
 * safely.
 */
void umi_debug_thread_inspector_destroy(UmiDebugThreadInspector *inspector)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL) return;
    free(inspector->threads);
    free(inspector->frames);
    free(inspector->scopes);
    free(inspector->variables);
    (void)memset(inspector, 0, sizeof(*inspector));
    free(inspector);
}

/*
 * Release or reset state held by debug thread inspector so the same storage can be reused
 * safely.
 */
UmiStatus umi_debug_thread_inspector_clear(
    UmiDebugThreadInspector *inspector)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    inspector->thread_count = 0U;
    inspector->frame_count = 0U;
    inspector->scope_count = 0U;
    inspector->variable_count = 0U;
    inspector->selected_thread_id[0] = '\0';
    inspector->selected_frame_id[0] = '\0';
    inspector->selected_scope_id[0] = '\0';
    inspector->thread_registry_revision = 0U;
    inspector->frame_registry_revision = 0U;
    inspector->scope_registry_revision = 0U;
    inspector->variable_registry_revision = 0U;
    inspector->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug thread inspector set filter operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_thread_inspector_set_filter(
    UmiDebugThreadInspector *inspector,
    const UmiDebugThreadInspectorFilter *filter)
{
    UmiDebugThreadInspectorFilter copy;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || filter == NULL ||
        filter->struct_size != (uint32_t)sizeof(*filter) ||
        filter->api_version != UMI_DEBUG_THREAD_INSPECTOR_API_VERSION ||
        memchr(filter->session_id, '\0', sizeof(filter->session_id)) == NULL ||
        memchr(filter->text, '\0', sizeof(filter->text)) == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    copy = *filter;
    copy.session_id[sizeof(copy.session_id) - 1U] = '\0';
    copy.text[sizeof(copy.text) - 1U] = '\0';
    copy.stopped_only = copy.stopped_only != 0;
    copy.current_only = copy.current_only != 0;
    copy.include_expensive_scopes = copy.include_expensive_scopes != 0;
    inspector->filter = copy;
    inspector->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug thread inspector set follow current thread operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_thread_inspector_set_follow_current_thread(
    UmiDebugThreadInspector *inspector,
    int follow_current_thread)
{
    int normalized;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    normalized = follow_current_thread != 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (inspector->follow_current_thread != normalized) {
        inspector->follow_current_thread = normalized;
        choose_thread(inspector);
        inspector->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the debug thread inspector refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_thread_inspector_refresh(
    UmiDebugThreadInspector *inspector,
    const UmiDebugThreadRegistry *threads,
    const UmiDebugStackFrameRegistry *frames,
    const UmiDebugScopeRegistry *scopes,
    const UmiDebugVariableRegistry *variables)
{
    size_t position;
    size_t source_count;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || threads == NULL || frames == NULL ||
        scopes == NULL || variables == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    inspector->thread_count = 0U;
    inspector->frame_count = 0U;
    inspector->scope_count = 0U;
    inspector->variable_count = 0U;

    source_count = umi_debug_thread_registry_count(threads);
    status = reserve_items((void **)&inspector->threads,
                           &inspector->thread_capacity, source_count,
                           sizeof(inspector->threads[0]));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < source_count; ++position) {
        UmiDebugThreadSnapshot item;
        status = umi_debug_thread_registry_at(threads, position, &item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (thread_matches(inspector, &item)) {
            inspector->threads[inspector->thread_count] = item;
            inspector->thread_count += 1U;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (inspector->thread_count > 1U) {
        qsort(inspector->threads, inspector->thread_count,
              sizeof(inspector->threads[0]), compare_threads);
    }

    source_count = umi_debug_stack_frame_registry_count(frames);
    status = reserve_items((void **)&inspector->frames,
                           &inspector->frame_capacity, source_count,
                           sizeof(inspector->frames[0]));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < source_count; ++position) {
        UmiDebugStackFrameSnapshot item;
        status = umi_debug_stack_frame_registry_at(frames, position, &item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (frame_belongs_to_included_thread(inspector, &item)) {
            inspector->frames[inspector->frame_count] = item;
            inspector->frame_count += 1U;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (inspector->frame_count > 1U) {
        qsort(inspector->frames, inspector->frame_count,
              sizeof(inspector->frames[0]), compare_frames);
    }

    source_count = umi_debug_scope_registry_count(scopes);
    status = reserve_items((void **)&inspector->scopes,
                           &inspector->scope_capacity, source_count,
                           sizeof(inspector->scopes[0]));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < source_count; ++position) {
        UmiDebugScopeSnapshot item;
        status = umi_debug_scope_registry_at(scopes, position, &item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (scope_belongs_to_included_frame(inspector, &item)) {
            inspector->scopes[inspector->scope_count] = item;
            inspector->scope_count += 1U;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (inspector->scope_count > 1U) {
        qsort(inspector->scopes, inspector->scope_count,
              sizeof(inspector->scopes[0]), compare_scopes);
    }

    source_count = umi_debug_variable_registry_count(variables);
    status = reserve_items((void **)&inspector->variables,
                           &inspector->variable_capacity, source_count,
                           sizeof(inspector->variables[0]));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < source_count; ++position) {
        UmiDebugVariableSnapshot item;
        status = umi_debug_variable_registry_at(variables, position, &item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (variable_belongs_to_included_scope(inspector, &item)) {
            inspector->variables[inspector->variable_count] = item;
            inspector->variable_count += 1U;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (inspector->variable_count > 1U) {
        qsort(inspector->variables, inspector->variable_count,
              sizeof(inspector->variables[0]), compare_variables);
    }

    inspector->thread_registry_revision =
        umi_debug_thread_registry_revision(threads);
    inspector->frame_registry_revision =
        umi_debug_stack_frame_registry_revision(frames);
    inspector->scope_registry_revision =
        umi_debug_scope_registry_revision(scopes);
    inspector->variable_registry_revision =
        umi_debug_variable_registry_revision(variables);
    choose_thread(inspector);
    inspector->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug thread inspector select thread operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_thread_inspector_select_thread(
    UmiDebugThreadInspector *inspector,
    const char *thread_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || thread_id == NULL || thread_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (thread_index(inspector, thread_id) == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = copy_text(inspector->selected_thread_id,
                       sizeof(inspector->selected_thread_id), thread_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    inspector->follow_current_thread = 0;
    choose_frame_for_selected_thread(inspector);
    inspector->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug thread inspector select frame operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_thread_inspector_select_frame(
    UmiDebugThreadInspector *inspector,
    const char *frame_id)
{
    size_t position;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || frame_id == NULL || frame_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = frame_index(inspector, frame_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == SIZE_MAX ||
        strcmp(inspector->frames[position].thread_id,
               inspector->selected_thread_id) != 0) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = copy_text(inspector->selected_frame_id,
                       sizeof(inspector->selected_frame_id), frame_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    inspector->selected_scope_id[0] = '\0';
    choose_scope_for_selected_frame(inspector);
    inspector->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug thread inspector select scope operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_thread_inspector_select_scope(
    UmiDebugThreadInspector *inspector,
    const char *scope_id)
{
    size_t position;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || scope_id == NULL || scope_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = scope_index(inspector, scope_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == SIZE_MAX ||
        strcmp(inspector->scopes[position].frame_id,
               inspector->selected_frame_id) != 0) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = copy_text(inspector->selected_scope_id,
                       sizeof(inspector->selected_scope_id), scope_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    inspector->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find debug thread inspector thread while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_debug_thread_inspector_thread_at(
    const UmiDebugThreadInspector *inspector,
    size_t index,
    UmiDebugThreadSnapshot *out_thread)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || out_thread == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= inspector->thread_count) return UMI_STATUS_NOT_FOUND;
    *out_thread = inspector->threads[index];
    return UMI_STATUS_OK;
}

/*
 * Find debug thread inspector frame while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_debug_thread_inspector_frame_at(
    const UmiDebugThreadInspector *inspector,
    size_t index,
    UmiDebugStackFrameSnapshot *out_frame)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || out_frame == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= inspector->frame_count) return UMI_STATUS_NOT_FOUND;
    *out_frame = inspector->frames[index];
    return UMI_STATUS_OK;
}

/*
 * Find debug thread inspector scope while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_debug_thread_inspector_scope_at(
    const UmiDebugThreadInspector *inspector,
    size_t index,
    UmiDebugScopeSnapshot *out_scope)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || out_scope == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= inspector->scope_count) return UMI_STATUS_NOT_FOUND;
    *out_scope = inspector->scopes[index];
    return UMI_STATUS_OK;
}

/*
 * Find debug thread inspector variable while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_debug_thread_inspector_variable_at(
    const UmiDebugThreadInspector *inspector,
    size_t index,
    UmiDebugVariableSnapshot *out_variable)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || out_variable == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= inspector->variable_count) return UMI_STATUS_NOT_FOUND;
    *out_variable = inspector->variables[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the debug thread inspector selected thread operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_thread_inspector_selected_thread(
    const UmiDebugThreadInspector *inspector,
    UmiDebugThreadSnapshot *out_thread)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || out_thread == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = thread_index(inspector, inspector->selected_thread_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_thread = inspector->threads[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the debug thread inspector selected frame operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_thread_inspector_selected_frame(
    const UmiDebugThreadInspector *inspector,
    UmiDebugStackFrameSnapshot *out_frame)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || out_frame == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = frame_index(inspector, inspector->selected_frame_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_frame = inspector->frames[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the debug thread inspector selected scope operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_thread_inspector_selected_scope(
    const UmiDebugThreadInspector *inspector,
    UmiDebugScopeSnapshot *out_scope)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || out_scope == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = scope_index(inspector, inspector->selected_scope_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_scope = inspector->scopes[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the debug thread inspector snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_thread_inspector_snapshot(
    const UmiDebugThreadInspector *inspector,
    UmiDebugThreadInspectorSnapshot *out_snapshot)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inspector == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEBUG_THREAD_INSPECTOR_API_VERSION;
    out_snapshot->thread_count = inspector->thread_count;
    out_snapshot->frame_count = inspector->frame_count;
    out_snapshot->scope_count = inspector->scope_count;
    out_snapshot->variable_count = inspector->variable_count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < inspector->thread_count; ++position) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (inspector->threads[position].stopped) {
            out_snapshot->stopped_thread_count += 1U;
        }
    }
    (void)copy_text(out_snapshot->selected_thread_id,
                    sizeof(out_snapshot->selected_thread_id),
                    inspector->selected_thread_id);
    (void)copy_text(out_snapshot->selected_frame_id,
                    sizeof(out_snapshot->selected_frame_id),
                    inspector->selected_frame_id);
    (void)copy_text(out_snapshot->selected_scope_id,
                    sizeof(out_snapshot->selected_scope_id),
                    inspector->selected_scope_id);
    out_snapshot->thread_registry_revision =
        inspector->thread_registry_revision;
    out_snapshot->frame_registry_revision = inspector->frame_registry_revision;
    out_snapshot->scope_registry_revision = inspector->scope_registry_revision;
    out_snapshot->variable_registry_revision =
        inspector->variable_registry_revision;
    out_snapshot->revision = inspector->revision;
    out_snapshot->follow_current_thread = inspector->follow_current_thread;
    out_snapshot->has_thread_selection =
        inspector->selected_thread_id[0] != '\0';
    out_snapshot->has_frame_selection =
        inspector->selected_frame_id[0] != '\0';
    out_snapshot->has_scope_selection =
        inspector->selected_scope_id[0] != '\0';
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by debug thread inspector thread without
 * changing their state.
 */
size_t umi_debug_thread_inspector_thread_count(
    const UmiDebugThreadInspector *inspector)
{
    return inspector != NULL ? inspector->thread_count : 0U;
}

/*
 * Return the number of records represented by debug thread inspector frame without
 * changing their state.
 */
size_t umi_debug_thread_inspector_frame_count(
    const UmiDebugThreadInspector *inspector)
{
    return inspector != NULL ? inspector->frame_count : 0U;
}

/*
 * Return the number of records represented by debug thread inspector scope without
 * changing their state.
 */
size_t umi_debug_thread_inspector_scope_count(
    const UmiDebugThreadInspector *inspector)
{
    return inspector != NULL ? inspector->scope_count : 0U;
}

/*
 * Return the number of records represented by debug thread inspector variable without
 * changing their state.
 */
size_t umi_debug_thread_inspector_variable_count(
    const UmiDebugThreadInspector *inspector)
{
    return inspector != NULL ? inspector->variable_count : 0U;
}

/*
 * Provide the debug thread inspector revision operation used by this module and its client
 * applications.
 */
uint64_t umi_debug_thread_inspector_revision(
    const UmiDebugThreadInspector *inspector)
{
    return inspector != NULL ? inspector->revision : 0U;
}
