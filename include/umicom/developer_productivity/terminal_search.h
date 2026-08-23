/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/terminal_search.h
 *
 * PURPOSE:
 *   Search retained terminal transcripts without depending on a terminal UI.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_TERMINAL_SEARCH_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_TERMINAL_SEARCH_H

#include "umicom/terminal/transcript.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_TERMINAL_SEARCH_CAPACITY 256U

typedef struct UmiDeveloperTerminalSearchMatch {
    size_t transcript_index;
    uint64_t sequence;
    char text[UMI_TERMINAL_LINE_CAPACITY];
} UmiDeveloperTerminalSearchMatch;

UmiStatus umi_developer_terminal_search(
    const UmiTerminalTranscript *transcript,
    const char *query,
    int case_sensitive,
    UmiDeveloperTerminalSearchMatch *out_matches,
    size_t capacity,
    size_t *out_count);

#ifdef __cplusplus
}
#endif

#endif
