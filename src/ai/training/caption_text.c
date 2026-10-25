/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/caption_text.c
 * PURPOSE:
 *   Reuse the existing immutable subtitle/caption track as the only parser
 *   and cue owner when building a reviewable UTF-8 training-text candidate.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/caption_text.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

UmiStatus UmiAiTrainingCaptionTextExtract(
    const UmiCaptionTrack *track,
    char *outText, size_t capacity, size_t *outBytes)
{
    UmiCaptionInfo info = {0};
    size_t needed = 0U;
    size_t cursor = 0U;
    char *staged = NULL;
    UmiStatus status;

    if (track == NULL || outText == NULL || outBytes == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiCaptionTrackInspect(track, &info);
    if (status != UMI_STATUS_OK) return status;
    if (info.cue_count == 0U || info.cue_count > UMI_CAPTION_CUE_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Pass one: calculate the exact destination size without writing output.
     * The caption owner guarantees bounded, terminated and validated cues. */
    for (size_t index = 0U; index < info.cue_count; ++index) {
        UmiCaptionCue cue = {0};
        status = UmiCaptionTrackCue(track, index, &cue);
        if (status != UMI_STATUS_OK) return status;
        size_t length = strlen(cue.text);
        if (length == 0U || length > UMI_CAPTION_TEXT_CAPACITY - 1U ||
            needed > SIZE_MAX - length - 1U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        needed += length + 1U; /* Every cue ends in LF. */
    }
    if (needed >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* The track is immutable, but staging still guarantees that a failed
     * second-pass read or allocation never writes a partial caller buffer. */
    staged = malloc(needed + 1U);
    if (staged == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t index = 0U; index < info.cue_count; ++index) {
        UmiCaptionCue cue = {0};
        status = UmiCaptionTrackCue(track, index, &cue);
        if (status != UMI_STATUS_OK) {
            free(staged);
            return status;
        }
        size_t length = strlen(cue.text);
        if (length == 0U || length > needed - cursor - 1U) {
            free(staged);
            return UMI_STATUS_INVALID_STATE;
        }
        (void)memcpy(staged + cursor, cue.text, length);
        cursor += length;
        staged[cursor++] = '\n';
    }
    staged[cursor] = '\0';
    (void)memcpy(outText, staged, cursor + 1U);
    *outBytes = cursor;
    free(staged);
    return UMI_STATUS_OK;
}
