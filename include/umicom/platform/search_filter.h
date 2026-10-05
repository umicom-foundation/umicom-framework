/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/search_filter.h
 * PURPOSE: Limit indexed text search using copied include and exclude path patterns.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_SEARCH_FILTER_H
#define UMICOM_PLATFORM_SEARCH_FILTER_H
#include "umicom/platform/search.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SEARCH_FILTER_TEXT_CAPACITY 512U
#define UMI_SEARCH_FILTER_MAX_PATTERNS 16U

/* Each field is a semicolon-separated list, with surrounding spaces trimmed.
 * Empty include means all indexed files; empty exclude means no exclusions.
 * A pattern without a slash matches the basename at any depth. A pattern with
 * a slash matches the entire workspace-relative path. '*' matches zero or
 * more bytes (including slashes); '?' matches one byte. Matching is case
 * sensitive, independent of the text search's Match case option. Backslashes
 * become slashes. Brackets, braces, absolute paths, control characters and
 * empty list items are rejected. There is no regex, escaping or negation.
 * These filters narrow index results; they are not an access-control boundary. */
typedef struct UmiSearchPathFilter {
    char include_patterns[UMI_SEARCH_FILTER_TEXT_CAPACITY];
    char exclude_patterns[UMI_SEARCH_FILTER_TEXT_CAPACITY];
} UmiSearchPathFilter;

/* Copy and validate both lists before publishing output. NULL inputs are
 * invalid; use "" for an empty list. Output can alias either input field and
 * remains unchanged on failure. Each list permits at most 16 patterns. */
UmiStatus UmiSearchPathFilterInit(const char *include_patterns,
    const char *exclude_patterns, UmiSearchPathFilter *out_filter);

/* Match a nonempty workspace-relative path of fewer than UMI_PATH_CAPACITY
 * bytes. Exclude wins over Include. Output changes only on success. Neither
 * file content nor the index is read, and no allocation or recursion occurs. */
UmiStatus UmiSearchPathFilterMatch(const UmiSearchPathFilter *filter,
    const char *relative_path, int *out_matches);

/* Apply a copied filter before opening each candidate file. NULL preserves
 * unfiltered behavior. files_considered includes filtered-out entries; skipped
 * filter entries do not count as searched, binary or oversized. The existing
 * revision, cancellation, bounded-read and streaming-sink rules still apply.
 * A malformed filter returns without reading files or modifying out_stats. */
UmiStatus UmiSearchFileIndexScoped(const UmiFileIndex *index,
    const UmiSearchRequest *request, const UmiSearchOptions *options,
    const UmiSearchPathFilter *filter, UmiSearchMatchSink sink,
    void *user_data, UmiSearchStats *out_stats);
#ifdef __cplusplus
}
#endif
#endif
