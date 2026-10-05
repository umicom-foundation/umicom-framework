/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_song_plan.c
 * PURPOSE: Cover timing math, caption export and hostile song plan imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/song_plan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *test = argv[1];
    UmiSongDraft *draft = calloc(1, sizeof(*draft));
    CHECK(draft);
    strcpy(draft->title, "Song \"review\"");
    strcpy(draft->style, "A warm sunrise");
    strcpy(draft->timed_lyrics, "[00:00.000]First line\n[00:02.500]Second line\n");
    draft->duration_ms = 5500;
    draft->beats_per_minute = 120;
    draft->beats_per_shot = 4;
    if (!strcmp(test, "offset"))
        draft->first_beat_ms = 500;
    if (!strcmp(test, "odd-tempo"))
    {
        draft->beats_per_minute = 137;
        draft->duration_ms = 600000;
        draft->beats_per_shot = 8;
    }
    if (!strcmp(test, "instrumental"))
        draft->timed_lyrics[0] = 0;
    if (!strcmp(test, "crlf"))
        strcpy(draft->timed_lyrics, "\r\n[00:00.000]First line\r\n\r\n[00:02.500]Second line\r\n");
    if (!strcmp(test, "duplicate-time"))
        strcpy(draft->timed_lyrics, "[00:00.000]A\n[00:00.000]B");
    if (!strcmp(test, "outside"))
        strcpy(draft->timed_lyrics, "[00:05.500]Late");
    if (!strcmp(test, "seconds"))
        strcpy(draft->timed_lyrics, "[00:60.000]Bad");
    if (!strcmp(test, "precision"))
        strcpy(draft->timed_lyrics, "[00:01.50]Bad");
    if (!strcmp(test, "blank-cue"))
        strcpy(draft->timed_lyrics, "[00:01.500] ");
    if (!strcmp(test, "markup"))
        strcpy(draft->timed_lyrics, "[00:00.000]<b>Bold</b>");
    if (!strcmp(test, "utf8"))
        draft->title[0] = (char)0xff;
    if (!strcmp(test, "unterminated"))
        memset(draft->title, 'a', sizeof(draft->title));
    if (!strcmp(test, "capacity"))
    {
        draft->duration_ms = 600000;
        draft->beats_per_minute = 300;
        draft->beats_per_shot = 1;
    }
    bool invalid = !strcmp(test, "duplicate-time") || !strcmp(test, "outside") || !strcmp(test, "seconds") ||
                   !strcmp(test, "precision") || !strcmp(test, "blank-cue") || !strcmp(test, "markup") ||
                   !strcmp(test, "utf8") || !strcmp(test, "unterminated") || !strcmp(test, "capacity");
    UmiSongPlan *plan = NULL;
    UmiStatus status = UmiSongPlanCreate(draft, &plan);
    if (invalid)
    {
        CHECK(status != UMI_STATUS_OK && !plan);
        free(draft);
        return 0;
    }
    CHECK(status == UMI_STATUS_OK && plan);
    CHECK(UmiSongPlanCreate(draft, &plan) == UMI_STATUS_INVALID_ARGUMENT);
    size_t cues = 0, shots = 0;
    CHECK(UmiSongPlanCounts(plan, &cues, &shots) == UMI_STATUS_OK);
    CHECK(cues == (!strcmp(test, "instrumental") ? 0U : 2U));
    UmiSongShot shot;
    unsigned previous = 0;
    for (size_t index = 0; index < shots; ++index)
    {
        CHECK(UmiSongPlanShot(plan, index, &shot) == UMI_STATUS_OK);
        CHECK(shot.start_ms == previous && shot.end_ms > shot.start_ms);
        previous = shot.end_ms;
        if (!strcmp(test, "odd-tempo") && index + 1U < shots)
            CHECK(shot.end_ms == ((uint64_t)(index + 1U) * 8U * 60000U + 68U) / 137U);
    }
    CHECK(previous == draft->duration_ms);
    if (!strcmp(test, "offset"))
    {
        CHECK(UmiSongPlanShot(plan, 0, &shot) == UMI_STATUS_OK);
        CHECK(shot.end_ms == 500);
    }
    UmiSongCue cue;
    if (cues)
    {
        CHECK(UmiSongPlanCue(plan, 0, &cue) == UMI_STATUS_OK && cue.end_ms == 2500);
    }
    UmiCreativeAsset *asset = NULL;
    UmiSongExport kind = !strcmp(test, "srt") ? UMI_SONG_EXPORT_SUBTITLES : UMI_SONG_EXPORT_PLAN;
    CHECK(UmiSongPlanExport(plan, kind, NULL, &asset) == UMI_STATUS_OK);
    const void *bytes = NULL;
    size_t length = 0;
    CHECK(UmiCreativeAssetBytes(asset, &bytes, &length) == UMI_STATUS_OK);
    if (!strcmp(test, "srt"))
    {
        const char expected[] = "1\n00:00:00,000 --> 00:00:02,500\nFirst line\n\n2\n00:00:02,500 --> "
                                "00:00:05,500\nSecond line\n\n";
        CHECK(length == strlen(expected) && !memcmp(bytes, expected, length));
    }
    else
    {
        char *changed = malloc(length + 128U);
        CHECK(changed);
        memcpy(changed, bytes, length);
        changed[length] = 0;
        size_t input_length = length;
        if (!strcmp(test, "import-duplicate"))
        {
            snprintf(changed + length - 2U, 128U, ",\"duration_ms\":1}");
            input_length = strlen(changed);
        }
        if (!strcmp(test, "import-trailing"))
        {
            changed[length] = 'x';
            changed[length + 1U] = 0;
            input_length = length + 1U;
        }
        UmiSongPlan *restored = NULL;
        status = UmiSongPlanImport(changed, input_length, NULL, &restored);
        if (!strncmp(test, "import-", 7))
            CHECK(status != UMI_STATUS_OK && !restored);
        else
        {
            CHECK(status == UMI_STATUS_OK);
            UmiSongDraft *copy = calloc(1, sizeof(*copy));
            CHECK(copy);
            CHECK(UmiSongPlanDraft(restored, copy) == UMI_STATUS_OK);
            CHECK(!strcmp(copy->title, draft->title) && !strcmp(copy->timed_lyrics, draft->timed_lyrics));
            free(copy);
        }
        UmiSongPlanDestroy(restored);
        free(changed);
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    umi_cancellation_token_request(cancel);
    UmiCreativeAsset *cancelled = NULL;
    CHECK(UmiSongPlanExport(plan, kind, cancel, &cancelled) == UMI_STATUS_CANCELLED && !cancelled);
    umi_cancellation_token_destroy(cancel);
    UmiCreativeAssetDestroy(asset);
    UmiSongPlanDestroy(plan);
    free(draft);
    return 0;
}
