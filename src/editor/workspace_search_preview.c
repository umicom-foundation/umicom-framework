/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workspace_search_preview.c
 *
 * PURPOSE:
 *   Expand replacement templates against immutable search results, validate
 *   indexed source identity and create bounded before/after preview context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workspace_search_preview.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorWorkspaceReplacementPreview {
    UmiEditorWorkspaceReplacementPreviewItem *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
    uint64_t source_query_revision;
    uint64_t source_index_revision;
    int ready;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the reserve items operation used by this module and its client applications. */
static UmiStatus reserve_items(UmiEditorWorkspaceReplacementPreview *preview,
                               size_t required)
{
    size_t capacity;
    UmiEditorWorkspaceReplacementPreviewItem *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= preview->capacity) return UMI_STATUS_OK;
    capacity = preview->capacity > 0U ? preview->capacity : 128U;
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
    replacement = (UmiEditorWorkspaceReplacementPreviewItem *)realloc(
        preview->items, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    preview->items = replacement;
    preview->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find item operation used by this module and its client applications. */
static size_t find_item(const UmiEditorWorkspaceReplacementPreview *preview,
                        const char *match_id)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || match_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < preview->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(preview->items[position].match.id, match_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

/* Provide the append bytes operation used by this module and its client applications. */
static UmiStatus append_bytes(char *output,
                              size_t capacity,
                              size_t *length,
                              const char *bytes,
                              size_t byte_count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || length == NULL || bytes == NULL ||
        *length >= capacity || byte_count > capacity - *length - 1U) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (byte_count > 0U) {
        (void)memcpy(output + *length, bytes, byte_count);
        *length += byte_count;
    }
    output[*length] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the append character operation used by this module and its client applications. */
static UmiStatus append_character(char *output,
                                  size_t capacity,
                                  size_t *length,
                                  char value)
{
    return append_bytes(output, capacity, length, &value, 1U);
}

/* Provide the append capture operation used by this module and its client applications. */
static UmiStatus append_capture(
    const UmiEditorWorkspaceSearchDocumentView *document,
    const UmiEditorWorkspaceSearchMatch *match,
    size_t capture_index,
    char *output,
    size_t capacity,
    size_t *length)
{
    uint64_t start;
    uint64_t end;
    /* Apply this branch only when its contract condition is satisfied. */
    if (capture_index == 0U) {
        start = match->location.byte_offset;
        end = match->location.end_byte_offset;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capture_index >= match->capture_count ||
            !match->captures[capture_index].matched) {
            return UMI_STATUS_OK;
        }
        start = match->captures[capture_index].start_byte_offset;
        end = match->captures[capture_index].end_byte_offset;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (start > SIZE_MAX || end > SIZE_MAX || end < start ||
        (size_t)end > document->content_length) {
        return UMI_STATUS_INVALID_STATE;
    }
    return append_bytes(output,
                        capacity,
                        length,
                        document->content + (size_t)start,
                        (size_t)(end - start));
}

/* Provide the expand template operation used by this module and its client applications. */
static UmiStatus expand_template(
    const UmiEditorWorkspaceReplacementRequest *request,
    const UmiEditorWorkspaceSearchDocumentView *document,
    const UmiEditorWorkspaceSearchMatch *match,
    char *output,
    size_t capacity)
{
    size_t input_position = 0U;
    size_t output_length = 0U;
    const char *input = request->replacement_template;
    output[0] = '\0';
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (input[input_position] != '\0') {
        UmiStatus status;
        char value = input[input_position++];
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '$') {
            char token = input[input_position];
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (token == '$') {
                ++input_position;
                status = append_character(output, capacity, &output_length, '$');
            } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (token == '&') {
                ++input_position;
                status = append_capture(document, match, 0U,
                                        output, capacity, &output_length);
            } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (token >= '0' && token <= '8') {
                ++input_position;
                status = append_capture(document,
                                        match,
                                        (size_t)(token - '0'),
                                        output,
                                        capacity,
                                        &output_length);
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                status = append_character(output, capacity, &output_length, '$');
            }
        } else /* Apply this branch only when its contract condition is satisfied. */ if (value == '\\' && request->interpret_escapes &&
                   input[input_position] != '\0') {
            char token = input[input_position++];
            char expanded;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (token == 'n') expanded = '\n';
            else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (token == 'r') expanded = '\r';
            else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (token == 't') expanded = '\t';
            else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (token == '\\') expanded = '\\';
            /* Use this fallback path when the earlier condition does not apply. */
            else {
                status = append_character(output, capacity,
                                          &output_length, '\\');
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) return status;
                expanded = token;
            }
            status = append_character(output, capacity,
                                      &output_length, expanded);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            status = append_character(output, capacity, &output_length, value);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/* Provide the preserve case operation used by this module and its client applications. */
static void preserve_case(const char *matched_text, char *replacement)
{
    size_t index;
    int has_alpha = 0;
    int all_upper = 1;
    int all_lower = 1;
    int first_alpha_seen = 0;
    int title_case = 1;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; matched_text[index] != '\0'; ++index) {
        unsigned char value = (unsigned char)matched_text[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (isalpha(value)) {
            has_alpha = 1;
            /* Apply this branch only when its contract condition is satisfied. */
            if (!isupper(value)) all_upper = 0;
            /* Apply this branch only when its contract condition is satisfied. */
            if (!islower(value)) all_lower = 0;
            /* Apply this branch only when its contract condition is satisfied. */
            if (!first_alpha_seen) {
                /* Apply this branch only when its contract condition is satisfied. */
                if (!isupper(value)) title_case = 0;
                first_alpha_seen = 1;
            } else /* Apply this branch only when its contract condition is satisfied. */ if (!islower(value)) {
                title_case = 0;
            }
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!has_alpha) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (all_upper) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; replacement[index] != '\0'; ++index) {
            replacement[index] =
                (char)toupper((unsigned char)replacement[index]);
        }
    } else /* Apply this branch only when its contract condition is satisfied. */ if (all_lower) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; replacement[index] != '\0'; ++index) {
            replacement[index] =
                (char)tolower((unsigned char)replacement[index]);
        }
    } else /* Apply this branch only when its contract condition is satisfied. */ if (title_case) {
        int changed_first = 0;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; replacement[index] != '\0'; ++index) {
            unsigned char value = (unsigned char)replacement[index];
            /* Apply this branch only when its contract condition is satisfied. */
            if (isalpha(value)) {
                replacement[index] = changed_first
                    ? (char)tolower(value) : (char)toupper(value);
                changed_first = 1;
            }
        }
    }
}

