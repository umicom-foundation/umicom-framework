/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/captions.c
 * PURPOSE: Validate and own immutable caption documents for all creative applications.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "captions_internal.h"
#include <stdlib.h>
#include <string.h>
/* Validate one line through the existing UTF-8 owner. Splitting only on LF
 * prevents platform line endings and formatting from changing exported meaning. */
bool UmiCaptionCueValid(const UmiCaptionCue *cue)
{
    if (!cue || cue->begin_ms >= cue->end_ms || cue->end_ms > UMI_CAPTION_TIME_LIMIT_MS)
        return false;
    const char *end = memchr(cue->text, 0, sizeof(cue->text));
    if (!end || end == cue->text) return false;
    char line[UMI_CAPTION_TEXT_CAPACITY];
    size_t used = 0;
    for (const char *p = cue->text;; ++p)
    {
        if (*p == '<' || *p == '>' || *p == '&') return false;
        if (*p == '\n' || !*p)
        {
            line[used] = 0;
            if (!UmiCreativeTextValid(line, sizeof(line), false)) return false;
            used = 0;
            if (!*p) break;
        }
        else line[used++] = *p;
    }
    return true;
}
/* Large cue storage belongs on the heap, including in tests on small Windows
 * stacks. A title is copied once and never borrowed from an editor widget. */
UmiStatus UmiCaptionTrackAllocate(const char *title, UmiCaptionTrack **out)
{
    if (!out || *out || !UmiCreativeTextValid(title, UMI_CREATIVE_LABEL_CAPACITY, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCaptionTrack *track = calloc(1, sizeof(*track));
    if (!track) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(track->info.title, title, strlen(title) + 1U);
    *out = track;
    return UMI_STATUS_OK;
}
/* Overlapping captions mean the last row need not have the latest end time. */
void UmiCaptionTrackMeasure(UmiCaptionTrack *track)
{
    track->info.end_ms = 0;
    for (size_t i = 0; i < track->info.cue_count; ++i)
        if (track->cues[i].end_ms > track->info.end_ms)
            track->info.end_ms = track->cues[i].end_ms;
}
UmiStatus UmiCaptionTrackCreate(const char *title, const UmiCaptionCue *cues,
                              size_t count, UmiCaptionTrack **out)
{
    if (!out || *out || (count && !cues)) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_CAPTION_CUE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0; i < count; ++i)
        if (!UmiCaptionCueValid(&cues[i]) || (i && cues[i].begin_ms < cues[i-1U].begin_ms))
            return UMI_STATUS_INVALID_ARGUMENT;
    UmiCaptionTrack *track = NULL;
    UmiStatus status = UmiCaptionTrackAllocate(title, &track);
    if (status != UMI_STATUS_OK) return status;
    if (count) memcpy(track->cues, cues, count * sizeof(*cues));
    track->info.cue_count = count;
    UmiCaptionTrackMeasure(track);
    *out = track;
    return UMI_STATUS_OK;
}
void UmiCaptionTrackDestroy(UmiCaptionTrack *track) { free(track); }
UmiStatus UmiCaptionTrackInspect(const UmiCaptionTrack *track, UmiCaptionInfo *out)
{
    if (!track || !out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = track->info;
    return UMI_STATUS_OK;
}
UmiStatus UmiCaptionTrackCue(const UmiCaptionTrack *track, size_t index, UmiCaptionCue *out)
{
    if (!track || !out) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= track->info.cue_count) return UMI_STATUS_NOT_FOUND;
    *out = track->cues[index];
    return UMI_STATUS_OK;
}
