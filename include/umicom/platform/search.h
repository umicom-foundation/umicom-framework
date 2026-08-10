/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/search.h
 *
 * PURPOSE:
 *   Define cancellable text search across a Framework file index with bounded
 *   results, line/column locations and source previews.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiSearchRequest {
    const char *query;
    int case_sensitive;
    size_t maximum_results;
    size_t maximum_file_size;
    const UmiCancellationToken *cancellation;
} UmiSearchRequest;

typedef struct UmiSearchMatch {
    char path[UMI_PATH_CAPACITY];
    size_t line;
    size_t column;
    char preview[UMI_SEARCH_PREVIEW_CAPACITY];
} UmiSearchMatch;

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

UmiSearchRequest umi_search_request_default(const char *query);
UmiStatus umi_search_file_index(const UmiFileIndex *index,
                                const UmiSearchRequest *request,
                                UmiSearchMatchSink sink,
                                void *user_data,
                                UmiSearchStats *out_stats);

#ifdef __cplusplus
}
#endif

#endif