/* Provide the make context operation used by this module and its client applications. */
static void make_context(
    const UmiEditorWorkspaceSearchDocumentView *document,
    const UmiEditorWorkspaceSearchMatch *match,
    const char *replacement,
    char *out_original,
    char *out_replacement,
    size_t capacity)
{
    size_t start = (size_t)match->location.byte_offset;
    size_t end = (size_t)match->location.end_byte_offset;
    size_t context_start = start;
    size_t context_end = end;
    size_t original_length = 0U;
    size_t replacement_length = 0U;
    const size_t context_wing = 220U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (context_start > context_wing) context_start -= context_wing;
    /* Use this fallback path when the earlier condition does not apply. */
    else context_start = 0U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (context_end < document->content_length) {
        size_t remaining = document->content_length - context_end;
        context_end += remaining < context_wing ? remaining : context_wing;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (context_start < start &&
           document->content[context_start] != '\n' &&
           document->content[context_start] != '\r') {
        ++context_start;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (context_start < start) ++context_start;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (context_end > end &&
           document->content[context_end - 1U] != '\n' &&
           document->content[context_end - 1U] != '\r') {
        --context_end;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (context_end <= end) {
        context_end = end;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (context_end < document->content_length &&
               document->content[context_end] != '\n' &&
               document->content[context_end] != '\r' &&
               context_end - end < context_wing) {
            ++context_end;
        }
    }
    out_original[0] = '\0';
    out_replacement[0] = '\0';
    (void)append_bytes(out_original, capacity, &original_length,
                       document->content + context_start,
                       context_end - context_start);
    (void)append_bytes(out_replacement, capacity, &replacement_length,
                       document->content + context_start,
                       start - context_start);
    (void)append_bytes(out_replacement, capacity, &replacement_length,
                       replacement, strlen(replacement));
    (void)append_bytes(out_replacement, capacity, &replacement_length,
                       document->content + end,
                       context_end - end);
}

/* Provide the set state operation used by this module and its client applications. */
static void set_state(UmiEditorWorkspaceReplacementPreviewItem *item,
                      UmiEditorWorkspaceReplacementPreviewState state,
                      const char *reason)
{
    item->state = state;
    item->applicable = state == UMI_EDITOR_WORKSPACE_REPLACEMENT_PREVIEW_READY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!item->applicable) item->selected = 0;
    (void)snprintf(item->reason, sizeof(item->reason), "%s", reason);
}

/*
 * Initialise editor workspace replacement request from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_workspace_replacement_request_init(
    UmiEditorWorkspaceReplacementRequest *request,
    const char *replacement_template)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL) return;
    (void)memset(request, 0, sizeof(*request));
    request->struct_size = (uint32_t)sizeof(*request);
    request->api_version = UMI_EDITOR_WORKSPACE_SEARCH_PREVIEW_API_VERSION;
    request->interpret_escapes = 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement_template != NULL) {
        (void)snprintf(request->replacement_template,
                       sizeof(request->replacement_template),
                       "%s",
                       replacement_template);
    }
}

/*
 * Initialise editor workspace replacement preview from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_workspace_replacement_preview_create(
    UmiEditorWorkspaceReplacementPreview **out_preview)
{
    UmiEditorWorkspaceReplacementPreview *preview;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_preview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_preview = NULL;
    preview = (UmiEditorWorkspaceReplacementPreview *)calloc(
        1U, sizeof(*preview));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    preview->revision = 1U;
    *out_preview = preview;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor workspace replacement preview so the same storage
 * can be reused safely.
 */
void umi_editor_workspace_replacement_preview_destroy(
    UmiEditorWorkspaceReplacementPreview *preview)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL) return;
    free(preview->items);
    preview->items = NULL;
    free(preview);
}

