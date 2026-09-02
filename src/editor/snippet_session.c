/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/snippet_session.c
 *
 * PURPOSE:
 *   Parse bounded snippet syntax into expanded text and deterministic cursor
 *   traversal stops without depending on a graphical editor toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the validate template operation used by this module and its client applications. */
static UmiStatus validate_template(const UmiEditorSnippetTemplate *snippet)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the reserve placeholders operation used by this module and its client
 * applications.
 */
static UmiStatus reserve_placeholders(UmiEditorSnippetSession *session,
                                      size_t required)
{
    size_t capacity;
    UmiEditorSnippetPlaceholder *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= session->placeholder_capacity) return UMI_STATUS_OK;
    capacity = session->placeholder_capacity > 0U
        ? session->placeholder_capacity : 16U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorSnippetPlaceholder *)realloc(
        session->placeholders, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->placeholders = replacement;
    session->placeholder_capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the append text operation used by this module and its client applications. */
static UmiStatus append_text(UmiEditorSnippetSession *session,
                             const char *text,
                             size_t length)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > UMI_EDITOR_SNIPPET_EXPANDED_CAPACITY - 1U ||
        session->expanded_length >
            UMI_EDITOR_SNIPPET_EXPANDED_CAPACITY - 1U - length) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) {
        (void)memcpy(&session->expanded[session->expanded_length], text,
                     length);
        session->expanded_length += length;
    }
    session->expanded[session->expanded_length] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the ordinal seen operation used by this module and its client applications. */
static int ordinal_seen(const UmiEditorSnippetSession *session,
                        uint32_t ordinal)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->placeholder_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (session->placeholders[index].ordinal == ordinal) return 1;
    }
    return 0;
}

/*
 * Provide the primary placeholder operation used by this module and its client
 * applications.
 */
static const UmiEditorSnippetPlaceholder *primary_placeholder(
    const UmiEditorSnippetSession *session,
    uint32_t ordinal)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->placeholder_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (session->placeholders[index].ordinal == ordinal &&
            session->placeholders[index].primary) {
            return &session->placeholders[index];
        }
    }
    return NULL;
}

/* Provide the add placeholder operation used by this module and its client applications. */
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

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (default_length >= UMI_EDITOR_SNIPPET_DEFAULT_CAPACITY ||
        choices_length >= UMI_EDITOR_SNIPPET_CHOICES_CAPACITY ||
        session->insertion_byte_offset > UINT64_MAX - (uint64_t)end) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = reserve_placeholders(session, session->placeholder_count + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&placeholder, 0, sizeof(placeholder));
    placeholder.struct_size = (uint32_t)sizeof(placeholder);
    placeholder.api_version = UMI_EDITOR_SNIPPET_SESSION_API_VERSION;
    placeholder.ordinal = ordinal;
    placeholder.start_byte_offset = session->insertion_byte_offset +
                                    (uint64_t)start;
    placeholder.end_byte_offset = session->insertion_byte_offset +
                                  (uint64_t)end;
    /* Apply this branch only when its contract condition is satisfied. */
    if (default_length > 0U) {
        (void)memcpy(placeholder.default_text, default_text, default_length);
    }
    placeholder.default_text[default_length] = '\0';
    /* Apply this branch only when its contract condition is satisfied. */
    if (choices_length > 0U) {
        (void)memcpy(placeholder.choices, choices, choices_length);
    }
    placeholder.choices[choices_length] = '\0';
    placeholder.primary = !ordinal_seen(session, ordinal);
    placeholder.final_stop = ordinal == 0U;
    session->placeholders[session->placeholder_count++] = placeholder;
    return UMI_STATUS_OK;
}

/* Provide the parse ordinal operation used by this module and its client applications. */
static int parse_ordinal(const char *text,
                         size_t length,
                         size_t *in_out_position,
                         uint32_t *out_ordinal)
{
    size_t position = *in_out_position;
    uint64_t value = 0U;
    int found = 0;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (position < length && text[position] >= '0' &&
           text[position] <= '9') {
        uint32_t digit = (uint32_t)(text[position] - '0');
        /* Apply this branch only when its contract condition is satisfied. */
        if (value > ((uint64_t)UINT32_MAX - digit) / 10U) return 0;
        value = value * 10U + digit;
        ++position;
        found = 1;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!found) return 0;
    *in_out_position = position;
    *out_ordinal = (uint32_t)value;
    return 1;
}

