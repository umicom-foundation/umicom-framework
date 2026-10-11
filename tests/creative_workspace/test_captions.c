/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_captions.c
 * PURPOSE: Exercise caption interchange, immutable edits and failure boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/captions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(x))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                        \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static const char source[] = "1\n00:00:01,000 --> 00:00:04,000\nFirst line\nSecond line\n\n"
                             "2\n00:00:03,000 --> 00:00:05,000\nAnother speaker\n\n";
static UmiCaptionTrack *Load(void)
{
    UmiCaptionTrack *track = NULL;
    OK(UmiCaptionTrackParseSrt("Lesson", source, strlen(source), &track));
    return track;
}
/* Compare the actual interchange bytes as well as timing, so losing text,
 * overlap or a blank separator is observable independently of the parser. */
static void Bytes(const UmiCaptionTrack *track, UmiCaptionFormat format, const char *expected)
{
    UmiCreativeAsset *asset = NULL;
    OK(UmiCaptionTrackExport(track, format, NULL, &asset));
    const void *bytes = NULL;
    size_t size = 0;
    OK(UmiCreativeAssetBytes(asset, &bytes, &size));
    CHECK(size == strlen(expected) && !memcmp(bytes, expected, size));
    UmiCreativeAssetDestroy(asset);
}
static void Cue(const UmiCaptionTrack *track, size_t index, uint32_t begin, uint32_t end,
                const char *text)
{
    UmiCaptionCue cue;
    OK(UmiCaptionTrackCue(track, index, &cue));
    CHECK(cue.begin_ms == begin && cue.end_ms == end && !strcmp(cue.text, text));
}
static void Count(const UmiCaptionTrack *track, size_t count, uint32_t end)
{
    UmiCaptionInfo info;
    OK(UmiCaptionTrackInspect(track, &info));
    CHECK(info.cue_count == count && info.end_ms == end);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiCaptionTrack *track = Load(), *next = NULL;
    /* Invalid documents must reject atomically. The caller's independent
     * existing track must remain readable after every rejected candidate. */
    const char *bad = NULL;
    if (!strcmp(mode, "invalid-time"))
        bad = "1\n00:60:00,000 --> 01:00:01,000\nBad\n";
    else if (!strcmp(mode, "invalid-order"))
        bad = "1\n00:00:03,000 --> 00:00:04,000\nLater\n\n2\n00:00:01,000 --> "
              "00:00:02,000\nEarlier\n";
    else if (!strcmp(mode, "invalid-number"))
        bad = "4294967296\n00:00:01,000 --> 00:00:02,000\nBad\n";
    else if (!strcmp(mode, "empty-cue"))
        bad = "1\n00:00:01,000 --> 00:00:02,000\n\n";
    else if (!strcmp(mode, "lone-cr"))
        bad = "1\n00:00:01,000 --> 00:00:02,000\nBad\r";
    else if (!strcmp(mode, "utf8"))
        bad = "1\n00:00:01,000 --> 00:00:02,000\n\xc0\xaf\n";
    else if (!strcmp(mode, "markup"))
        bad = "1\n00:00:01,000 --> 00:00:02,000\n<b>Styled</b>\n";
    if (bad)
    {
        CHECK(UmiCaptionTrackParseSrt("Lesson", bad, strlen(bad), &next) == UMI_STATUS_PARSE_ERROR);
        CHECK(!next);
        Bytes(track, UMI_CAPTION_SRT, source);
    }
    else if (!strcmp(mode, "import") || !strcmp(mode, "overlap"))
    {
        Count(track, 2, 5000);
        Cue(track, 0, 1000, 4000, "First line\nSecond line");
        Cue(track, 1, 3000, 5000, "Another speaker");
        Bytes(track, UMI_CAPTION_SRT, source);
    }
    else if (!strcmp(mode, "crlf") || !strcmp(mode, "bom") || !strcmp(mode, "identifiers"))
    {
        const char *input =
            !strcmp(mode, "crlf")  ? "1\r\n00:00:01,000 --> 00:00:02,000\r\ncaf\xc3\xa9\r\n\r\n"
            : !strcmp(mode, "bom") ? "\xef\xbb\xbf"
                                     "1\n00:00:01,000 --> 00:00:02,000\ncaf\xc3\xa9"
                                   : "42\n00:00:01,000 --> 00:00:02,000\ncaf\xc3\xa9\n";
        OK(UmiCaptionTrackParseSrt("Lesson", input, strlen(input), &next));
        Cue(next, 0, 1000, 2000, "caf\xc3\xa9");
        Bytes(next, UMI_CAPTION_SRT, "1\n00:00:01,000 --> 00:00:02,000\ncaf\xc3\xa9\n\n");
    }
    else if (!strcmp(mode, "empty"))
    {
        OK(UmiCaptionTrackParseSrt("Empty", NULL, 0, &next));
        Count(next, 0, 0);
        Bytes(next, UMI_CAPTION_SRT, "");
        Bytes(next, UMI_CAPTION_WEBVTT, "WEBVTT\n\n");
    }
    else if (!strcmp(mode, "null-byte"))
    {
        CHECK(UmiCaptionTrackParseSrt("Bad", source, sizeof(source), &next) ==
              UMI_STATUS_PARSE_ERROR);
        CHECK(!next);
    }
    else if (!strcmp(mode, "document-capacity"))
    {
        CHECK(UmiCaptionTrackParseSrt("Bad", source, UMI_CAPTION_DOCUMENT_LIMIT + 1U, &next) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!next);
    }
    else if (!strcmp(mode, "text-capacity"))
    {
        /* A full first line followed by another line used to be an unsigned
         * subtraction hazard. Test it at the exact capacity boundary. */
        const char *prefix = "1\n00:00:01,000 --> 00:00:02,000\n";
        size_t n = strlen(prefix);
        char *input = calloc(n + UMI_CAPTION_TEXT_CAPACITY + 4U, 1);
        CHECK(input);
        memcpy(input, prefix, n);
        memset(input + n, 'x', UMI_CAPTION_TEXT_CAPACITY - 1U);
        OK(UmiCaptionTrackParseSrt("Full", input, strlen(input), &next));
        UmiCaptionTrackDestroy(next);
        next = NULL;
        strcat(input, "\ny");
        CHECK(UmiCaptionTrackParseSrt("Full", input, strlen(input), &next) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!next);
        free(input);
    }
    else if (!strcmp(mode, "cue-capacity"))
    {
        UmiCaptionCue *cues = calloc(UMI_CAPTION_CUE_LIMIT, sizeof(*cues));
        CHECK(cues);
        for (size_t i = 0; i < UMI_CAPTION_CUE_LIMIT; ++i)
        {
            cues[i].begin_ms = (uint32_t)i;
            cues[i].end_ms = (uint32_t)i + 1U;
            strcpy(cues[i].text, "Caption");
        }
        OK(UmiCaptionTrackCreate("Full", cues, UMI_CAPTION_CUE_LIMIT, &next));
        UmiCaptionTrack *extra = NULL;
        CHECK(UmiCaptionTrackInsert(next, UMI_CAPTION_CUE_LIMIT, &cues[0], &extra) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!extra);
        UmiCreativeAsset *asset = NULL;
        OK(UmiCaptionTrackExport(next, UMI_CAPTION_SRT, NULL, &asset));
        const void *bytes = NULL;
        size_t size = 0;
        OK(UmiCreativeAssetBytes(asset, &bytes, &size));
        OK(UmiCaptionTrackParseSrt("Round trip", bytes, size, &extra));
        Count(extra, UMI_CAPTION_CUE_LIMIT, 512);
        UmiCaptionTrackDestroy(extra);
        UmiCreativeAssetDestroy(asset);
        free(cues);
    }
    else if (!strcmp(mode, "shift-forward") || !strcmp(mode, "shift-backward"))
    {
        int64_t offset = !strcmp(mode, "shift-forward") ? 1500 : -1000;
        OK(UmiCaptionTrackShift(track, offset, &next));
        Cue(next, 0, (uint32_t)(1000 + offset), (uint32_t)(4000 + offset),
            "First line\nSecond line");
        Bytes(track, UMI_CAPTION_SRT, source);
    }
    else if (!strcmp(mode, "shift-underflow") || !strcmp(mode, "shift-overflow") ||
             !strcmp(mode, "shift-intmin"))
    {
        int64_t offset = !strcmp(mode, "shift-underflow") ? -1001
                         : !strcmp(mode, "shift-intmin")  ? INT64_MIN
                                                          : UMI_CAPTION_TIME_LIMIT_MS;
        CHECK(UmiCaptionTrackShift(track, offset, &next) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!next);
        Bytes(track, UMI_CAPTION_SRT, source);
    }
    else if (!strcmp(mode, "clip-rebase") || !strcmp(mode, "clip-original"))
    {
        bool rebase = !strcmp(mode, "clip-rebase");
        OK(UmiCaptionTrackClip(track, 3000, 4000, rebase, &next));
        Count(next, 2, rebase ? 1000 : 4000);
        Cue(next, 0, rebase ? 0 : 3000, rebase ? 1000 : 4000, "First line\nSecond line");
        Cue(next, 1, rebase ? 0 : 3000, rebase ? 1000 : 4000, "Another speaker");
        Bytes(track, UMI_CAPTION_SRT, source);
    }
    else if (!strcmp(mode, "clip-empty"))
    {
        OK(UmiCaptionTrackClip(track, 5000, 6000, true, &next));
        Count(next, 0, 0);
        UmiCaptionTrackDestroy(next);
        next = NULL;
        OK(UmiCaptionTrackClip(track, 0, 1000, true, &next));
        Count(next, 0, 0);
    }
    else if (!strcmp(mode, "replace"))
    {
        UmiCaptionCue cue = {2000, 2500, "Replacement"};
        OK(UmiCaptionTrackReplace(track, 0, &cue, &next));
        Cue(next, 0, 2000, 2500, "Replacement");
        Bytes(track, UMI_CAPTION_SRT, source);
    }
    else if (!strcmp(mode, "insert"))
    {
        UmiCaptionCue cue = {2500, 3500, "Inserted"};
        OK(UmiCaptionTrackInsert(track, 1, &cue, &next));
        Count(next, 3, 5000);
        Cue(next, 1, 2500, 3500, "Inserted");
        Cue(next, 2, 3000, 5000, "Another speaker");
    }
    else if (!strcmp(mode, "remove"))
    {
        OK(UmiCaptionTrackRemove(track, 1, &next));
        Count(next, 1, 4000);
        UmiCaptionTrack *empty = NULL;
        OK(UmiCaptionTrackRemove(next, 0, &empty));
        Count(empty, 0, 0);
        UmiCaptionTrackDestroy(empty);
        Bytes(track, UMI_CAPTION_SRT, source);
    }
    else if (!strcmp(mode, "edit-order"))
    {
        UmiCaptionCue cue = {6000, 7000, "Out of order"};
        CHECK(UmiCaptionTrackReplace(track, 0, &cue, &next) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!next);
        CHECK(UmiCaptionTrackInsert(track, 1, &cue, &next) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!next);
        Bytes(track, UMI_CAPTION_SRT, source);
    }
    else if (!strcmp(mode, "export-srt"))
        Bytes(track, UMI_CAPTION_SRT, source);
    else if (!strcmp(mode, "export-vtt"))
        Bytes(track, UMI_CAPTION_WEBVTT,
              "WEBVTT\n\n1\n00:00:01.000 --> 00:00:04.000\nFirst line\nSecond line\n\n"
              "2\n00:00:03.000 --> 00:00:05.000\nAnother speaker\n\n");
    else if (!strcmp(mode, "cancel"))
    {
        UmiCancellationToken *cancel = NULL;
        OK(umi_cancellation_token_create(&cancel));
        umi_cancellation_token_request(cancel);
        UmiCreativeAsset *asset = NULL;
        CHECK(UmiCaptionTrackExport(track, UMI_CAPTION_SRT, cancel, &asset) ==
              UMI_STATUS_CANCELLED);
        CHECK(!asset);
        umi_cancellation_token_destroy(cancel);
    }
    else if (!strcmp(mode, "live-output"))
    {
        next = track;
        CHECK(UmiCaptionTrackShift(track, 1, &next) == UMI_STATUS_INVALID_ARGUMENT &&
              next == track);
        CHECK(UmiCaptionTrackParseSrt("No", source, strlen(source), &next) ==
              UMI_STATUS_INVALID_ARGUMENT);
        next = NULL;
        Bytes(track, UMI_CAPTION_SRT, source);
    }
    else if (!strcmp(mode, "read-bounds"))
    {
        UmiCaptionCue cue = {123, 456, "Keep"}, before = cue;
        CHECK(UmiCaptionTrackCue(track, SIZE_MAX, &cue) == UMI_STATUS_NOT_FOUND);
        CHECK(cue.begin_ms == before.begin_ms && !strcmp(cue.text, before.text));
        CHECK(UmiCaptionTrackInspect(NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else
        CHECK(false);
    UmiCaptionTrackDestroy(next);
    UmiCaptionTrackDestroy(track);
    return 0;
}
