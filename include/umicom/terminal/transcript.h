/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/transcript.h
 *
 * PURPOSE:
 *   Retain bounded, timestamped terminal input, output, error and system records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TERMINAL_TRANSCRIPT_H
#define UMICOM_TERMINAL_TRANSCRIPT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/terminal/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTerminalTranscriptLine {
    uint64_t sequence;
    uint64_t timestamp_ns;
    UmiTerminalStream stream;
    char text[UMI_TERMINAL_LINE_CAPACITY];
} UmiTerminalTranscriptLine;

typedef struct UmiTerminalTranscript UmiTerminalTranscript;

UmiStatus umi_terminal_transcript_create(size_t capacity,
                                         UmiTerminalTranscript **out_transcript);
void umi_terminal_transcript_destroy(UmiTerminalTranscript *transcript);
UmiStatus umi_terminal_transcript_append(UmiTerminalTranscript *transcript,
                                         uint64_t timestamp_ns,
                                         UmiTerminalStream stream,
                                         const char *text);
size_t umi_terminal_transcript_count(const UmiTerminalTranscript *transcript);
UmiStatus umi_terminal_transcript_at(const UmiTerminalTranscript *transcript,
                                     size_t index,
                                     UmiTerminalTranscriptLine *out_line);
void umi_terminal_transcript_clear(UmiTerminalTranscript *transcript);

#ifdef __cplusplus
}
#endif

#endif
