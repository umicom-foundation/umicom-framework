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

/**
 * Represent the terminal transcript line data shared with callers of this public contract.
 */
typedef struct UmiTerminalTranscriptLine {
    uint64_t sequence;
    uint64_t timestamp_ns;
    UmiTerminalStream stream;
    char text[UMI_TERMINAL_LINE_CAPACITY];
} UmiTerminalTranscriptLine;

/**
 * Represent the terminal transcript data shared with callers of this public contract.
 */
typedef struct UmiTerminalTranscript UmiTerminalTranscript;

/**
 * Initialise terminal transcript from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_terminal_transcript_create(size_t capacity,
                                         UmiTerminalTranscript **out_transcript);
/**
 * Release or reset state held by terminal transcript so the same storage can be reused
 * safely.
 */
void umi_terminal_transcript_destroy(UmiTerminalTranscript *transcript);
/**
 * Add terminal transcript only after its inputs and available capacity have been checked.
 */
UmiStatus umi_terminal_transcript_append(UmiTerminalTranscript *transcript,
                                         uint64_t timestamp_ns,
                                         UmiTerminalStream stream,
                                         const char *text);
/**
 * Return the number of records represented by terminal transcript without changing their
 * state.
 */
size_t umi_terminal_transcript_count(const UmiTerminalTranscript *transcript);
/**
 * Find terminal transcript while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_terminal_transcript_at(const UmiTerminalTranscript *transcript,
                                     size_t index,
                                     UmiTerminalTranscriptLine *out_line);
/**
 * Release or reset state held by terminal transcript so the same storage can be reused
 * safely.
 */
void umi_terminal_transcript_clear(UmiTerminalTranscript *transcript);

#ifdef __cplusplus
}
#endif

#endif
