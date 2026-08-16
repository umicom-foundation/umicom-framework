/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/snippet_session.c
 *
 * PURPOSE:
 *   Parse bounded snippet syntax into expanded text and deterministic cursor
 *   traversal stops without depending on a graphical editor toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/snippet_session.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorSnippetSession {
    UmiEditorSnippetTemplate snippet;
    UmiEditorSnippetPlaceholder *placeholders;
    size_t placeholder_count;
    size_t placeholder_capacity;
    char expanded[UMI_EDITOR_SNIPPET_EXPANDED_CAPACITY];
    size_t expanded_length;
    size_t active_index;
    uint64_t insertion_byte_offset;
    uint64_t revision;
    UmiEditorSnippetSessionState state;
    int has_active;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static UmiStatus validate_template(const UmiEditorSnippetTemplate *snippet)
{
    if (snippet == NULL ||
        snippet->struct_size != (uint32_t)sizeof(*snippet) ||
        snippet->api_version != UMI_EDITOR_SNIPPET_SESSION_API_VERSION ||
        !terminated(snippet->id, sizeof(snippet->id)) ||
        snippet->id[0] == '\0' ||
        !terminated(snippet->language_id, sizeof(snippet->language_id)) ||
        snippet->language_id[0] == '\0' ||
        !terminated(snippet->name, sizeof(snippet->name)) ||
        snippet->name[0] == '\0' ||
        !terminated(snippet->body, sizeof(snippet->body))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus reserve_placeholders(UmiEditorSnippetSession *session,
                                      size_t required)
{
    size_t capacity;
    UmiEditorSnippetPlaceholder *replacement;

    if (required <= session->placeholder_capacity) return UMI_STATUS_OK;
    capacity = session->placeholder_capacity > 0U
        ? session->placeholder_capacity : 16U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorSnippetPlaceholder *)realloc(
        session->placeholders, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->placeholders = replacement;
    session->placeholder_capacity = capacity;
    return UMI_STATUS_OK;
}

static UmiStatus append_text(UmiEditorSnippetSession *session,
                             const char *text,
                             size_t length)
{
    if (length > UMI_EDITOR_SNIPPET_EXPANDED_CAPACITY - 1U ||
        session->expanded_length >
            UMI_EDITOR_SNIPPET_EXPANDED_CAPACITY - 1U - length) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (length > 0U) {
        (void)memcpy(&session->expanded[session->expanded_length], text,
                     length);
        session->expanded_length += length;
    }
    session->expanded[session->expanded_length] = '\0';
    return UMI_STATUS_OK;
}

static int ordinal_seen(const UmiEditorSnippetSession *session,
                        uint32_t ordinal)
{
    size_t index;

    for (index = 0U; index < session->placeholder_count; ++index) {
        if (session->placeholders[index].ordinal == ordinal) return 1;
    }
    return 0;
}

static const UmiEditorSnippetPlaceholder *primary_placeholder(
    const UmiEditorSnippetSession *session,
    uint32_t ordinal)
{
    size_t index;

    for (index = 0U; index < session->placeholder_count; ++index) {
        if (session->placeholders[index].ordinal == ordinal &&
            session->placeholders[index].primary) {
            return &session->placeholders[index];
        }
    }
    return NULL;
}

static UmiStatus add_placeholder(UmiEditorSnippetSession *session,
                                 uint32_t ordinal,
                                 size_t start,
                                 size_t end,
                                 const char *default_text,
                                 size_t default_length,
                                 const char *choices,
                                 size_t choices_length)
{
    UmiEditorSnippetPlaceholder placeholder;
    UmiStatus status;

    if (default_length >= UMI_EDITOR_SNIPPET_DEFAULT_CAPACITY ||
        choices_length >= UMI_EDITOR_SNIPPET_CHOICES_CAPACITY ||
        session->insertion_byte_offset > UINT64_MAX - (uint64_t)end) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = reserve_placeholders(session, session->placeholder_count + 1U);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&placeholder, 0, sizeof(placeholder));
    placeholder.struct_size = (uint32_t)sizeof(placeholder);
    placeholder.api_version = UMI_EDITOR_SNIPPET_SESSION_API_VERSION;
    placeholder.ordinal = ordinal;
    placeholder.start_byte_offset = session->insertion_byte_offset +
                                    (uint64_t)start;
    placeholder.end_byte_offset = session->insertion_byte_offset +
                                  (uint64_t)end;
    if (default_length > 0U) {
        (void)memcpy(placeholder.default_text, default_text, default_length);
    }
    placeholder.default_text[default_length] = '\0';
    if (choices_length > 0U) {
        (void)memcpy(placeholder.choices, choices, choices_length);
    }
    placeholder.choices[choices_length] = '\0';
    placeholder.primary = !ordinal_seen(session, ordinal);
    placeholder.final_stop = ordinal == 0U;
    session->placeholders[session->placeholder_count++] = placeholder;
    return UMI_STATUS_OK;
}

static int parse_ordinal(const char *text,
                         size_t length,
                         size_t *in_out_position,
                         uint32_t *out_ordinal)
{
    size_t position = *in_out_position;
    uint64_t value = 0U;
    int found = 0;

    while (position < length && text[position] >= '0' &&
           text[position] <= '9') {
        uint32_t digit = (uint32_t)(text[position] - '0');
        if (value > ((uint64_t)UINT32_MAX - digit) / 10U) return 0;
        value = value * 10U + digit;
        ++position;
        found = 1;
    }
    if (!found) return 0;
    *in_out_position = position;
    *out_ordinal = (uint32_t)value;
    return 1;
}

static UmiStatus parse_braced_placeholder(UmiEditorSnippetSession *session,
                                          const char *body,
                                          size_t length,
                                          size_t *in_out_position)
{
    size_t position = *in_out_position + 2U;
    size_t content_start;
    size_t content_end;
    size_t choice_end;
    size_t expanded_start = session->expanded_length;
    uint32_t ordinal;
    UmiStatus status;

    if (!parse_ordinal(body, length, &position, &ordinal)) {
        return UMI_STATUS_PARSE_ERROR;
    }
    if (position < length && body[position] == '}') {
        const UmiEditorSnippetPlaceholder *primary =
            primary_placeholder(session, ordinal);
        char default_text[UMI_EDITOR_SNIPPET_DEFAULT_CAPACITY] = "";
        size_t default_length = primary != NULL
            ? strlen(primary->default_text) : 0U;

        if (default_length > 0U) {
            (void)memcpy(default_text, primary->default_text,
                         default_length + 1U);
        }

        status = append_text(session, default_text, default_length);
        if (status == UMI_STATUS_OK) {
            status = add_placeholder(session, ordinal, expanded_start,
                                     session->expanded_length, default_text,
                                     default_length, "", 0U);
        }
        if (status == UMI_STATUS_OK) *in_out_position = position + 1U;
        return status;
    }
    if (position < length && body[position] == ':') {
        content_start = ++position;
        while (position < length && body[position] != '}') ++position;
        if (position >= length) return UMI_STATUS_PARSE_ERROR;
        content_end = position;
        status = append_text(session, &body[content_start],
                             content_end - content_start);
        if (status == UMI_STATUS_OK) {
            status = add_placeholder(session, ordinal, expanded_start,
                                     session->expanded_length,
                                     &body[content_start],
                                     content_end - content_start, "", 0U);
        }
        if (status == UMI_STATUS_OK) *in_out_position = position + 1U;
        return status;
    }
    if (position < length && body[position] == '|') {
        content_start = ++position;
        while (position + 1U < length &&
               !(body[position] == '|' && body[position + 1U] == '}')) {
            ++position;
        }
        if (position + 1U >= length) return UMI_STATUS_PARSE_ERROR;
        content_end = position;
        choice_end = content_start;
        while (choice_end < content_end && body[choice_end] != ',') {
            ++choice_end;
        }
        status = append_text(session, &body[content_start],
                             choice_end - content_start);
        if (status == UMI_STATUS_OK) {
            status = add_placeholder(session, ordinal, expanded_start,
                                     session->expanded_length,
                                     &body[content_start],
                                     choice_end - content_start,
                                     &body[content_start],
                                     content_end - content_start);
        }
        if (status == UMI_STATUS_OK) *in_out_position = position + 2U;
        return status;
    }
    return UMI_STATUS_PARSE_ERROR;
}

static UmiStatus parse_body(UmiEditorSnippetSession *session)
{
    const char *body = session->snippet.body;
    size_t length = strlen(body);
    size_t position = 0U;

    while (position < length) {
        UmiStatus status;

        if (body[position] == '$' && position + 1U < length &&
            body[position + 1U] == '{') {
            size_t parsed_position = position;
            status = parse_braced_placeholder(session, body, length,
                                               &parsed_position);
            if (status == UMI_STATUS_OK) {
                position = parsed_position;
                continue;
            }
            if (status != UMI_STATUS_PARSE_ERROR) return status;
        } else if (body[position] == '$' && position + 1U < length &&
                   body[position + 1U] >= '0' &&
                   body[position + 1U] <= '9') {
            size_t parsed_position = position + 1U;
            uint32_t ordinal;

            if (parse_ordinal(body, length, &parsed_position, &ordinal)) {
                const UmiEditorSnippetPlaceholder *primary =
                    primary_placeholder(session, ordinal);
                char default_text[UMI_EDITOR_SNIPPET_DEFAULT_CAPACITY] = "";
                size_t default_length = primary != NULL
                    ? strlen(primary->default_text) : 0U;
                size_t expanded_start = session->expanded_length;

                if (default_length > 0U) {
                    (void)memcpy(default_text, primary->default_text,
                                 default_length + 1U);
                }
                status = append_text(session, default_text, default_length);
                if (status == UMI_STATUS_OK) {
                    status = add_placeholder(session, ordinal, expanded_start,
                                             session->expanded_length,
                                             default_text, default_length,
                                             "", 0U);
                }
                if (status != UMI_STATUS_OK) return status;
                position = parsed_position;
                continue;
            }
        }
        status = append_text(session, &body[position], 1U);
        if (status != UMI_STATUS_OK) return status;
        ++position;
    }
    return UMI_STATUS_OK;
}

static size_t primary_for_ordinal(const UmiEditorSnippetSession *session,
                                  uint32_t ordinal)
{
    size_t index;

    for (index = 0U; index < session->placeholder_count; ++index) {
        if (session->placeholders[index].ordinal == ordinal &&
            session->placeholders[index].primary) {
            return index;
        }
    }
    return SIZE_MAX;
}

static size_t first_traversal_index(const UmiEditorSnippetSession *session)
{
    size_t index;
    size_t selected = SIZE_MAX;
    uint32_t ordinal = UINT32_MAX;

    for (index = 0U; index < session->placeholder_count; ++index) {
        const UmiEditorSnippetPlaceholder *placeholder =
            &session->placeholders[index];
        if (!placeholder->primary || placeholder->ordinal == 0U) continue;
        if (placeholder->ordinal < ordinal) {
            ordinal = placeholder->ordinal;
            selected = index;
        }
    }
    if (selected == SIZE_MAX) selected = primary_for_ordinal(session, 0U);
    return selected;
}

UmiStatus umi_editor_snippet_session_create(
    UmiEditorSnippetSession **out_session)
{
    UmiEditorSnippetSession *session;

    if (out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_session = NULL;
    session = (UmiEditorSnippetSession *)calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->revision = 1U;
    session->state = UMI_EDITOR_SNIPPET_IDLE;
    *out_session = session;
    return UMI_STATUS_OK;
}

void umi_editor_snippet_session_destroy(UmiEditorSnippetSession *session)
{
    if (session == NULL) return;
    free(session->placeholders);
    session->placeholders = NULL;
    free(session);
}

UmiStatus umi_editor_snippet_session_start(
    UmiEditorSnippetSession *session,
    const UmiEditorSnippetTemplate *snippet,
    uint64_t insertion_byte_offset)
{
    UmiStatus status;

    if (session == NULL || validate_template(snippet) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->snippet = *snippet;
    session->placeholder_count = 0U;
    session->expanded_length = 0U;
    session->expanded[0] = '\0';
    session->insertion_byte_offset = insertion_byte_offset;
    session->has_active = 0;
    status = parse_body(session);
    if (status != UMI_STATUS_OK) {
        session->state = UMI_EDITOR_SNIPPET_FAILED;
        session->revision = next_revision(session->revision);
        return status;
    }
    session->active_index = first_traversal_index(session);
    session->has_active = session->active_index != SIZE_MAX;
    session->state = session->has_active
        ? UMI_EDITOR_SNIPPET_ACTIVE : UMI_EDITOR_SNIPPET_COMPLETED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_snippet_session_select(
    UmiEditorSnippetSession *session,
    uint32_t ordinal)
{
    size_t index;

    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->state != UMI_EDITOR_SNIPPET_ACTIVE) {
        return UMI_STATUS_INVALID_STATE;
    }
    index = primary_for_ordinal(session, ordinal);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    session->active_index = index;
    session->has_active = 1;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_snippet_session_next(UmiEditorSnippetSession *session)
{
    uint32_t active_ordinal;
    uint32_t selected_ordinal = UINT32_MAX;
    size_t index;
    size_t selected = SIZE_MAX;

    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->state != UMI_EDITOR_SNIPPET_ACTIVE || !session->has_active) {
        return UMI_STATUS_INVALID_STATE;
    }
    active_ordinal = session->placeholders[session->active_index].ordinal;
    if (active_ordinal == 0U) {
        session->has_active = 0;
        session->state = UMI_EDITOR_SNIPPET_COMPLETED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_OK;
    }
    for (index = 0U; index < session->placeholder_count; ++index) {
        const UmiEditorSnippetPlaceholder *placeholder =
            &session->placeholders[index];
        if (!placeholder->primary || placeholder->ordinal == 0U ||
            placeholder->ordinal <= active_ordinal) {
            continue;
        }
        if (placeholder->ordinal < selected_ordinal) {
            selected_ordinal = placeholder->ordinal;
            selected = index;
        }
    }
    if (selected == SIZE_MAX) selected = primary_for_ordinal(session, 0U);
    if (selected == SIZE_MAX) {
        session->has_active = 0;
        session->state = UMI_EDITOR_SNIPPET_COMPLETED;
    } else {
        session->active_index = selected;
    }
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_snippet_session_previous(UmiEditorSnippetSession *session)
{
    uint32_t active_ordinal;
    uint32_t selected_ordinal = 0U;
    size_t index;
    size_t selected = SIZE_MAX;

    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->state != UMI_EDITOR_SNIPPET_ACTIVE || !session->has_active) {
        return UMI_STATUS_INVALID_STATE;
    }
    active_ordinal = session->placeholders[session->active_index].ordinal;
    for (index = 0U; index < session->placeholder_count; ++index) {
        const UmiEditorSnippetPlaceholder *placeholder =
            &session->placeholders[index];
        if (!placeholder->primary || placeholder->ordinal == 0U) continue;
        if ((active_ordinal == 0U || placeholder->ordinal < active_ordinal) &&
            (selected == SIZE_MAX || placeholder->ordinal > selected_ordinal)) {
            selected_ordinal = placeholder->ordinal;
            selected = index;
        }
    }
    if (selected == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    session->active_index = selected;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_snippet_session_cancel(UmiEditorSnippetSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->state != UMI_EDITOR_SNIPPET_ACTIVE) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->has_active = 0;
    session->state = UMI_EDITOR_SNIPPET_CANCELLED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_snippet_session_active(
    const UmiEditorSnippetSession *session,
    UmiEditorSnippetPlaceholder *out_placeholder)
{
    if (session == NULL || out_placeholder == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!session->has_active || session->state != UMI_EDITOR_SNIPPET_ACTIVE) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_placeholder = session->placeholders[session->active_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_snippet_session_placeholder_at(
    const UmiEditorSnippetSession *session,
    size_t index,
    UmiEditorSnippetPlaceholder *out_placeholder)
{
    if (session == NULL || out_placeholder == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= session->placeholder_count) return UMI_STATUS_NOT_FOUND;
    *out_placeholder = session->placeholders[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_snippet_session_expanded_text(
    const UmiEditorSnippetSession *session,
    char *out_text,
    size_t out_capacity)
{
    if (session == NULL || out_text == NULL || out_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (session->expanded_length >= out_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(out_text, session->expanded, session->expanded_length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_snippet_session_snapshot(
    const UmiEditorSnippetSession *session,
    UmiEditorSnippetSessionSnapshot *out_snapshot)
{
    size_t index;

    if (session == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_SNIPPET_SESSION_API_VERSION;
    (void)memcpy(out_snapshot->template_id, session->snippet.id,
                 sizeof(out_snapshot->template_id));
    out_snapshot->state = session->state;
    out_snapshot->placeholder_count = session->placeholder_count;
    out_snapshot->expanded_length = session->expanded_length;
    out_snapshot->insertion_byte_offset = session->insertion_byte_offset;
    out_snapshot->revision = session->revision;
    out_snapshot->has_active_placeholder = session->has_active;
    if (session->has_active) {
        out_snapshot->active_placeholder_index = session->active_index;
        out_snapshot->active_ordinal =
            session->placeholders[session->active_index].ordinal;
    }
    for (index = 0U; index < session->placeholder_count; ++index) {
        if (session->placeholders[index].primary) {
            ++out_snapshot->traversal_stop_count;
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_snippet_session_placeholder_count(
    const UmiEditorSnippetSession *session)
{
    return session != NULL ? session->placeholder_count : 0U;
}

uint64_t umi_editor_snippet_session_revision(
    const UmiEditorSnippetSession *session)
{
    return session != NULL ? session->revision : 0U;
}
