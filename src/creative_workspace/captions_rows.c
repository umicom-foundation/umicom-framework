/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/captions_rows.c
 * PURPOSE: Keep subtitle split and merge operations atomic at the immutable track boundary.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "captions_internal.h"
#include <stdlib.h>
#include <string.h>

/* A bounded text copy rejects an oversized cue before reading past the public
 * capacity. UTF-8, blank-line and markup validation remains in the shared
 * cue validator so file imports and row edits accept the same text. */
static UmiStatus CueText(const char *text, char output[UMI_CAPTION_TEXT_CAPACITY])
{
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_CAPTION_TEXT_CAPACITY && text[length] != '\0') ++length;
    if (length == UMI_CAPTION_TEXT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(output, text, length + 1U);
    return UMI_STATUS_OK;
}
UmiStatus UmiCaptionTrackSplit(const UmiCaptionTrack *track, size_t index,
    uint32_t split_ms, const char *first_text, const char *second_text, UmiCaptionTrack **out)
{
    if (track == NULL || out == NULL || *out != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= track->info.cue_count) return UMI_STATUS_NOT_FOUND;
    const UmiCaptionCue *source = &track->cues[index];
    if (split_ms <= source->begin_ms || split_ms >= source->end_ms)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (track->info.cue_count == UMI_CAPTION_CUE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCaptionCue first = *source, second = *source;
    first.end_ms = split_ms;
    second.begin_ms = split_ms;
    UmiStatus status = CueText(first_text, first.text);
    if (status == UMI_STATUS_OK) status = CueText(second_text, second.text);
    if (status != UMI_STATUS_OK) return status;
    if (!UmiCaptionCueValid(&first) || !UmiCaptionCueValid(&second))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index + 1U < track->info.cue_count && split_ms > track->cues[index + 1U].begin_ms)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Work through the existing immutable edits. The temporary owner is never
     * published, so a failed insertion cannot leave the caller with half a split. */
    UmiCaptionTrack *temporary = NULL;
    status = UmiCaptionTrackReplace(track, index, &first, &temporary);
    if (status == UMI_STATUS_OK)
        status = UmiCaptionTrackInsert(temporary, index + 1U, &second, out);
    UmiCaptionTrackDestroy(temporary);
    return status;
}
UmiStatus UmiCaptionTrackMerge(const UmiCaptionTrack *track, size_t index, UmiCaptionTrack **out)
{
    if (track == NULL || out == NULL || *out != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= track->info.cue_count || track->info.cue_count - index < 2U)
        return UMI_STATUS_NOT_FOUND;
    UmiCaptionCue merged = track->cues[index];
    const UmiCaptionCue *following = &track->cues[index + 1U];
    size_t first = strlen(merged.text), second = strlen(following->text);
    if (first + 1U + second >= sizeof(merged.text)) return UMI_STATUS_CAPACITY_EXCEEDED;
    merged.text[first] = '\n';
    memcpy(merged.text + first + 1U, following->text, second + 1U);
    if (following->end_ms > merged.end_ms) merged.end_ms = following->end_ms;

    /* Joining text must be lossless; we never truncate it to fit a cue. Keep
     * the neighbouring cue until the replacement has passed every validation. */
    UmiCaptionTrack *temporary = NULL;
    UmiStatus status = UmiCaptionTrackReplace(track, index, &merged, &temporary);
    if (status == UMI_STATUS_OK) status = UmiCaptionTrackRemove(temporary, index + 1U, out);
    UmiCaptionTrackDestroy(temporary);
    return status;
}
