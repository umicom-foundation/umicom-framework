/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/captions_edit.c
 * PURPOSE: Apply checked caption timing and row edits while preserving source documents.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "captions_internal.h"
#include <stdlib.h>
#include <string.h>
/* Every edit receives a fresh owner. Callers can retain the previous owner as
 * undo history without borrowing pointers into a mutable editor buffer. */
static UmiStatus Copy(const UmiCaptionTrack *track, UmiCaptionTrack **out)
{
    if (!track || !out || *out) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCaptionTrack *copy = malloc(sizeof(*copy));
    if (!copy) return UMI_STATUS_OUT_OF_MEMORY;
    *copy = *track;
    *out = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiCaptionTrackShift(const UmiCaptionTrack *track, int64_t offset_ms,
                             UmiCaptionTrack **out)
{
    if (!track || !out || *out) return UMI_STATUS_INVALID_ARGUMENT;
    /* Check the offset itself before addition: negating INT64_MIN or adding
     * it to a timestamp must never be the way an invalid request is detected. */
    if (offset_ms < -(int64_t)UMI_CAPTION_TIME_LIMIT_MS ||
        offset_ms > (int64_t)UMI_CAPTION_TIME_LIMIT_MS)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0; i < track->info.cue_count; ++i)
        if ((int64_t)track->cues[i].begin_ms + offset_ms < 0 ||
            (int64_t)track->cues[i].end_ms + offset_ms > UMI_CAPTION_TIME_LIMIT_MS)
            return UMI_STATUS_INVALID_ARGUMENT;
    UmiCaptionTrack *copy = NULL;
    UmiStatus status = Copy(track, &copy);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0; i < copy->info.cue_count; ++i)
    {
        copy->cues[i].begin_ms = (uint32_t)((int64_t)copy->cues[i].begin_ms + offset_ms);
        copy->cues[i].end_ms = (uint32_t)((int64_t)copy->cues[i].end_ms + offset_ms);
    }
    UmiCaptionTrackMeasure(copy);
    *out = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiCaptionTrackClip(const UmiCaptionTrack *track, uint32_t begin_ms,
                            uint32_t end_ms, bool rebase, UmiCaptionTrack **out)
{
    if (!track || !out || *out || begin_ms >= end_ms || end_ms > UMI_CAPTION_TIME_LIMIT_MS)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCaptionTrack *copy = NULL;
    UmiStatus status = UmiCaptionTrackAllocate(track->info.title, &copy);
    if (status != UMI_STATUS_OK) return status;
    /* Half-open intersections agree with the audio arrangement timeline.
     * Rebase is explicit so callers can either make a short clip or keep the
     * original timeline when exporting only a selected portion. */
    for (size_t i = 0; i < track->info.cue_count; ++i)
    {
        const UmiCaptionCue *cue = &track->cues[i];
        if (cue->end_ms <= begin_ms || cue->begin_ms >= end_ms) continue;
        UmiCaptionCue *next = &copy->cues[copy->info.cue_count++];
        *next = *cue;
        next->begin_ms = cue->begin_ms < begin_ms ? begin_ms : cue->begin_ms;
        next->end_ms = cue->end_ms > end_ms ? end_ms : cue->end_ms;
        if (rebase) { next->begin_ms -= begin_ms; next->end_ms -= begin_ms; }
    }
    UmiCaptionTrackMeasure(copy);
    *out = copy;
    return UMI_STATUS_OK;
}
/* Validate ordering after a row edit without sorting the user's document.
 * Silent sorting would change row identity and could move an overlapping cue. */
static UmiStatus Publish(UmiCaptionTrack *copy, UmiCaptionTrack **out)
{
    for (size_t i = 1; i < copy->info.cue_count; ++i)
        if (copy->cues[i].begin_ms < copy->cues[i-1U].begin_ms)
        { free(copy); return UMI_STATUS_INVALID_ARGUMENT; }
    UmiCaptionTrackMeasure(copy);
    *out = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiCaptionTrackReplace(const UmiCaptionTrack *track, size_t index,
                               const UmiCaptionCue *cue, UmiCaptionTrack **out)
{
    if (!track || !out || *out || !UmiCaptionCueValid(cue)) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= track->info.cue_count) return UMI_STATUS_NOT_FOUND;
    UmiCaptionTrack *copy = NULL;
    UmiStatus status = Copy(track, &copy);
    if (status != UMI_STATUS_OK) return status;
    copy->cues[index] = *cue;
    return Publish(copy, out);
}
UmiStatus UmiCaptionTrackInsert(const UmiCaptionTrack *track, size_t index,
                              const UmiCaptionCue *cue, UmiCaptionTrack **out)
{
    if (!track || !out || *out || !UmiCaptionCueValid(cue)) return UMI_STATUS_INVALID_ARGUMENT;
    if (index > track->info.cue_count) return UMI_STATUS_NOT_FOUND;
    if (track->info.cue_count == UMI_CAPTION_CUE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCaptionTrack *copy = NULL;
    UmiStatus status = Copy(track, &copy);
    if (status != UMI_STATUS_OK) return status;
    memmove(copy->cues + index + 1U, copy->cues + index,
            (copy->info.cue_count - index) * sizeof(*cue));
    copy->cues[index] = *cue;
    ++copy->info.cue_count;
    return Publish(copy, out);
}
UmiStatus UmiCaptionTrackRemove(const UmiCaptionTrack *track, size_t index, UmiCaptionTrack **out)
{
    if (!track || !out || *out) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= track->info.cue_count) return UMI_STATUS_NOT_FOUND;
    UmiCaptionTrack *copy = NULL;
    UmiStatus status = Copy(track, &copy);
    if (status != UMI_STATUS_OK) return status;
    --copy->info.cue_count;
    memmove(copy->cues + index, copy->cues + index + 1U,
            (copy->info.cue_count - index) * sizeof(*copy->cues));
    memset(copy->cues + copy->info.cue_count, 0, sizeof(*copy->cues));
    return Publish(copy, out);
}
