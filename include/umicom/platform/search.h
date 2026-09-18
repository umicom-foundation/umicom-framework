/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/search.h
 *
 * PURPOSE:
 *   Define cancellable text search across a Framework file index with bounded
 *   results, line/column locations and source previews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_SEARCH_H
#define UMICOM_PLATFORM_SEARCH_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"
#include "umicom/platform/file_index.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SEARCH_PREVIEW_CAPACITY 320U

/**
 * Represent the search request data shared with callers of this public contract.
 */
typedef struct UmiSearchRequest {
    const char *query;
    int case_sensitive;
    size_t maximum_results;
    size_t maximum_file_size;
    const UmiCancellationToken *cancellation;
} UmiSearchRequest;

/**
 * Represent the search match data shared with callers of this public contract.
 */
typedef struct UmiSearchMatch {
    char path[UMI_PATH_CAPACITY];
    size_t line;
    size_t column;
    char preview[UMI_SEARCH_PREVIEW_CAPACITY];
} UmiSearchMatch;

/**
 * Represent the search stats data shared with callers of this public contract.
 */
typedef struct UmiSearchStats {
    size_t files_considered;
    size_t files_searched;
    size_t binary_files_skipped;
    size_t oversized_files_skipped;
    size_t matches;
    int truncated;
    int cancelled;
} UmiSearchStats;

typedef UmiStatus (*UmiSearchMatchSink)(const UmiSearchMatch *match,
                                        void *user_data);

/**
 * Provide the search request default operation used by this module and its client
 * applications.
 */
UmiSearchRequest umi_search_request_default(const char *query);
/**
 * Provide the search file index operation used by this module and its client applications.
 */
UmiStatus umi_search_file_index(const UmiFileIndex *index,
                                const UmiSearchRequest *request,
                                UmiSearchMatchSink sink,
                                void *user_data,
                                UmiSearchStats *out_stats);

/** Optional controls for a coherent indexed search. expectedRevision zero
 * captures the current index revision. Progress runs on the searching thread;
 * callbacks must not destroy the index or retained cancellation token. */
typedef void (*UmiSearchProgressSink)(const UmiSearchStats *stats, void *userData);
typedef struct UmiSearchOptions {
    uint64_t expectedRevision;
    UmiSearchProgressSink progress;
    void *progressUserData;
} UmiSearchOptions;

/** Search saved bytes, using revision-checked index pages. Text matching is
 * literal, with ASCII-only case folding when case_sensitive is zero. Columns
 * are one-based byte positions. NUL-containing files are skipped. Limits must
 * be nonzero; actual reads, not only cached sizes, enforce maximum_file_size.
 * A changed index returns BUSY; unreadable files return IO_ERROR. A streaming
 * sink may have received earlier matches before any failure: discard them when
 * a complete result is required. No file or editor draft is modified. */
UmiStatus UmiSearchFileIndexWithOptions(const UmiFileIndex *index,
    const UmiSearchRequest *request, const UmiSearchOptions *options,
    UmiSearchMatchSink sink, void *userData, UmiSearchStats *outStats);

/** Verify that a saved-file match still names the same literal bytes at its
 * reported location in visible UTF-8 text. NOT_FOUND means the text moved or
 * changed; no output is written on failure. Does not search another occurrence. */
UmiStatus UmiSearchMatchLocate(const UmiSearchMatch *match, const char *query,
    int caseSensitive, const char *text, size_t length, size_t *outOffset);

#ifdef __cplusplus
}
#endif

#endif
