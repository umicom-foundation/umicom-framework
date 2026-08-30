/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/search.h
 *
 * PURPOSE:
 *   Search retained terminal transcripts with stream, case and whole-word
 *   filtering while returning stable sequence-based matches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_SEARCH_H
#define UMICOM_TERMINAL_SEARCH_H

#include "umicom/terminal/transcript.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TERMINAL_SEARCH_MAX_RESULTS 512U

typedef struct UmiTerminalSearchQuery {
    char text[UMI_TERMINAL_QUERY_CAPACITY];
    int case_sensitive;
    int whole_word;
    int use_stream_filter;
    UmiTerminalStream stream;
    uint64_t after_sequence;
    size_t limit;
} UmiTerminalSearchQuery;

typedef struct UmiTerminalSearchMatch {
    uint64_t sequence;
    size_t line_index;
    size_t byte_offset;
    size_t byte_length;
    UmiTerminalStream stream;
    char preview[UMI_TERMINAL_LINE_CAPACITY];
} UmiTerminalSearchMatch;

typedef struct UmiTerminalSearchResult {
    UmiTerminalSearchMatch matches[UMI_TERMINAL_SEARCH_MAX_RESULTS];
    size_t count;
    size_t total_matches;
    int truncated;
} UmiTerminalSearchResult;

void umi_terminal_search_query_init(UmiTerminalSearchQuery *query);
UmiStatus umi_terminal_search_transcript(
    const UmiTerminalTranscript *transcript,
    const UmiTerminalSearchQuery *query,
    UmiTerminalSearchResult *out_result);

#ifdef __cplusplus
}
#endif
#endif
