/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/captions_internal.h
 * PURPOSE: Keep caption ownership and validation in the shared creative service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_CAPTIONS_INTERNAL_H
#define UMICOM_CREATIVE_CAPTIONS_INTERNAL_H
#include "internal.h"
#include "umicom/creative_workspace/captions.h"
struct UmiCaptionTrack
{
    UmiCaptionInfo info;
    UmiCaptionCue cues[UMI_CAPTION_CUE_LIMIT];
};
bool UmiCaptionCueValid(const UmiCaptionCue *cue);
UmiStatus UmiCaptionTrackAllocate(const char *title, UmiCaptionTrack **out);
void UmiCaptionTrackMeasure(UmiCaptionTrack *track);
#endif
