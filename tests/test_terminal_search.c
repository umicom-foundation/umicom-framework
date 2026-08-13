/*-----------------------------------------------------------------------------
 * Umicom Framework terminal transcript-search tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/terminal/search.h"

int main(void)
{
    UmiTerminalTranscript *transcript = NULL;
    UmiTerminalSearchQuery query;
    UmiTerminalSearchResult result;
    assert(umi_terminal_transcript_create(8U, &transcript) == UMI_STATUS_OK);
    assert(umi_terminal_transcript_append(transcript, 1U,
        UMI_TERMINAL_STREAM_OUTPUT, "Build completed") == UMI_STATUS_OK);
    assert(umi_terminal_transcript_append(transcript, 2U,
        UMI_TERMINAL_STREAM_ERROR, "build failed") == UMI_STATUS_OK);
    umi_terminal_search_query_init(&query);
    (void)snprintf(query.text, sizeof(query.text), "%s", "build");
    assert(umi_terminal_search_transcript(transcript, &query, &result) ==
           UMI_STATUS_OK);
    assert(result.count == 2U && result.total_matches == 2U);
    query.case_sensitive = 1;
    assert(umi_terminal_search_transcript(transcript, &query, &result) ==
           UMI_STATUS_OK);
    assert(result.count == 1U);
    query.case_sensitive = 0;
    query.use_stream_filter = 1;
    query.stream = UMI_TERMINAL_STREAM_ERROR;
    assert(umi_terminal_search_transcript(transcript, &query, &result) ==
           UMI_STATUS_OK);
    assert(result.count == 1U && result.matches[0].stream == UMI_TERMINAL_STREAM_ERROR);
    umi_terminal_transcript_destroy(transcript);
    return 0;
}
