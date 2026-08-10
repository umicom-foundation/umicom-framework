/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_terminal_transcript.c
 *
 * PURPOSE:
 *   Verify bounded terminal transcript ordering and clearing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/terminal/transcript.h"

int main(void)
{
    UmiTerminalTranscript *transcript = NULL;
    UmiTerminalTranscriptLine line;
    assert(umi_terminal_transcript_create(2U, &transcript) == UMI_STATUS_OK);
    assert(umi_terminal_transcript_append(transcript, 1U, UMI_TERMINAL_STREAM_INPUT, "one") == UMI_STATUS_OK);
    assert(umi_terminal_transcript_append(transcript, 2U, UMI_TERMINAL_STREAM_OUTPUT, "two") == UMI_STATUS_OK);
    assert(umi_terminal_transcript_append(transcript, 3U, UMI_TERMINAL_STREAM_ERROR, "three") == UMI_STATUS_OK);
    assert(umi_terminal_transcript_count(transcript) == 2U);
    assert(umi_terminal_transcript_at(transcript, 0U, &line) == UMI_STATUS_OK);
    assert(strcmp(line.text, "two") == 0);
    umi_terminal_transcript_clear(transcript);
    assert(umi_terminal_transcript_count(transcript) == 0U);
    umi_terminal_transcript_destroy(transcript);
    return 0;
}