/*
 * Provide the parse braced placeholder operation used by this module and its client
 * applications.
 */
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

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!parse_ordinal(body, length, &position, &ordinal)) {
        return UMI_STATUS_PARSE_ERROR;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position < length && body[position] == '}') {
        const UmiEditorSnippetPlaceholder *primary =
            primary_placeholder(session, ordinal);
        char default_text[UMI_EDITOR_SNIPPET_DEFAULT_CAPACITY] = "";
        size_t default_length = primary != NULL
            ? strlen(primary->default_text) : 0U;

        /* Apply this branch only when its contract condition is satisfied. */
        if (default_length > 0U) {
            (void)memcpy(default_text, primary->default_text,
                         default_length + 1U);
        }

        status = append_text(session, default_text, default_length);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = add_placeholder(session, ordinal, expanded_start,
                                     session->expanded_length, default_text,
                                     default_length, "", 0U);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) *in_out_position = position + 1U;
        return status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position < length && body[position] == ':') {
        content_start = ++position;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (position < length && body[position] != '}') ++position;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (position >= length) return UMI_STATUS_PARSE_ERROR;
        content_end = position;
        status = append_text(session, &body[content_start],
                             content_end - content_start);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = add_placeholder(session, ordinal, expanded_start,
                                     session->expanded_length,
                                     &body[content_start],
                                     content_end - content_start, "", 0U);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) *in_out_position = position + 1U;
        return status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position < length && body[position] == '|') {
        content_start = ++position;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (position + 1U < length &&
               !(body[position] == '|' && body[position + 1U] == '}')) {
            ++position;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (position + 1U >= length) return UMI_STATUS_PARSE_ERROR;
        content_end = position;
        choice_end = content_start;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (choice_end < content_end && body[choice_end] != ',') {
            ++choice_end;
        }
        status = append_text(session, &body[content_start],
                             choice_end - content_start);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = add_placeholder(session, ordinal, expanded_start,
                                     session->expanded_length,
                                     &body[content_start],
                                     choice_end - content_start,
                                     &body[content_start],
                                     content_end - content_start);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) *in_out_position = position + 2U;
        return status;
    }
    return UMI_STATUS_PARSE_ERROR;
}

/* Provide the parse body operation used by this module and its client applications. */
static UmiStatus parse_body(UmiEditorSnippetSession *session)
{
    const char *body = session->snippet.body;
    size_t length = strlen(body);
    size_t position = 0U;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (position < length) {
        UmiStatus status;

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (body[position] == '$' && position + 1U < length &&
            body[position + 1U] == '{') {
            size_t parsed_position = position;
            status = parse_braced_placeholder(session, body, length,
                                               &parsed_position);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) {
                position = parsed_position;
                continue;
            }
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_PARSE_ERROR) return status;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (body[position] == '$' && position + 1U < length &&
                   body[position + 1U] >= '0' &&
                   body[position + 1U] <= '9') {
            size_t parsed_position = position + 1U;
            uint32_t ordinal;

            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (parse_ordinal(body, length, &parsed_position, &ordinal)) {
                const UmiEditorSnippetPlaceholder *primary =
                    primary_placeholder(session, ordinal);
                char default_text[UMI_EDITOR_SNIPPET_DEFAULT_CAPACITY] = "";
                size_t default_length = primary != NULL
                    ? strlen(primary->default_text) : 0U;
                size_t expanded_start = session->expanded_length;

                /* Apply this branch only when its contract condition is satisfied. */
                if (default_length > 0U) {
                    (void)memcpy(default_text, primary->default_text,
                                 default_length + 1U);
                }
                status = append_text(session, default_text, default_length);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) {
                    status = add_placeholder(session, ordinal, expanded_start,
                                             session->expanded_length,
                                             default_text, default_length,
                                             "", 0U);
                }
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) return status;
                position = parsed_position;
                continue;
            }
        }
        status = append_text(session, &body[position], 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        ++position;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the primary for ordinal operation used by this module and its client
 * applications.
 */
static size_t primary_for_ordinal(const UmiEditorSnippetSession *session,
                                  uint32_t ordinal)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->placeholder_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (session->placeholders[index].ordinal == ordinal &&
            session->placeholders[index].primary) {
            return index;
        }
    }
    return SIZE_MAX;
}

/*
 * Provide the first traversal index operation used by this module and its client
 * applications.
 */
