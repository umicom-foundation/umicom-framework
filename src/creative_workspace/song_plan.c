/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/song_plan.c
 * PURPOSE: Compute lyric intervals and shot boundaries without accumulating beat drift.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "song_plan_internal.h"
#include "umicom/ai/mcp/json.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include <string.h>
/* Reuse complete JSON text validation for Unicode rather than assuming bytes
 * from a file or future web form were already checked by a native widget. */
static UmiStatus TextValid(const char *text, size_t capacity, bool multiline, bool required)
{
    if (!memchr(text, 0, capacity))
        return UMI_STATUS_INVALID_ARGUMENT;
    bool visible = false;
    for (size_t index = 0; text[index]; ++index)
    {
        unsigned char c = (unsigned char)text[index];
        if (c == 0x7fU || (c < 0x20U && !(multiline && (c == '\n' || c == '\r'))))
            return UMI_STATUS_INVALID_ARGUMENT;
        visible = visible || c > 0x20U;
    }
    if (required && !visible)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t bound = capacity * 6U + 3U;
    char *quoted = malloc(bound);
    if (!quoted)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_ai_mcp_json_escape_string(text, quoted, bound);
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {bound, 4U, 2U};
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeCreate(quoted, strlen(quoted), &limits, NULL, &tree);
    UmiJsonTreeDestroy(tree);
    umi_secret_clear(quoted, bound);
    free(quoted);
    return status;
}
/* Timestamp positions are checked as decimal digits before this conversion. Keeping the conversion small makes the accepted spelling easy to audit. */
static unsigned Digit(char c) { return (unsigned)(c - '0'); }
/* Convert a deliberately small timed-lyrics format into owned half-open intervals. Extend this parser explicitly if additional lyric formats are supported; do not guess missing timestamps. */
static UmiStatus Lyrics(UmiSongPlan *plan)
{
    const char *line = plan->draft.timed_lyrics;
    while (*line)
    {
        const char *end = strchr(line, '\n');
        if (!end)
            end = line + strlen(line);
        size_t length = (size_t)(end - line);
        if (length && line[length - 1] == '\r')
            --length;
        if (length)
        {
            if (length <= 11U || line[0] != '[' || line[3] != ':' || line[6] != '.' || line[10] != ']')
                return UMI_STATUS_PARSE_ERROR;
            const size_t positions[] = {1, 2, 4, 5, 7, 8, 9};
            for (size_t index = 0; index < sizeof(positions) / sizeof(positions[0]); ++index)
                if (line[positions[index]] < '0' || line[positions[index]] > '9')
                    return UMI_STATUS_PARSE_ERROR;
            unsigned seconds = Digit(line[4]) * 10U + Digit(line[5]);
            unsigned time = (Digit(line[1]) * 10U + Digit(line[2])) * 60000U + seconds * 1000U +
                            Digit(line[7]) * 100U + Digit(line[8]) * 10U + Digit(line[9]);
            if (seconds >= 60U || time >= plan->draft.duration_ms ||
                (plan->cue_count && time <= plan->cues[plan->cue_count - 1U].start_ms))
                return UMI_STATUS_INVALID_ARGUMENT;
            if (plan->cue_count == UMI_SONG_CUE_LIMIT || length - 11U >= sizeof(plan->cues[0].text))
                return UMI_STATUS_CAPACITY_EXCEEDED;
            UmiSongCue *cue = &plan->cues[plan->cue_count];
            memcpy(cue->text, line + 11U, length - 11U);
            /* SRT interprets angle brackets as markup in some players. Keep
             * captions literal by refusing them rather than changing lyrics. */
            if (strchr(cue->text, '<') || strchr(cue->text, '>'))
                return UMI_STATUS_INVALID_ARGUMENT;
            UmiStatus status = TextValid(cue->text, sizeof(cue->text), false, true);
            if (status != UMI_STATUS_OK)
                return status;
            cue->start_ms = time;
            cue->end_ms = plan->draft.duration_ms;
            if (plan->cue_count)
                plan->cues[plan->cue_count - 1U].end_ms = time;
            ++plan->cue_count;
        }
        line = *end ? end + 1 : end;
    }
    return UMI_STATUS_OK;
}
/* Associate every overlapping cue with a shot. A cue ending exactly at a cut belongs only to the earlier shot, which avoids double captions at boundaries. */
static UmiStatus Shot(UmiSongPlan *plan, unsigned begin, unsigned end)
{
    if (plan->shot_count == UMI_SONG_SHOT_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiSongShot *shot = &plan->shots[plan->shot_count++];
    shot->start_ms = begin;
    shot->end_ms = end;
    for (size_t index = 0; index < plan->cue_count; ++index)
    {
        const UmiSongCue *cue = &plan->cues[index];
        if (cue->start_ms < end && cue->end_ms > begin)
        {
            if (!shot->cue_count)
                shot->first_cue = index;
            ++shot->cue_count;
        }
    }
    return UMI_STATUS_OK;
}
/* Validate the entire draft before publishing the new owner. A failed cue or an oversized shot list releases temporary state and leaves the caller without a partial plan. */
UmiStatus UmiSongPlanCreate(const UmiSongDraft *draft, UmiSongPlan **out)
{
    if (!draft || !out || *out || !draft->duration_ms || draft->duration_ms > 600000U ||
        draft->beats_per_minute < 30U || draft->beats_per_minute > 300U || !draft->beats_per_shot ||
        draft->beats_per_shot > 32U || draft->first_beat_ms >= draft->duration_ms)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = TextValid(draft->title, sizeof(draft->title), false, true);
    if (status == UMI_STATUS_OK)
        status = TextValid(draft->style, sizeof(draft->style), false, false);
    if (status == UMI_STATUS_OK)
        status = TextValid(draft->timed_lyrics, sizeof(draft->timed_lyrics), true, false);
    if (status != UMI_STATUS_OK)
        return status;
    UmiSongPlan *plan = calloc(1, sizeof(*plan));
    if (!plan)
        return UMI_STATUS_OUT_OF_MEMORY;
    plan->draft = *draft;
    status = Lyrics(plan);
    unsigned begin = draft->first_beat_ms;
    if (status == UMI_STATUS_OK && begin)
        status = Shot(plan, 0U, begin);
    /* Calculate each cut from the origin using wide integer arithmetic. Adding
     * a rounded beat length repeatedly would drift on long songs at odd tempos. */
    for (uint64_t step = 1; status == UMI_STATUS_OK && begin < draft->duration_ms; ++step)
    {
        uint64_t boundary =
            (uint64_t)draft->first_beat_ms +
            (step * draft->beats_per_shot * 60000U + draft->beats_per_minute / 2U) / draft->beats_per_minute;
        unsigned end = boundary >= draft->duration_ms ? draft->duration_ms : (unsigned)boundary;
        status = Shot(plan, begin, end);
        begin = end;
    }
    if (status == UMI_STATUS_OK)
        *out = plan;
    else
        UmiSongPlanDestroy(plan);
    return status;
}
/* Retire the copied lyrics together with their derived timing. Readers must finish before the owner is destroyed. */
void UmiSongPlanDestroy(UmiSongPlan *plan)
{
    if (plan)
    {
        umi_secret_clear(plan, sizeof(*plan));
        free(plan);
    }
}
/* Return a value copy for an editable form. Editing this copy cannot change an already-reviewed plan. */
UmiStatus UmiSongPlanDraft(const UmiSongPlan *plan, UmiSongDraft *out)
{
    if (!plan || !out)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = plan->draft;
    return UMI_STATUS_OK;
}
/* Expose bounded counts so frontends can allocate rows without seeing private storage. */
UmiStatus UmiSongPlanCounts(const UmiSongPlan *plan, size_t *cues, size_t *shots)
{
    if (!plan || !cues || !shots)
        return UMI_STATUS_INVALID_ARGUMENT;
    *cues = plan->cue_count;
    *shots = plan->shot_count;
    return UMI_STATUS_OK;
}
/* Copy one cue only after checking its index. Invalid requests preserve the caller's previous row. */
UmiStatus UmiSongPlanCue(const UmiSongPlan *plan, size_t index, UmiSongCue *out)
{
    if (!plan || !out)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= plan->cue_count)
        return UMI_STATUS_NOT_FOUND;
    *out = plan->cues[index];
    return UMI_STATUS_OK;
}
/* Copy one shot for a timeline or storyboard view without lending mutable internal arrays. */
UmiStatus UmiSongPlanShot(const UmiSongPlan *plan, size_t index, UmiSongShot *out)
{
    if (!plan || !out)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= plan->shot_count)
        return UMI_STATUS_NOT_FOUND;
    *out = plan->shots[index];
    return UMI_STATUS_OK;
}
