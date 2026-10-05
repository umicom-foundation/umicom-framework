/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/song_plan.h
 * PURPOSE: Share owned lyric cues and beat-aligned video shot planning.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_WORKSPACE_SONG_PLAN_H
#define UMICOM_CREATIVE_WORKSPACE_SONG_PLAN_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/creative_workspace/asset.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_SONG_CUE_LIMIT 256U
#define UMI_SONG_SHOT_LIMIT 512U
#define UMI_SONG_DOCUMENT_LIMIT (512U * 1024U)
    typedef struct UmiSongDraft
    {
        char title[257], style[1025], timed_lyrics[32769];
        unsigned duration_ms, beats_per_minute, beats_per_shot, first_beat_ms;
    } UmiSongDraft;
    typedef struct UmiSongCue
    {
        unsigned start_ms, end_ms;
        char text[513];
    } UmiSongCue;
    typedef struct UmiSongShot
    {
        unsigned start_ms, end_ms;
        size_t first_cue, cue_count; /* No overlap is reported as first_cue=cue_count=0. */
    } UmiSongShot;
    typedef struct UmiSongPlan UmiSongPlan;
    typedef enum UmiSongExport
    {
        UMI_SONG_EXPORT_PLAN = 1,
        UMI_SONG_EXPORT_SUBTITLES
    } UmiSongExport;
    /* An immutable plan copies the draft and computes bounded cues/shots without
 * I/O. Initialise *out=NULL; failures never replace an existing plan.
 * Duration is 1..600000 ms, tempo 30..300, beats per shot 1..32, first beat
 * less than duration. Tempo is manually supplied, not inferred from audio.
 * Lyrics use exactly [mm:ss.mmm]text, one cue per line; blank lines and CRLF
 * are accepted. Times must strictly increase and precede the duration. Each
 * cue ends at the next cue or song end. Empty lyrics mean instrumental.
 * Limits are bytes of valid UTF-8. No lyrics, audio or video are AI-generated. */
    UmiStatus UmiSongPlanCreate(const UmiSongDraft *draft, UmiSongPlan **out);
    void UmiSongPlanDestroy(UmiSongPlan *plan);
    UmiStatus UmiSongPlanDraft(const UmiSongPlan *plan, UmiSongDraft *out);
    UmiStatus UmiSongPlanCounts(const UmiSongPlan *plan, size_t *cues, size_t *shots);
    UmiStatus UmiSongPlanCue(const UmiSongPlan *plan, size_t index, UmiSongCue *out);
    UmiStatus UmiSongPlanShot(const UmiSongPlan *plan, size_t index, UmiSongShot *out);
    /* Exports an immutable asset ready for create-new writing on a worker. JSON
 * retains the draft and includes derived shots/cues; SRT supplies captions for
 * an editor. Neither contains audio/video bytes or credentials. A plan import
 * validates its draft and recomputes derived data rather than trusting stale
 * or edited shot arrays. Unknown fields allow educational notes/extensions.
 * Owners must keep inputs alive during each call; cancellation is cooperative. */
    UmiStatus UmiSongPlanExport(const UmiSongPlan *plan, UmiSongExport kind,
                                const UmiCancellationToken *cancel, UmiCreativeAsset **out);
    UmiStatus UmiSongPlanImport(const void *bytes, size_t length, const UmiCancellationToken *cancel,
                                UmiSongPlan **out);
#ifdef __cplusplus
}
#endif
#endif
