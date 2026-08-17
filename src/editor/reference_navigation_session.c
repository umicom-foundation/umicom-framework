/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/reference_navigation_session.c
 *
 * PURPOSE:
 *   Implement grouped and filterable reference navigation above the canonical
 *   provider query session.  The model is deliberately toolkit-neutral so a
 *   tree view, list view, web table or headless client observes identical
 *   grouping, selection and preview behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/reference_navigation_session.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorReferenceNavigationSession {
    UmiEditorNavigationQuerySession *query;
    UmiEditorNavigationSourcePreviewCache *previews;
    UmiEditorNavigationRequest request;
    UmiEditorReferenceNavigationGroup *groups;
    size_t group_count;
    size_t group_capacity;
    UmiEditorReferenceNavigationEntry *entries;
    size_t entry_count;
    size_t entry_capacity;
    UmiEditorNavigationSourcePreview active_preview;
    size_t selected_index;
    size_t visible_count;
    uint64_t revision;
    UmiEditorReferenceNavigationState state;
    int has_request;
    int has_preview;
    int include_declaration;
    int truncated;
    char filter[UMI_EDITOR_REFERENCE_NAVIGATION_FILTER_CAPACITY];
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static int contains_case_insensitive(const char *text, const char *pattern)
{
    size_t text_length;
    size_t pattern_length;
    size_t start;

    if (pattern == NULL || pattern[0] == '\0') return 1;
    if (text == NULL) return 0;
    text_length = strlen(text);
    pattern_length = strlen(pattern);
    if (pattern_length > text_length) return 0;
    for (start = 0U; start + pattern_length <= text_length; ++start) {
        size_t offset;
        int match = 1;
        for (offset = 0U; offset < pattern_length; ++offset) {
            unsigned char left = (unsigned char)text[start + offset];
            unsigned char right = (unsigned char)pattern[offset];
            if (tolower(left) != tolower(right)) {
                match = 0;
                break;
            }
        }
        if (match) return 1;
    }
    return 0;
}

static UmiStatus reserve_groups(UmiEditorReferenceNavigationSession *session,
                                size_t required)
{
    size_t capacity;
    UmiEditorReferenceNavigationGroup *replacement;
    if (required <= session->group_capacity) return UMI_STATUS_OK;
    capacity = session->group_capacity > 0U ? session->group_capacity : 16U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorReferenceNavigationGroup *)realloc(
        session->groups, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->groups = replacement;
    session->group_capacity = capacity;
    return UMI_STATUS_OK;
}

static UmiStatus reserve_entries(UmiEditorReferenceNavigationSession *session,
                                 size_t required)
{
    size_t capacity;
    UmiEditorReferenceNavigationEntry *replacement;
    if (required <= session->entry_capacity) return UMI_STATUS_OK;
    capacity = session->entry_capacity > 0U ? session->entry_capacity : 64U;
    while (capacity < required) {
        if (capacity > UMI_EDITOR_NAVIGATION_MAXIMUM_RESULTS / 2U) {
            capacity = UMI_EDITOR_NAVIGATION_MAXIMUM_RESULTS;
            break;
        }
        capacity *= 2U;
    }
    replacement = (UmiEditorReferenceNavigationEntry *)realloc(
        session->entries, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->entries = replacement;
    session->entry_capacity = capacity;
    return UMI_STATUS_OK;
}

static int compare_entries(const void *left_pointer, const void *right_pointer)
{
    const UmiEditorReferenceNavigationEntry *left =
        (const UmiEditorReferenceNavigationEntry *)left_pointer;
    const UmiEditorReferenceNavigationEntry *right =
        (const UmiEditorReferenceNavigationEntry *)right_pointer;
    int order = strcmp(left->result.location.uri, right->result.location.uri);
    if (order != 0) return order;
    if (left->result.location.line < right->result.location.line) return -1;
    if (left->result.location.line > right->result.location.line) return 1;
    if (left->result.location.column < right->result.location.column) return -1;
    if (left->result.location.column > right->result.location.column) return 1;
    return strcmp(left->result.provider_id, right->result.provider_id);
}

static size_t find_group(const UmiEditorReferenceNavigationSession *session,
                         const char *uri)
{
    size_t index;
    for (index = 0U; index < session->group_count; ++index) {
        if (strcmp(session->groups[index].uri, uri) == 0) return index;
    }
    return SIZE_MAX;
}

static const char *uri_label(const char *uri)
{
    const char *separator;
    const char *backslash;
    if (uri == NULL) return "";
    separator = strrchr(uri, '/');
    backslash = strrchr(uri, '\\');
    if (backslash != NULL && (separator == NULL || backslash > separator)) {
        separator = backslash;
    }
    return separator != NULL && separator[1] != '\0' ? separator + 1 : uri;
}

static int entry_matches_filter(
    const UmiEditorReferenceNavigationSession *session,
    const UmiEditorReferenceNavigationEntry *entry)
{
    return contains_case_insensitive(entry->result.location.uri,
                                     session->filter) ||
           contains_case_insensitive(entry->result.location.label,
                                     session->filter) ||
           contains_case_insensitive(entry->result.location.preview,
                                     session->filter) ||
           contains_case_insensitive(entry->result.detail, session->filter);
}

static void recompute_visibility(UmiEditorReferenceNavigationSession *session)
{
    size_t index;
    size_t first_visible = SIZE_MAX;

    session->visible_count = 0U;
    for (index = 0U; index < session->group_count; ++index) {
        session->groups[index].visible_result_count = 0U;
    }
    for (index = 0U; index < session->entry_count; ++index) {
        UmiEditorReferenceNavigationEntry *entry = &session->entries[index];
        const UmiEditorReferenceNavigationGroup *group =
            &session->groups[entry->group_index];
        entry->visible = group->expanded &&
                         (session->include_declaration || !entry->declaration) &&
                         entry_matches_filter(session, entry);
        entry->selected = 0;
        if (entry->visible) {
            if (first_visible == SIZE_MAX) first_visible = index;
            ++session->visible_count;
            ++session->groups[entry->group_index].visible_result_count;
        }
    }
    if (session->selected_index >= session->entry_count ||
        (session->selected_index != SIZE_MAX &&
         !session->entries[session->selected_index].visible)) {
        session->selected_index = first_visible;
    }
    if (session->selected_index != SIZE_MAX) {
        session->entries[session->selected_index].selected = 1;
    }
}

static UmiStatus refresh_selected_preview(
    UmiEditorReferenceNavigationSession *session)
{
    UmiEditorNavigationPreviewRequest request;
    UmiEditorReferenceNavigationEntry *entry;
    uint64_t end_line;
    UmiStatus status;

    session->has_preview = 0;
    (void)memset(&session->active_preview, 0, sizeof(session->active_preview));
    if (session->selected_index == SIZE_MAX ||
        session->selected_index >= session->entry_count) {
        return UMI_STATUS_NOT_FOUND;
    }
    entry = &session->entries[session->selected_index];
    end_line = entry->result.location.line > UINT64_MAX - 2U
        ? UINT64_MAX
        : entry->result.location.line + 2U;
    status = umi_editor_navigation_preview_request_initialize(
        &request,
        session->request.request_id,
        entry->result.location.uri,
        session->request.language_id,
        entry->result.location.document_revision,
        entry->result.location.line > 2U ? entry->result.location.line - 2U : 0U,
        end_line);
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_navigation_source_preview_cache_resolve(
        session->previews, &request, &session->active_preview);
    if (status == UMI_STATUS_OK) {
        session->has_preview = 1;
        return UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_NOT_FOUND || status == UMI_STATUS_UNAVAILABLE) {
        return UMI_STATUS_OK;
    }
    return status;
}

static UmiStatus rebuild_entries(UmiEditorReferenceNavigationSession *session)
{
    UmiEditorNavigationQuerySessionSnapshot query_snapshot;
    size_t index;
    UmiStatus status;

    session->entry_count = 0U;
    session->group_count = 0U;
    session->selected_index = SIZE_MAX;
    status = umi_editor_navigation_query_session_snapshot(session->query,
                                                          &query_snapshot);
    if (status != UMI_STATUS_OK) return status;
    session->truncated = query_snapshot.truncated;
    status = reserve_entries(session, query_snapshot.result_count);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < query_snapshot.result_count; ++index) {
        UmiEditorReferenceNavigationEntry *entry =
            &session->entries[session->entry_count];
        (void)memset(entry, 0, sizeof(*entry));
        entry->struct_size = (uint32_t)sizeof(*entry);
        entry->api_version = UMI_EDITOR_REFERENCE_NAVIGATION_SESSION_API_VERSION;
        status = umi_editor_navigation_query_session_at(session->query, index,
                                                        &entry->result);
        if (status != UMI_STATUS_OK) return status;
        entry->declaration =
            entry->result.location.kind ==
            UMI_EDITOR_SOURCE_LOCATION_DECLARATION;
        ++session->entry_count;
    }
    if (session->entry_count > 1U) {
        qsort(session->entries, session->entry_count,
              sizeof(*session->entries), compare_entries);
    }
    for (index = 0U; index < session->entry_count; ++index) {
        UmiEditorReferenceNavigationEntry *entry = &session->entries[index];
        size_t group_index = find_group(session, entry->result.location.uri);
        if (group_index == SIZE_MAX) {
            UmiEditorReferenceNavigationGroup *group;
            status = reserve_groups(session, session->group_count + 1U);
            if (status != UMI_STATUS_OK) return status;
            group_index = session->group_count++;
            group = &session->groups[group_index];
            (void)memset(group, 0, sizeof(*group));
            group->struct_size = (uint32_t)sizeof(*group);
            group->api_version =
                UMI_EDITOR_REFERENCE_NAVIGATION_SESSION_API_VERSION;
            group->expanded = 1;
            status = copy_text(group->uri, sizeof(group->uri),
                               entry->result.location.uri);
            if (status == UMI_STATUS_OK) {
                status = copy_text(group->label, sizeof(group->label),
                                   uri_label(entry->result.location.uri));
            }
            if (status != UMI_STATUS_OK) return status;
        }
        entry->group_index = group_index;
        ++session->groups[group_index].result_count;
    }
    recompute_visibility(session);
    status = refresh_selected_preview(session);
    return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_OK : status;
}

UmiStatus umi_editor_reference_navigation_session_create(
    UmiEditorNavigationProviderRegistry *registry,
    UmiEditorReferenceNavigationSession **out_session)
{
    UmiEditorReferenceNavigationSession *session;
    UmiStatus status;

    if (registry == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_session = NULL;
    session = (UmiEditorReferenceNavigationSession *)calloc(1U,
                                                             sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_navigation_query_session_create(registry,
                                                        &session->query);
    if (status == UMI_STATUS_OK) {
        status = umi_editor_navigation_source_preview_cache_create(
            registry, NULL, &session->previews);
    }
    if (status != UMI_STATUS_OK) {
        umi_editor_navigation_query_session_destroy(session->query);
        free(session);
        return status;
    }
    session->selected_index = SIZE_MAX;
    session->include_declaration = 1;
    session->state = UMI_EDITOR_REFERENCE_NAVIGATION_CLOSED;
    session->revision = 1U;
    *out_session = session;
    return UMI_STATUS_OK;
}

void umi_editor_reference_navigation_session_destroy(
    UmiEditorReferenceNavigationSession *session)
{
    if (session == NULL) return;
    umi_editor_navigation_query_session_destroy(session->query);
    umi_editor_navigation_source_preview_cache_destroy(session->previews);
    free(session->groups);
    free(session->entries);
    session->groups = NULL;
    session->entries = NULL;
    free(session);
}

UmiStatus umi_editor_reference_navigation_session_open(
    UmiEditorReferenceNavigationSession *session,
    const UmiEditorNavigationRequest *request)
{
    UmiEditorNavigationRequest effective;
    UmiEditorNavigationQueryOptions options;
    UmiStatus status;

    if (session == NULL || request == NULL ||
        request->query_kind != UMI_EDITOR_NAVIGATION_QUERY_REFERENCE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = *request;
    if (session->include_declaration) {
        effective.flags |= UMI_EDITOR_NAVIGATION_REQUEST_INCLUDE_DECLARATIONS;
    } else {
        effective.flags &= ~(UmiEditorNavigationRequestFlags)UMI_EDITOR_NAVIGATION_REQUEST_INCLUDE_DECLARATIONS;
    }
    session->state = UMI_EDITOR_REFERENCE_NAVIGATION_LOADING;
    session->request = effective;
    session->has_request = 1;
    (void)umi_editor_navigation_query_options_initialize(&options);
    options.maximum_results = effective.maximum_results > 0U
        ? effective.maximum_results
        : UMI_EDITOR_NAVIGATION_DEFAULT_MAXIMUM_RESULTS;
    status = umi_editor_navigation_query_session_execute(session->query,
                                                         &effective,
                                                         &options);
    if (status == UMI_STATUS_CANCELLED) {
        session->state = UMI_EDITOR_REFERENCE_NAVIGATION_CANCELLED;
        session->revision = next_revision(session->revision);
        return status;
    }
    if (status != UMI_STATUS_OK) {
        session->state = UMI_EDITOR_REFERENCE_NAVIGATION_FAILED;
        session->revision = next_revision(session->revision);
        return status;
    }
    status = rebuild_entries(session);
    if (status != UMI_STATUS_OK) {
        session->state = UMI_EDITOR_REFERENCE_NAVIGATION_FAILED;
        session->revision = next_revision(session->revision);
        return status;
    }
    session->state = UMI_EDITOR_REFERENCE_NAVIGATION_OPEN;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_navigation_session_refresh(
    UmiEditorReferenceNavigationSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!session->has_request) return UMI_STATUS_INVALID_STATE;
    return umi_editor_reference_navigation_session_open(session,
                                                        &session->request);
}

UmiStatus umi_editor_reference_navigation_session_cancel(
    UmiEditorReferenceNavigationSession *session)
{
    UmiStatus status;
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_navigation_query_session_cancel(session->query);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_INVALID_STATE) {
        return status;
    }
    session->state = UMI_EDITOR_REFERENCE_NAVIGATION_CANCELLED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_navigation_session_close(
    UmiEditorReferenceNavigationSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_editor_navigation_query_session_clear(session->query);
    session->entry_count = 0U;
    session->group_count = 0U;
    session->visible_count = 0U;
    session->selected_index = SIZE_MAX;
    session->has_request = 0;
    session->has_preview = 0;
    session->state = UMI_EDITOR_REFERENCE_NAVIGATION_CLOSED;
    session->filter[0] = '\0';
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_navigation_session_set_filter(
    UmiEditorReferenceNavigationSession *session,
    const char *filter)
{
    UmiStatus status;
    if (session == NULL || filter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = copy_text(session->filter, sizeof(session->filter), filter);
    if (status != UMI_STATUS_OK) return status;
    recompute_visibility(session);
    (void)refresh_selected_preview(session);
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_navigation_session_set_include_declaration(
    UmiEditorReferenceNavigationSession *session,
    int include_declaration)
{
    int effective;

    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    effective = include_declaration != 0;
    if (session->include_declaration == effective) return UMI_STATUS_OK;
    session->include_declaration = effective;
    if (session->has_request) {
        return umi_editor_reference_navigation_session_open(session,
                                                            &session->request);
    }
    recompute_visibility(session);
    (void)refresh_selected_preview(session);
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_navigation_session_set_group_expanded(
    UmiEditorReferenceNavigationSession *session,
    size_t group_index,
    int expanded)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (group_index >= session->group_count) return UMI_STATUS_NOT_FOUND;
    session->groups[group_index].expanded = expanded != 0;
    recompute_visibility(session);
    (void)refresh_selected_preview(session);
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_navigation_session_select(
    UmiEditorReferenceNavigationSession *session,
    size_t result_index)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (result_index >= session->entry_count ||
        !session->entries[result_index].visible) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (session->selected_index != SIZE_MAX &&
        session->selected_index < session->entry_count) {
        session->entries[session->selected_index].selected = 0;
    }
    session->selected_index = result_index;
    session->entries[result_index].selected = 1;
    (void)refresh_selected_preview(session);
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

static UmiStatus select_relative(UmiEditorReferenceNavigationSession *session,
                                 int direction,
                                 int wrap)
{
    size_t index;
    if (session->visible_count == 0U) return UMI_STATUS_NOT_FOUND;
    if (session->selected_index == SIZE_MAX) {
        for (index = 0U; index < session->entry_count; ++index) {
            if (session->entries[index].visible) {
                return umi_editor_reference_navigation_session_select(session,
                                                                      index);
            }
        }
        return UMI_STATUS_NOT_FOUND;
    }
    if (direction > 0) {
        for (index = session->selected_index + 1U;
             index < session->entry_count; ++index) {
            if (session->entries[index].visible) {
                return umi_editor_reference_navigation_session_select(session,
                                                                      index);
            }
        }
        if (wrap) {
            for (index = 0U; index < session->selected_index; ++index) {
                if (session->entries[index].visible) {
                    return umi_editor_reference_navigation_session_select(
                        session, index);
                }
            }
        }
    } else {
        index = session->selected_index;
        while (index > 0U) {
            --index;
            if (session->entries[index].visible) {
                return umi_editor_reference_navigation_session_select(session,
                                                                      index);
            }
        }
        if (wrap) {
            index = session->entry_count;
            while (index > session->selected_index + 1U) {
                --index;
                if (session->entries[index].visible) {
                    return umi_editor_reference_navigation_session_select(
                        session, index);
                }
            }
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_editor_reference_navigation_session_select_next(
    UmiEditorReferenceNavigationSession *session,
    int wrap)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return select_relative(session, 1, wrap != 0);
}

UmiStatus umi_editor_reference_navigation_session_select_previous(
    UmiEditorReferenceNavigationSession *session,
    int wrap)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return select_relative(session, -1, wrap != 0);
}

UmiStatus umi_editor_reference_navigation_session_group_at(
    const UmiEditorReferenceNavigationSession *session,
    size_t group_index,
    UmiEditorReferenceNavigationGroup *out_group)
{
    if (session == NULL || out_group == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (group_index >= session->group_count) return UMI_STATUS_NOT_FOUND;
    *out_group = session->groups[group_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_navigation_session_entry_at(
    const UmiEditorReferenceNavigationSession *session,
    size_t result_index,
    UmiEditorReferenceNavigationEntry *out_entry)
{
    if (session == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (result_index >= session->entry_count) return UMI_STATUS_NOT_FOUND;
    *out_entry = session->entries[result_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_navigation_session_visible_entry_at(
    const UmiEditorReferenceNavigationSession *session,
    size_t visible_index,
    UmiEditorReferenceNavigationEntry *out_entry)
{
    size_t index;
    size_t current = 0U;
    if (session == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < session->entry_count; ++index) {
        if (session->entries[index].visible) {
            if (current == visible_index) {
                *out_entry = session->entries[index];
                return UMI_STATUS_OK;
            }
            ++current;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_editor_reference_navigation_session_selected(
    const UmiEditorReferenceNavigationSession *session,
    UmiEditorReferenceNavigationEntry *out_entry)
{
    if (session == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (session->selected_index == SIZE_MAX ||
        session->selected_index >= session->entry_count) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_entry = session->entries[session->selected_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_navigation_session_selected_preview(
    const UmiEditorReferenceNavigationSession *session,
    UmiEditorNavigationSourcePreview *out_preview)
{
    if (session == NULL || out_preview == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!session->has_preview) return UMI_STATUS_NOT_FOUND;
    *out_preview = session->active_preview;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_navigation_session_snapshot(
    const UmiEditorReferenceNavigationSession *session,
    UmiEditorReferenceNavigationSnapshot *out_snapshot)
{
    if (session == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_EDITOR_REFERENCE_NAVIGATION_SESSION_API_VERSION;
    out_snapshot->state = session->state;
    out_snapshot->group_count = session->group_count;
    out_snapshot->result_count = session->entry_count;
    out_snapshot->visible_result_count = session->visible_count;
    out_snapshot->selected_index = session->selected_index;
    out_snapshot->request_id = session->has_request
        ? session->request.request_id
        : 0U;
    out_snapshot->revision = session->revision;
    out_snapshot->include_declaration = session->include_declaration;
    out_snapshot->has_selection = session->selected_index != SIZE_MAX;
    out_snapshot->has_preview = session->has_preview;
    out_snapshot->truncated = session->truncated;
    (void)copy_text(out_snapshot->filter, sizeof(out_snapshot->filter),
                    session->filter);
    return UMI_STATUS_OK;
}

UmiEditorNavigationQuerySession *umi_editor_reference_navigation_session_query(
    UmiEditorReferenceNavigationSession *session)
{
    return session != NULL ? session->query : NULL;
}

size_t umi_editor_reference_navigation_session_group_count(
    const UmiEditorReferenceNavigationSession *session)
{
    return session != NULL ? session->group_count : 0U;
}

size_t umi_editor_reference_navigation_session_count(
    const UmiEditorReferenceNavigationSession *session)
{
    return session != NULL ? session->entry_count : 0U;
}

size_t umi_editor_reference_navigation_session_visible_count(
    const UmiEditorReferenceNavigationSession *session)
{
    return session != NULL ? session->visible_count : 0U;
}

uint64_t umi_editor_reference_navigation_session_revision(
    const UmiEditorReferenceNavigationSession *session)
{
    return session != NULL ? session->revision : 0U;
}
