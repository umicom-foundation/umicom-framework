/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/transcript.c
 *
 * PURPOSE:
 *   Implement a thread-safe chronological terminal transcript ring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/terminal/transcript.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"

struct UmiTerminalTranscript {
    UmiTerminalTranscriptLine *lines;
    size_t capacity;
    size_t count;
    size_t head;
    uint64_t next_sequence;
    UmiMutex *mutex;
};

UmiStatus umi_terminal_transcript_create(size_t capacity,
                                         UmiTerminalTranscript **out_transcript)
{
    UmiTerminalTranscript *transcript;
    if (out_transcript == NULL || capacity == 0U ||
        capacity > UMI_TERMINAL_TRANSCRIPT_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_transcript = NULL;
    transcript = (UmiTerminalTranscript *)calloc(1U, sizeof(*transcript));
    if (transcript == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    transcript->lines = (UmiTerminalTranscriptLine *)calloc(
        capacity,
        sizeof(*transcript->lines)
    );
    if (transcript->lines == NULL ||
        umi_mutex_create(&transcript->mutex) != UMI_STATUS_OK) {
        umi_terminal_transcript_destroy(transcript);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    transcript->capacity = capacity;
    transcript->next_sequence = 1U;
    *out_transcript = transcript;
    return UMI_STATUS_OK;
}

void umi_terminal_transcript_destroy(UmiTerminalTranscript *transcript)
{
    if (transcript != NULL) {
        umi_mutex_destroy(transcript->mutex);
        free(transcript->lines);
        free(transcript);
    }
}

UmiStatus umi_terminal_transcript_append(UmiTerminalTranscript *transcript,
                                         uint64_t timestamp_ns,
                                         UmiTerminalStream stream,
                                         const char *text)
{
    size_t position;
    UmiTerminalTranscriptLine *line;
    if (transcript == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(transcript->mutex);
    position = (transcript->head + transcript->count) %
               transcript->capacity;
    if (transcript->count == transcript->capacity) {
        position = transcript->head;
        transcript->head = (transcript->head + 1U) %
                           transcript->capacity;
    } else {
        transcript->count += 1U;
    }
    line = &transcript->lines[position];
    (void)memset(line, 0, sizeof(*line));
    line->sequence = transcript->next_sequence++;
    line->timestamp_ns = timestamp_ns;
    line->stream = stream;
    (void)snprintf(line->text, sizeof(line->text), "%s", text);
    (void)umi_mutex_unlock(transcript->mutex);
    return UMI_STATUS_OK;
}

size_t umi_terminal_transcript_count(const UmiTerminalTranscript *transcript)
{
    size_t count;
    if (transcript == NULL) {
        return 0U;
    }
    (void)umi_mutex_lock(transcript->mutex);
    count = transcript->count;
    (void)umi_mutex_unlock(transcript->mutex);
    return count;
}

UmiStatus umi_terminal_transcript_at(const UmiTerminalTranscript *transcript,
                                     size_t index,
                                     UmiTerminalTranscriptLine *out_line)
{
    size_t position;
    if (transcript == NULL || out_line == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(transcript->mutex);
    if (index >= transcript->count) {
        (void)umi_mutex_unlock(transcript->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    position = (transcript->head + index) % transcript->capacity;
    *out_line = transcript->lines[position];
    (void)umi_mutex_unlock(transcript->mutex);
    return UMI_STATUS_OK;
}

void umi_terminal_transcript_clear(UmiTerminalTranscript *transcript)
{
    if (transcript == NULL) {
        return;
    }
    (void)umi_mutex_lock(transcript->mutex);
    transcript->count = 0U;
    transcript->head = 0U;
    (void)memset(transcript->lines,
                 0,
                 transcript->capacity * sizeof(transcript->lines[0]));
    (void)umi_mutex_unlock(transcript->mutex);
}
