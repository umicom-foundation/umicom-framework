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

#ifdef __cplusplus
}
#endif

#endif
