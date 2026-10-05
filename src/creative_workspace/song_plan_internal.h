/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/song_plan_internal.h
 * PURPOSE: Keep lyric and shot storage private to the shared song owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_WORKSPACE_SONG_PLAN_INTERNAL_H
#define UMICOM_CREATIVE_WORKSPACE_SONG_PLAN_INTERNAL_H
#include "umicom/creative_workspace/song_plan.h"
struct UmiSongPlan
{
    UmiSongDraft draft;
    size_t cue_count, shot_count;
    UmiSongCue cues[UMI_SONG_CUE_LIMIT];
    UmiSongShot shots[UMI_SONG_SHOT_LIMIT];
};
#endif