/*
 * Release or reset state held by editor workspace replacement preview so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_workspace_replacement_preview_clear(
    UmiEditorWorkspaceReplacementPreview *preview)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    preview->count = 0U;
    preview->ready = 0;
    preview->source_query_revision = 0U;
    preview->source_index_revision = 0U;
    preview->revision = next_revision(preview->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor workspace replacement preview build operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_workspace_replacement_preview_build(
    UmiEditorWorkspaceReplacementPreview *preview,
    const UmiEditorWorkspaceSearchIndex *index,
    const UmiEditorWorkspaceSearchQuery *query,
    const UmiEditorWorkspaceReplacementRequest *request)
{
    UmiEditorWorkspaceSearchQuerySnapshot query_snapshot;
    UmiEditorWorkspaceSearchIndexSnapshot index_snapshot;
    size_t position;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || index == NULL || query == NULL || request == NULL ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version !=
            UMI_EDITOR_WORKSPACE_SEARCH_PREVIEW_API_VERSION ||
        memchr(request->replacement_template, '\0',
               sizeof(request->replacement_template)) == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_workspace_search_query_snapshot(query, &query_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_workspace_search_index_snapshot(index, &index_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    preview->count = 0U;
    preview->ready = 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < query_snapshot.result_count; ++position) {
        UmiEditorWorkspaceSearchMatch match;
        UmiEditorWorkspaceSearchDocumentView document;
        UmiEditorWorkspaceReplacementPreviewItem *item;
        status = umi_editor_workspace_search_query_at(query, position, &match);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = reserve_items(preview, preview->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        item = &preview->items[preview->count++];
        (void)memset(item, 0, sizeof(*item));
        item->struct_size = (uint32_t)sizeof(*item);
        item->api_version = UMI_EDITOR_WORKSPACE_SEARCH_PREVIEW_API_VERSION;
        item->match = match;
        item->selected = match.selected;
        item->source_query_revision = query_snapshot.revision;
        item->source_index_revision = index_snapshot.revision;
        status = umi_editor_workspace_search_index_find_uri(
            index, match.location.uri, &document);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK ||
            document.fingerprint != match.document_fingerprint ||
            (match.location.document_revision != 0U &&
             document.document_revision != match.location.document_revision)) {
            set_state(item,
                      UMI_EDITOR_WORKSPACE_REPLACEMENT_PREVIEW_STALE,
                      "The indexed document changed after this match was found.");
            continue;
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (match.truncated_text) {
            set_state(item,
                      UMI_EDITOR_WORKSPACE_REPLACEMENT_PREVIEW_TRUNCATED_MATCH,
                      "The matched text exceeds the transaction-safe capacity.");
            continue;
        }
        status = expand_template(request,
                                 &document,
                                 &match,
                                 item->replacement_text,
                                 sizeof(item->replacement_text));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
            set_state(item,
                      UMI_EDITOR_WORKSPACE_REPLACEMENT_PREVIEW_REPLACEMENT_TOO_LARGE,
                      "The expanded replacement exceeds the transaction-safe capacity.");
            continue;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            set_state(item,
                      UMI_EDITOR_WORKSPACE_REPLACEMENT_PREVIEW_INVALID_TEMPLATE,
                      "The replacement template could not be expanded.");
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (request->preserve_case) {
            preserve_case(match.matched_text, item->replacement_text);
        }
        make_context(&document,
                     &match,
                     item->replacement_text,
                     item->original_context,
                     item->replacement_context,
                     sizeof(item->original_context));
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (document.read_only || match.read_only) {
            set_state(item,
                      UMI_EDITOR_WORKSPACE_REPLACEMENT_PREVIEW_READ_ONLY,
                      "The owning document is read-only.");
            continue;
        }
        set_state(item,
                  UMI_EDITOR_WORKSPACE_REPLACEMENT_PREVIEW_READY,
                  "Replacement is ready for transactional planning.");
    }
    preview->source_query_revision = query_snapshot.revision;
    preview->source_index_revision = index_snapshot.revision;
    preview->ready = 1;
    preview->revision = next_revision(preview->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor workspace replacement preview while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_workspace_replacement_preview_at(
    const UmiEditorWorkspaceReplacementPreview *preview,
    size_t position,
    UmiEditorWorkspaceReplacementPreviewItem *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= preview->count) return UMI_STATUS_NOT_FOUND;
    *out_item = preview->items[position];
    return UMI_STATUS_OK;
}

/*
 * Find editor workspace replacement preview while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_workspace_replacement_preview_find(
    const UmiEditorWorkspaceReplacementPreview *preview,
    const char *match_id,
    UmiEditorWorkspaceReplacementPreviewItem *out_item)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || match_id == NULL || match_id[0] == '\0' ||
        out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_item(preview, match_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = preview->items[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor workspace replacement preview select operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_replacement_preview_select(
    UmiEditorWorkspaceReplacementPreview *preview,
    const char *match_id,
    int selected)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || match_id == NULL || match_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_item(preview, match_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (preview->items[position].selected != (selected != 0)) {
        preview->items[position].selected = selected != 0;
        preview->revision = next_revision(preview->revision);
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the editor workspace replacement preview select all operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_workspace_replacement_preview_select_all(
    UmiEditorWorkspaceReplacementPreview *preview,
    int selected)
{
    size_t position;
    int changed = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < preview->count; ++position) {
        int desired = selected != 0 && preview->items[position].applicable;
        /* Apply this branch only when its contract condition is satisfied. */
        if (preview->items[position].selected != desired) {
            preview->items[position].selected = desired;
            changed = 1;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (changed) preview->revision = next_revision(preview->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor workspace replacement preview snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_replacement_preview_snapshot(
    const UmiEditorWorkspaceReplacementPreview *preview,
    UmiEditorWorkspaceReplacementPreviewSnapshot *out_snapshot)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_EDITOR_WORKSPACE_SEARCH_PREVIEW_API_VERSION;
    out_snapshot->item_count = preview->count;
    out_snapshot->source_query_revision = preview->source_query_revision;
    out_snapshot->source_index_revision = preview->source_index_revision;
    out_snapshot->revision = preview->revision;
    out_snapshot->ready = preview->ready;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < preview->count; ++position) {
        const UmiEditorWorkspaceReplacementPreviewItem *item =
            &preview->items[position];
        size_t comparison;
        int first_document = 1;
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->selected) ++out_snapshot->selected_item_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->applicable) ++out_snapshot->applicable_item_count;
        /* Use this fallback path when the earlier condition does not apply. */
        else ++out_snapshot->conflict_item_count;
        /* Visit each bounded item once so every record receives the same rule. */
        for (comparison = 0U; comparison < position; ++comparison) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(item->match.location.uri,
                       preview->items[comparison].match.location.uri) == 0) {
                first_document = 0;
                break;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (first_document) ++out_snapshot->document_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor workspace replacement preview without
 * changing their state.
 */
size_t umi_editor_workspace_replacement_preview_count(
    const UmiEditorWorkspaceReplacementPreview *preview)
{
    return preview != NULL ? preview->count : 0U;
}

/*
 * Provide the editor workspace replacement preview revision operation used by this module
 * and its client applications.
 */
uint64_t umi_editor_workspace_replacement_preview_revision(
    const UmiEditorWorkspaceReplacementPreview *preview)
{
    return preview != NULL ? preview->revision : 0U;
}
