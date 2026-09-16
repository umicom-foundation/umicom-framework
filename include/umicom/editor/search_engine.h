/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/search_engine.h
 *
 * PURPOSE:
 *   Define deterministic literal search results usable by editors, comparers,
 *   designers, command-line tools and headless tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_SEARCH_ENGINE_H
#define UMICOM_EDITOR_SEARCH_ENGINE_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_SEARCH_MATCH_CAPACITY 4096U

/**
 * List the named editor search case mode values accepted by this public contract.
 */
typedef enum UmiEditorSearchCaseMode {
    UMI_EDITOR_SEARCH_CASE_SENSITIVE = 0,
    UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE = 1,
    UMI_EDITOR_SEARCH_CASE_SMART = 2
} UmiEditorSearchCaseMode;

/**
 * Represent the editor search options data shared with callers of this public contract.
 */
typedef struct UmiEditorSearchOptions {
    UmiEditorSearchCaseMode case_mode;
    int whole_word;
    int allow_overlapping;
    size_t maximum_matches;
} UmiEditorSearchOptions;

/**
 * Represent the editor search match data shared with callers of this public contract.
 */
typedef struct UmiEditorSearchMatch {
    size_t offset;
    size_t byte_count;
} UmiEditorSearchMatch;

/**
 * Represent the editor search results data shared with callers of this public contract.
 */
typedef struct UmiEditorSearchResults {
    size_t count;
    UmiEditorSearchMatch matches[UMI_EDITOR_SEARCH_MATCH_CAPACITY];
    int truncated;
} UmiEditorSearchResults;

/**
 * Provide the editor search literal operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_search_literal(const char *haystack,
                                     size_t haystack_byte_count,
                                     const char *needle,
                                     size_t needle_byte_count,
                                     const UmiEditorSearchOptions *options,
                                     UmiEditorSearchResults *out_results);

/** Find one literal match from a byte offset. Forward searches include start;
 * backward searches select a match strictly before start. When wrap is nonzero,
 * search the opposite end after reaching a boundary. Case/word rules are shared
 * with umi_editor_search_literal; maximum_matches/allow_overlapping do not limit
 * navigation. Outputs are written only on success. Buffers are borrowed. */
UmiStatus UmiEditorSearchNavigate(const char *haystack, size_t haystackBytes,
    const char *needle, size_t needleBytes, const UmiEditorSearchOptions *options,
    size_t start, int backwards, int wrap, UmiEditorSearchMatch *outMatch,
    int *outWrapped);

/** Replace all non-overlapping literal matches into a separate output buffer.
 * Input and output storage must not overlap. Invalid arguments and insufficient
 * capacity leave the output and outCount unchanged. A successful no-match call
 * copies the original text. The output capacity includes its terminating zero.
 * Case and whole_word apply. maximum_matches and allow_overlapping do not limit
 * Replace All: every original, non-overlapping match is considered. */
UmiStatus UmiEditorSearchReplaceAll(const char *text, size_t textBytes,
    const char *needle, size_t needleBytes, const char *replacement,
    size_t replacementBytes, const UmiEditorSearchOptions *options,
    char *outText, size_t capacity, size_t *outCount);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SEARCH_ENGINE_H */
