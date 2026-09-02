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
        left = (unsigned char)tolower(left);
        right = (unsigned char)tolower(right);
    }
    return left == right;
}

/* Provide the is binary operation used by this module and its client applications. */
static int is_binary(const unsigned char *bytes, size_t length)
{
    size_t index;
    size_t limit = length < 4096U ? length : 4096U;
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

/* Provide the search file index operation used by this module and its client applications. */
UmiStatus umi_search_file_index(const UmiFileIndex *index,
                                const UmiSearchRequest *request,
                                UmiSearchMatchSink sink,
                                void *user_data,
                                UmiSearchStats *out_stats)
{
    UmiSearchStats stats;
    UmiFileIndexStats index_stats;
    size_t file_position;
    size_t query_length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || request == NULL || request->query == NULL ||
        request->query[0] == '\0' || sink == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&stats, 0, sizeof(stats));
    query_length = strlen(request->query);
    index_stats = umi_file_index_stats(index);
    /* Visit each bounded item once so every record receives the same rule. */
    for (file_position = 0U;
         file_position < index_stats.files;
         ++file_position) {
        UmiFileIndexEntry entry;
        unsigned char *bytes = NULL;
        size_t length = 0U;
        size_t offset;
        size_t line = 1U;
        size_t line_start = 0U;
        UmiStatus status;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (request->cancellation != NULL &&
            umi_cancellation_token_is_requested(request->cancellation)) {
            stats.cancelled = 1;
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (out_stats != NULL) *out_stats = stats;
            return UMI_STATUS_CANCELLED;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_file_index_at(index, file_position, &entry) != UMI_STATUS_OK) {
            continue;
        }
        stats.files_considered += 1U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (entry.size > request->maximum_file_size) {
            stats.oversized_files_skipped += 1U;
            continue;
        }
        status = umi_fs_read_bytes(entry.path, &bytes, &length);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (is_binary(bytes, length)) {
            stats.binary_files_skipped += 1U;
            umi_fs_free_bytes(bytes);
            continue;
        }
        stats.files_searched += 1U;
        offset = 0U;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (offset + query_length <= length) {
            size_t index_in_query;
            int match = 1;
            /* Apply this branch only when its contract condition is satisfied. */
            if (bytes[offset] == '\n') {
                line += 1U;
                line_start = offset + 1U;
            }
            /* Visit each bounded item once so every record receives the same rule. */
            for (index_in_query = 0U;
                 index_in_query < query_length;
                 ++index_in_query) {
                /* Use the stable identifier comparison to choose the matching record or policy. */
                if (!bytes_equal(bytes[offset + index_in_query],
                                 (unsigned char)request->query[index_in_query],
                                 request->case_sensitive)) {
                    match = 0;
                    break;
                }
            }
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (match) {
                UmiSearchMatch result;
                size_t line_end = offset;
                /*
                 * Continue only while work remains available; the loop body advances the state on each
                 * pass.
                 */
                while (line_end < length &&
                       bytes[line_end] != '\n' &&
                       bytes[line_end] != '\r') {
                    ++line_end;
                }
                (void)memset(&result, 0, sizeof(result));
                (void)snprintf(result.path,
                               sizeof(result.path),
                               "%s",
                               entry.path);
                result.line = line;
                result.column = offset - line_start + 1U;
                make_preview(bytes + line_start,
                             line_end - line_start,
                             result.preview,
                             sizeof(result.preview));
                status = sink(&result, user_data);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) {
                    umi_fs_free_bytes(bytes);
                    /*
                     * Protect caller-owned memory by checking that required state is available before it is
                     * used.
                     */
                    if (out_stats != NULL) *out_stats = stats;
                    return status;
                }
                stats.matches += 1U;
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (stats.matches >= request->maximum_results) {
                    stats.truncated = 1;
                    umi_fs_free_bytes(bytes);
                    /*
                     * Protect caller-owned memory by checking that required state is available before it is
                     * used.
                     */
                    if (out_stats != NULL) *out_stats = stats;
                    return UMI_STATUS_OK;
                }
            }
            ++offset;
        }
        umi_fs_free_bytes(bytes);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_stats != NULL) *out_stats = stats;
    return UMI_STATUS_OK;
}
