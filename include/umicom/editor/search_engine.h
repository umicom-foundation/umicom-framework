/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/search_engine.h
 *
 * PURPOSE:
 *   Define deterministic literal search results usable by editors, comparers,
 *   designers, command-line tools and headless tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_SEARCH_ENGINE_H
#define UMICOM_EDITOR_SEARCH_ENGINE_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_SEARCH_MATCH_CAPACITY 4096U

typedef enum UmiEditorSearchCaseMode {
    UMI_EDITOR_SEARCH_CASE_SENSITIVE = 0,
    UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE = 1,
    UMI_EDITOR_SEARCH_CASE_SMART = 2
} UmiEditorSearchCaseMode;

typedef struct UmiEditorSearchOptions {
    UmiEditorSearchCaseMode case_mode;
    int whole_word;
    int allow_overlapping;
    size_t maximum_matches;
} UmiEditorSearchOptions;

typedef struct UmiEditorSearchMatch {
    size_t offset;
    size_t byte_count;
} UmiEditorSearchMatch;

typedef struct UmiEditorSearchResults {
    size_t count;
    UmiEditorSearchMatch matches[UMI_EDITOR_SEARCH_MATCH_CAPACITY];
    int truncated;
} UmiEditorSearchResults;

UmiStatus umi_editor_search_literal(const char *haystack,
                                     size_t haystack_byte_count,
                                     const char *needle,
                                     size_t needle_byte_count,
                                     const UmiEditorSearchOptions *options,
                                     UmiEditorSearchResults *out_results);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SEARCH_ENGINE_H */
