/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workspace_search_match.c
 *
 * PURPOSE:
 *   Convert portable regex/literal engine offsets into stable source locations,
 *   bounded matched text, line previews and deterministic result identities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workspace_search_match.h"

#include <stdio.h>
#include <string.h>

static uint64_t hash_text(const char *text)
{
    uint64_t value = UINT64_C(1469598103934665603);
    size_t index;
    for (index = 0U; text[index] != '\0'; ++index) {
        value ^= (uint64_t)(unsigned char)text[index];
        value *= UINT64_C(1099511628211);
    }
    return value;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static void locate_offset(const char *content,
                          size_t content_length,
                          size_t offset,
                          uint64_t *out_line,
                          uint64_t *out_column)
{
    size_t position;
    uint64_t line = 1U;
    uint64_t column = 1U;
    if (offset > content_length) offset = content_length;
    for (position = 0U; position < offset; ++position) {
        if (content[position] == '\n') {
            line += 1U;
            column = 1U;
        } else {
            column += 1U;
        }
    }
    *out_line = line;
    *out_column = column;
}

static void copy_bounded(char *destination,
                         size_t capacity,
                         const char *source,
                         size_t source_length,
                         int *out_truncated)
{
    size_t copy_count = source_length;
    if (copy_count >= capacity) {
        copy_count = capacity - 1U;
        if (out_truncated != NULL) *out_truncated = 1;
    }
    if (copy_count > 0U) (void)memcpy(destination, source, copy_count);
    destination[copy_count] = '\0';
}

static void make_line_preview(const char *content,
                              size_t content_length,
                              size_t match_start,
                              char *out_preview,
                              size_t capacity)
{
    size_t line_start = match_start;
    size_t line_end = match_start;
    while (line_start > 0U && content[line_start - 1U] != '\n' &&
           content[line_start - 1U] != '\r') {
        --line_start;
    }
    while (line_end < content_length && content[line_end] != '\n' &&
           content[line_end] != '\r') {
        ++line_end;
    }
    copy_bounded(out_preview,
                 capacity,
                 content + line_start,
                 line_end - line_start,
                 NULL);
}

UmiStatus umi_editor_workspace_search_match_initialize(
    UmiEditorWorkspaceSearchMatch *match,
    const UmiEditorWorkspaceSearchDocumentView *document,
    const UmiEditorWorkspaceSearchPatternMatch *pattern_match,
    uint64_t index_revision,
    size_t ordinal_in_document)
{
    size_t start;
    size_t end;
    uint64_t end_line;
    uint64_t end_column;
    int written;
    uint64_t identity_hash;
    if (match == NULL || document == NULL || pattern_match == NULL ||
        document->struct_size != (uint32_t)sizeof(*document) ||
        document->api_version != UMI_EDITOR_WORKSPACE_SEARCH_INDEX_API_VERSION ||
        pattern_match->struct_size != (uint32_t)sizeof(*pattern_match) ||
        pattern_match->api_version !=
            UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_API_VERSION ||
        document->content == NULL ||
        pattern_match->start_byte_offset > SIZE_MAX ||
        pattern_match->end_byte_offset > SIZE_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    start = (size_t)pattern_match->start_byte_offset;
    end = (size_t)pattern_match->end_byte_offset;
    if (start > end || end > document->content_length ||
        pattern_match->capture_count >
            UMI_EDITOR_WORKSPACE_SEARCH_MAX_CAPTURES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(match, 0, sizeof(*match));
    match->struct_size = (uint32_t)sizeof(*match);
    match->api_version = UMI_EDITOR_WORKSPACE_SEARCH_MATCH_API_VERSION;
    if (umi_editor_source_location_initialize(
            &match->location, document->uri, 1U, 1U) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    locate_offset(document->content,
                  document->content_length,
                  start,
                  &match->location.line,
                  &match->location.column);
    locate_offset(document->content,
                  document->content_length,
                  end,
                  &end_line,
                  &end_column);
    match->location.end_line = end_line;
    match->location.end_column = end_column;
    match->location.byte_offset = (uint64_t)start;
    match->location.end_byte_offset = (uint64_t)end;
    match->location.document_revision = document->document_revision;
    match->location.kind = UMI_EDITOR_SOURCE_LOCATION_REFERENCE;
    copy_bounded(match->matched_text,
                 sizeof(match->matched_text),
                 document->content + start,
                 end - start,
                 &match->truncated_text);
    make_line_preview(document->content,
                      document->content_length,
                      start,
                      match->line_preview,
                      sizeof(match->line_preview));
    (void)snprintf(match->location.preview,
                   sizeof(match->location.preview),
                   "%s",
                   match->line_preview);
    (void)snprintf(match->location.label,
                   sizeof(match->location.label),
                   "%.180s:%llu:%llu",
                   document->relative_path,
                   (unsigned long long)match->location.line,
                   (unsigned long long)match->location.column);
    match->capture_count = pattern_match->capture_count;
    if (match->capture_count > 0U) {
        (void)memcpy(match->captures,
                     pattern_match->captures,
                     match->capture_count * sizeof(match->captures[0]));
    }
    match->document_fingerprint = document->fingerprint;
    match->index_revision = index_revision;
    match->execution_steps = pattern_match->execution_steps;
    match->ordinal_in_document = ordinal_in_document;
    match->selected = 1;
    match->read_only = document->read_only;
    match->generated = document->generated;
    identity_hash = hash_text(document->uri);
    written = snprintf(match->id,
                       sizeof(match->id),
                       "search-%016llx-%llu-%llu-%zu",
                       (unsigned long long)identity_hash,
                       (unsigned long long)pattern_match->start_byte_offset,
                       (unsigned long long)pattern_match->end_byte_offset,
                       ordinal_in_document);
    if (written < 0 || (size_t)written >= sizeof(match->id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_search_match_validate(
    const UmiEditorWorkspaceSearchMatch *match)
{
    size_t index;
    if (match == NULL ||
        match->struct_size != (uint32_t)sizeof(*match) ||
        match->api_version != UMI_EDITOR_WORKSPACE_SEARCH_MATCH_API_VERSION ||
        !terminated(match->id, sizeof(match->id)) || match->id[0] == '\0' ||
        !terminated(match->matched_text, sizeof(match->matched_text)) ||
        !terminated(match->line_preview, sizeof(match->line_preview)) ||
        match->capture_count > UMI_EDITOR_WORKSPACE_SEARCH_MAX_CAPTURES ||
        umi_editor_source_location_validate(&match->location) != UMI_STATUS_OK ||
        match->location.end_byte_offset < match->location.byte_offset) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < match->capture_count; ++index) {
        const UmiEditorWorkspaceSearchCapture *capture =
            &match->captures[index];
        if (capture->matched &&
            (capture->end_byte_offset < capture->start_byte_offset ||
             capture->start_byte_offset < match->location.byte_offset ||
             capture->end_byte_offset > match->location.end_byte_offset)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    return UMI_STATUS_OK;
}

int umi_editor_workspace_search_match_compare(
    const UmiEditorWorkspaceSearchMatch *left,
    const UmiEditorWorkspaceSearchMatch *right)
{
    int order;
    if (left == NULL && right == NULL) return 0;
    if (left == NULL) return -1;
    if (right == NULL) return 1;
    order = strcmp(left->location.uri, right->location.uri);
    if (order != 0) return order;
    if (left->location.byte_offset < right->location.byte_offset) return -1;
    if (left->location.byte_offset > right->location.byte_offset) return 1;
    if (left->location.end_byte_offset < right->location.end_byte_offset) {
        return -1;
    }
    if (left->location.end_byte_offset > right->location.end_byte_offset) {
        return 1;
    }
    return strcmp(left->id, right->id);
}

UmiStatus umi_editor_workspace_search_match_format(
    const UmiEditorWorkspaceSearchMatch *match,
    char *out_text,
    size_t out_capacity)
{
    int written;
    if (umi_editor_workspace_search_match_validate(match) != UMI_STATUS_OK ||
        out_text == NULL || out_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(out_text,
                       out_capacity,
                       "%s:%llu:%llu: %s",
                       match->location.uri,
                       (unsigned long long)match->location.line,
                       (unsigned long long)match->location.column,
                       match->line_preview);
    if (written < 0 || (size_t)written >= out_capacity) {
        out_text[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
