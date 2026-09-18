/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/search.c
 *
 * PURPOSE:
 *   Implement deterministic text search over indexed files while detecting
 *   binary content, enforcing file/result limits and honouring cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/search.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/filesystem.h"

/* Provide the bytes equal operation used by this module and its client applications. */
static int bytes_equal(unsigned char left,
                       unsigned char right,
                       int case_sensitive)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (!case_sensitive) {
        if (left >= 'A' && left <= 'Z') left = (unsigned char)(left + ('a' - 'A'));
        if (right >= 'A' && right <= 'Z') right = (unsigned char)(right + ('a' - 'A'));
    }
    return left == right;
}

/* Provide the is binary operation used by this module and its client applications. */
static int is_binary(const unsigned char *bytes, size_t length)
{
    size_t index;
    size_t limit = length;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < limit; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (bytes[index] == 0U) return 1;
    }
    return 0;
}

/* Provide the make preview operation used by this module and its client applications. */
static void make_preview(const unsigned char *line_start,
                         size_t line_length,
                         char *out_preview,
                         size_t capacity)
{
    size_t copy_count = line_length;
    size_t index;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (copy_count >= capacity) copy_count = capacity - 1U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < copy_count; ++index) {
        unsigned char value = line_start[index];
        out_preview[index] = value == '\t' ? ' ' : (char)value;
    }
    out_preview[copy_count] = '\0';
}

/*
 * Provide the search request default operation used by this module and its client
 * applications.
 */
UmiSearchRequest umi_search_request_default(const char *query)
{
    UmiSearchRequest request;
    request.query = query;
    request.case_sensitive = 0;
    request.maximum_results = 1000U;
    request.maximum_file_size = 8U * 1024U * 1024U;
    request.cancellation = NULL;
    return request;
}

/* Read in bounded chunks: a file can grow after its index row was recorded.
 * The final probe distinguishes an exactly-full file from a larger one. */