static size_t first_traversal_index(const UmiEditorSnippetSession *session)
{
    size_t index;
    size_t selected = SIZE_MAX;
    uint32_t ordinal = UINT32_MAX;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->placeholder_count; ++index) {
        const UmiEditorSnippetPlaceholder *placeholder =
            &session->placeholders[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!placeholder->primary || placeholder->ordinal == 0U) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (placeholder->ordinal < ordinal) {
            ordinal = placeholder->ordinal;
            selected = index;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (selected == SIZE_MAX) selected = primary_for_ordinal(session, 0U);
    return selected;
}

/*
 * Initialise editor snippet session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_snippet_session_create(
    UmiEditorSnippetSession **out_session)
{
    UmiEditorSnippetSession *session;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_session = NULL;
    session = (UmiEditorSnippetSession *)calloc(1U, sizeof(*session));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->revision = 1U;
    session->state = UMI_EDITOR_SNIPPET_IDLE;
    *out_session = session;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor snippet session so the same storage can be reused
 * safely.
 */
void umi_editor_snippet_session_destroy(UmiEditorSnippetSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    free(session->placeholders);
    session->placeholders = NULL;
    free(session);
}

/*
 * Provide the editor snippet session start operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_snippet_session_start(
    UmiEditorSnippetSession *session,
    const UmiEditorSnippetTemplate *snippet,
    uint64_t insertion_byte_offset)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the editor snippet session select operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_snippet_session_select(
    UmiEditorSnippetSession *session,
    uint32_t ordinal)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this operation only while the related capability or state is available. */
    if (session->state != UMI_EDITOR_SNIPPET_ACTIVE) {
        return UMI_STATUS_INVALID_STATE;
    }
    index = primary_for_ordinal(session, ordinal);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    session->active_index = index;
    session->has_active = 1;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor snippet session next operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_snippet_session_next(UmiEditorSnippetSession *session)
{
    uint32_t active_ordinal;
    uint32_t selected_ordinal = UINT32_MAX;
    size_t index;
    size_t selected = SIZE_MAX;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this operation only while the related capability or state is available. */
    if (session->state != UMI_EDITOR_SNIPPET_ACTIVE || !session->has_active) {
        return UMI_STATUS_INVALID_STATE;
    }
    active_ordinal = session->placeholders[session->active_index].ordinal;
    /* Apply this operation only while the related capability or state is available. */
    if (active_ordinal == 0U) {
        session->has_active = 0;
        session->state = UMI_EDITOR_SNIPPET_COMPLETED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_OK;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->placeholder_count; ++index) {
        const UmiEditorSnippetPlaceholder *placeholder =
            &session->placeholders[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!placeholder->primary || placeholder->ordinal == 0U ||
            placeholder->ordinal <= active_ordinal) {
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (placeholder->ordinal < selected_ordinal) {
            selected_ordinal = placeholder->ordinal;
            selected = index;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (selected == SIZE_MAX) selected = primary_for_ordinal(session, 0U);
    /* Apply this branch only when its contract condition is satisfied. */
    if (selected == SIZE_MAX) {
        session->has_active = 0;
        session->state = UMI_EDITOR_SNIPPET_COMPLETED;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        session->active_index = selected;
    }
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor snippet session previous operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_snippet_session_previous(UmiEditorSnippetSession *session)
{
    uint32_t active_ordinal;
    uint32_t selected_ordinal = 0U;
    size_t index;
    size_t selected = SIZE_MAX;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this operation only while the related capability or state is available. */
    if (session->state != UMI_EDITOR_SNIPPET_ACTIVE || !session->has_active) {
        return UMI_STATUS_INVALID_STATE;
    }
    active_ordinal = session->placeholders[session->active_index].ordinal;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->placeholder_count; ++index) {
        const UmiEditorSnippetPlaceholder *placeholder =
            &session->placeholders[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!placeholder->primary || placeholder->ordinal == 0U) continue;
        /* Apply this operation only while the related capability or state is available. */
        if ((active_ordinal == 0U || placeholder->ordinal < active_ordinal) &&
            (selected == SIZE_MAX || placeholder->ordinal > selected_ordinal)) {
            selected_ordinal = placeholder->ordinal;
            selected = index;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (selected == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    session->active_index = selected;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor snippet session cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_snippet_session_cancel(UmiEditorSnippetSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this operation only while the related capability or state is available. */
    if (session->state != UMI_EDITOR_SNIPPET_ACTIVE) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->has_active = 0;
    session->state = UMI_EDITOR_SNIPPET_CANCELLED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor snippet session active operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_snippet_session_active(
    const UmiEditorSnippetSession *session,
    UmiEditorSnippetPlaceholder *out_placeholder)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_placeholder == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!session->has_active || session->state != UMI_EDITOR_SNIPPET_ACTIVE) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_placeholder = session->placeholders[session->active_index];
    return UMI_STATUS_OK;
}

/*
 * Find editor snippet session placeholder while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_snippet_session_placeholder_at(
    const UmiEditorSnippetSession *session,
    size_t index,
    UmiEditorSnippetPlaceholder *out_placeholder)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_placeholder == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= session->placeholder_count) return UMI_STATUS_NOT_FOUND;
    *out_placeholder = session->placeholders[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor snippet session expanded text operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_snippet_session_expanded_text(
    const UmiEditorSnippetSession *session,
    char *out_text,
    size_t out_capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_text == NULL || out_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->expanded_length >= out_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(out_text, session->expanded, session->expanded_length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor snippet session snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_snippet_session_snapshot(
    const UmiEditorSnippetSession *session,
    UmiEditorSnippetSessionSnapshot *out_snapshot)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Apply this operation only while the related capability or state is available. */
    if (session->has_active) {
        out_snapshot->active_placeholder_index = session->active_index;
        out_snapshot->active_ordinal =
            session->placeholders[session->active_index].ordinal;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->placeholder_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (session->placeholders[index].primary) {
            ++out_snapshot->traversal_stop_count;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor snippet session placeholder without
 * changing their state.
 */
size_t umi_editor_snippet_session_placeholder_count(
    const UmiEditorSnippetSession *session)
{
    return session != NULL ? session->placeholder_count : 0U;
}

/*
 * Provide the editor snippet session revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_snippet_session_revision(
    const UmiEditorSnippetSession *session)
{
    return session != NULL ? session->revision : 0U;
}