static UmiStatus ReadSearchFile(const char *path, size_t limit,
    const UmiCancellationToken *cancellation, unsigned char **outBytes,
    size_t *outLength)
{
    FILE *file = fopen(path, "rb");
    unsigned char *bytes = NULL;
    size_t length = 0U, capacity = 0U;
    UmiStatus status = UMI_STATUS_OK;
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    while (status == UMI_STATUS_OK) {
        if (umi_cancellation_token_is_requested(cancellation)) {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        if (length == limit) {
            int extra = fgetc(file);
            if (ferror(file)) status = UMI_STATUS_IO_ERROR;
            else if (extra != EOF) status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        size_t amount = limit - length;
        if (amount > 65536U) amount = 65536U;
        if (capacity - length < amount) {
            size_t required = length + amount;
            size_t grown = capacity == 0U ? amount : capacity;
            while (grown < required)
                grown = grown > limit / 2U ? limit : grown * 2U;
            unsigned char *candidate = realloc(bytes, grown);
            if (candidate == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; break; }
            bytes = candidate; capacity = grown;
        }
        size_t received = fread(bytes + length, 1U, amount, file);
        length += received;
        if (received < amount) {
            if (ferror(file)) status = UMI_STATUS_IO_ERROR;
            break;
        }
    }
    if (fclose(file) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    if (status != UMI_STATUS_OK) { free(bytes); return status; }
    *outBytes = bytes; *outLength = length;
    return UMI_STATUS_OK;
}

/* The older API stays available and uses the same checked implementation. */
UmiStatus umi_search_file_index(const UmiFileIndex *index,
    const UmiSearchRequest *request, UmiSearchMatchSink sink,
    void *user_data, UmiSearchStats *out_stats)
{
    return UmiSearchFileIndexWithOptions(index, request, NULL, sink,
        user_data, out_stats);
}

UmiStatus UmiSearchFileIndexWithOptions(const UmiFileIndex *index,
    const UmiSearchRequest *request, const UmiSearchOptions *options,
    UmiSearchMatchSink sink, void *userData, UmiSearchStats *outStats)
{
    UmiSearchStats stats = {0};
    UmiFileIndexPage page;
    UmiFileIndexEntry *entries;
    UmiStatus status;
    size_t queryLength, position = 0U;
    uint64_t revision = options != NULL ? options->expectedRevision : 0U;
    if (outStats != NULL) *outStats = stats;
    if (index == NULL || request == NULL || request->query == NULL ||
        request->query[0] == '\0' || sink == NULL ||
        request->maximum_results == 0U || request->maximum_file_size == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(request->cancellation)) {
        stats.cancelled = 1;
        if (outStats != NULL) *outStats = stats;
        return UMI_STATUS_CANCELLED;
    }
    queryLength = strlen(request->query);
    status = UmiFileIndexReadPage(index, "", 1, 0U, revision, NULL, 0U, &page);
    if (status != UMI_STATUS_OK) return status;
    revision = page.stats.revision;
    entries = calloc(16U, sizeof(*entries));
    if (entries == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (options != NULL && options->progress != NULL)
        options->progress(&stats, options->progressUserData);
    for (;;) {
        if (umi_cancellation_token_is_requested(request->cancellation)) {
            status = UMI_STATUS_CANCELLED; break;
        }
        status = UmiFileIndexReadPage(index, "", 1, position, revision,
            entries, 16U, &page);
        if (status != UMI_STATUS_OK || page.count == 0U) break;
        for (size_t i = 0U; i < page.count; ++i) {
            unsigned char *bytes = NULL;
            size_t length = 0U, line = 1U, lineStart = 0U;
            ++stats.files_considered;
            if (entries[i].size > request->maximum_file_size) {
                ++stats.oversized_files_skipped; continue;
            }
            status = ReadSearchFile(entries[i].path, request->maximum_file_size,
                request->cancellation, &bytes, &length);
            if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
                ++stats.oversized_files_skipped; status = UMI_STATUS_OK; continue;
            }
            if (status != UMI_STATUS_OK) goto finished;
            if (is_binary(bytes, length)) {
                ++stats.binary_files_skipped; free(bytes); continue;
            }
            ++stats.files_searched;
            /* Document loading removes the UTF-8 BOM, so source columns begin
             * after it on the first line. UTF-16 remains a binary skip here. */
            size_t start = length >= 3U && bytes[0] == 0xefU && bytes[1] == 0xbbU &&
                bytes[2] == 0xbfU ? 3U : 0U;
            lineStart = start;
            /* Subtraction avoids overflow in offset + queryLength. */
            for (size_t offset = start; queryLength <= length &&
                    offset <= length - queryLength; ++offset) {
                if ((offset % 1024U) == 0U &&
                    umi_cancellation_token_is_requested(request->cancellation)) {
                    status = UMI_STATUS_CANCELLED; break;
                }
                size_t matched = 0U;
                while (matched < queryLength && bytes_equal(bytes[offset + matched],
                    (unsigned char)request->query[matched], request->case_sensitive)) {
                    ++matched;
                    if ((matched % 1024U) == 0U &&
                        umi_cancellation_token_is_requested(request->cancellation)) {
                        status = UMI_STATUS_CANCELLED; break;
                    }
                }
                if (status != UMI_STATUS_OK) break;
                if (matched == queryLength) {
                    /* Look for one further match before claiming truncation. */
                    if (stats.matches == request->maximum_results) {
                        stats.truncated = 1; break;
                    }
                    UmiSearchMatch match;
                    size_t lineEnd = offset;
                    while (lineEnd < length && bytes[lineEnd] != '\n' &&
                        bytes[lineEnd] != '\r') ++lineEnd;
                    memset(&match, 0, sizeof match);
                    memcpy(match.path, entries[i].path, sizeof match.path);
                    match.line = line; match.column = offset - lineStart + 1U;
                    make_preview(bytes + lineStart, lineEnd - lineStart,
                        match.preview, sizeof match.preview);
                    status = sink(&match, userData);
                    if (status != UMI_STATUS_OK) break;
                    ++stats.matches;
                }
                /* Count a newline after matching at it, never before: this also
                 * gives a literal newline query a valid byte column. */
                if (bytes[offset] == '\n') { ++line; lineStart = offset + 1U; }
            }
            free(bytes);
            if (options != NULL && options->progress != NULL)
                options->progress(&stats, options->progressUserData);
            if (status != UMI_STATUS_OK || stats.truncated) goto finished;
        }
        position += page.count;
        if (!page.has_more) break;
    }
finished:
    if (status == UMI_STATUS_OK &&
        umi_cancellation_token_is_requested(request->cancellation))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) {
        UmiFileIndexPage finalPage;
        status = UmiFileIndexReadPage(index, "", 1, 0U, revision,
            NULL, 0U, &finalPage);
    }
    stats.cancelled = status == UMI_STATUS_CANCELLED;
    if (options != NULL && options->progress != NULL)
        options->progress(&stats, options->progressUserData);
    if (outStats != NULL) *outStats = stats;
    free(entries);
    return status;
}

/* Shared location checking keeps stale saved-file coordinates from moving an
 * editor cursor to unrelated text after an unsaved edit. */
UmiStatus UmiSearchMatchLocate(const UmiSearchMatch *match, const char *query,
    int caseSensitive, const char *text, size_t length, size_t *outOffset)
{
    size_t line = 1U, start = 0U, end, offset, queryLength;
    if (match == NULL || query == NULL || query[0] == '\0' || text == NULL ||
        outOffset == NULL || match->line == 0U || match->column == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    while (start < length && line < match->line) {
        if (text[start++] == '\n') ++line;
    }
    if (line != match->line) return UMI_STATUS_NOT_FOUND;
    end = start;
    while (end < length && text[end] != '\n' && text[end] != '\r') ++end;
    if (match->column - 1U > end - start) return UMI_STATUS_NOT_FOUND;
    offset = start + match->column - 1U;
    queryLength = strlen(query);
    if (queryLength > end - offset) return UMI_STATUS_NOT_FOUND;
    for (size_t i = 0U; i < queryLength; ++i)
        if (!bytes_equal((unsigned char)text[offset + i],
                (unsigned char)query[i], caseSensitive)) return UMI_STATUS_NOT_FOUND;
    *outOffset = offset;
    return UMI_STATUS_OK;
}
