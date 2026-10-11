/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_caption_webvtt.c
 * PURPOSE: Check caption format detection, round trips and all-or-nothing WebVTT imports.
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
static const char plain[] = "WEBVTT\n\n00:00:01.000 --> 00:00:04.000\nFirst\nSecond\n\n"
                            "00:03.000 --> 00:05.000\nAnother\n\n";
static const char srt[] = "1\n00:00:01,000 --> 00:00:04,000\nFirst\nSecond\n\n"
                          "2\n00:00:03,000 --> 00:00:05,000\nAnother\n\n";
typedef struct RejectedDocument
{
    const char *name, *text;
} RejectedDocument;
static const RejectedDocument rejected[] = {
    {"header", "webvtt\n\n"},
    {"header-suffix", "WEBVTTX\n\n"},
    {"header-text", "WEBVTT captions\n\n"},
    {"header-gap", "WEBVTT\n00:01.000 --> 00:02.000\nText\n"},
    {"metadata", "WEBVTT\nRegion: id=first\n\n"},
    {"note", "WEBVTT\n\nNOTE retained author note\n\n"},
    {"style", "WEBVTT\n\nSTYLE\n::cue { color: red; }\n\n"},
    {"region", "WEBVTT\n\nREGION\nid:first\n\n"},
    {"named-label", "WEBVTT\n\nintro\n00:01.000 --> 00:02.000\nText\n"},
    {"duplicate-label",
     "WEBVTT\n\n1\n00:01.000 --> 00:02.000\nA\n\n1\n00:02.000 --> 00:03.000\nB\n"},
    {"zero-label", "WEBVTT\n\n0\n00:01.000 --> 00:02.000\nText\n"},
    {"label-overflow", "WEBVTT\n\n4294967296\n00:01.000 --> 00:02.000\nText\n"},
    {"settings", "WEBVTT\n\n00:01.000 --> 00:02.000 align:start\nText\n"},
    {"markup", "WEBVTT\n\n00:01.000 --> 00:02.000\n<b>Text</b>\n"},
    {"entity", "WEBVTT\n\n00:01.000 --> 00:02.000\nA &amp; B\n"},
    {"cr", "WEBVTT\n\n00:01.000 --> 00:02.000\nText\r"},
    {"utf8", "WEBVTT\n\n00:01.000 --> 00:02.000\n\xc0\xaf\n"},
    {"minute", "WEBVTT\n\n60:01.000 --> 61:02.000\nText\n"},
    {"second", "WEBVTT\n\n00:60.000 --> 01:02.000\nText\n"},
    {"hour", "WEBVTT\n\n100:00:00.000 --> 100:00:01.000\nText\n"},
    {"millisecond", "WEBVTT\n\n00:01.00 --> 00:02.000\nText\n"},
    {"arrow", "WEBVTT\n\n00:01.000-->00:02.000\nText\n"},
    {"duration", "WEBVTT\n\n00:01.000 --> 00:01.000\nText\n"},
    {"order", "WEBVTT\n\n00:03.000 --> 00:04.000\nA\n\n00:01.000 --> 00:02.000\nB\n"},
    {"empty-cue", "WEBVTT\n\n00:01.000 --> 00:02.000\n\n"},
    {"incomplete", "WEBVTT\n\n1\n"}};
static void Cue(const UmiCaptionTrack *track, size_t index, uint32_t begin, uint32_t end,
                const char *text)
{
    UmiCaptionCue cue;
    OK(UmiCaptionTrackCue(track, index, &cue));
    CHECK(cue.begin_ms == begin && cue.end_ms == end && !strcmp(cue.text, text));
}
static void Count(const UmiCaptionTrack *track, size_t expected)
{
    UmiCaptionInfo info;
    OK(UmiCaptionTrackInspect(track, &info));
    CHECK(info.cue_count == expected);
}
static void Bytes(const UmiCaptionTrack *track, const char *expected)
{
    UmiCreativeAsset *asset = NULL;
    OK(UmiCaptionTrackExport(track, UMI_CAPTION_SRT, NULL, &asset));
    const void *bytes = NULL;
    size_t size = 0;
    OK(UmiCreativeAssetBytes(asset, &bytes, &size));
    CHECK(size == strlen(expected) && !memcmp(bytes, expected, size));
    UmiCreativeAssetDestroy(asset);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiCaptionTrack *track = NULL;
    /* Every malformed candidate leaves the output unpublished. Testing the
     * automatic reader too ensures it cannot fall back after a WebVTT failure. */
    for (size_t i = 0; i < sizeof(rejected) / sizeof(rejected[0]); ++i)
        if (!strcmp(mode, rejected[i].name))
        {
            const char *text = rejected[i].text;
            CHECK(UmiCaptionTrackParseWebVtt("Captions", text, strlen(text), &track) ==
                  UMI_STATUS_PARSE_ERROR);
            CHECK(!track);
            CHECK(UmiCaptionTrackParse("Captions", text, strlen(text), &track) ==
                  UMI_STATUS_PARSE_ERROR);
            CHECK(!track);
            return 0;
        }
    if (!strcmp(mode, "import") || !strcmp(mode, "auto-vtt") || !strcmp(mode, "auto-srt"))
    {
        const char *text = !strcmp(mode, "auto-srt") ? srt : plain;
        if (!strcmp(mode, "import"))
            OK(UmiCaptionTrackParseWebVtt("Captions", text, strlen(text), &track));
        else
            OK(UmiCaptionTrackParse("Captions", text, strlen(text), &track));
        Count(track, 2);
        Cue(track, 0, 1000, 4000, "First\nSecond");
        Cue(track, 1, 3000, 5000, "Another");
        Bytes(track, srt);
    }
    else if (!strcmp(mode, "short-time") || !strcmp(mode, "numeric-label") ||
             !strcmp(mode, "crlf") || !strcmp(mode, "bom") || !strcmp(mode, "long-hour"))
    {
        const char *text =
            !strcmp(mode, "numeric-label") ? "WEBVTT\n\n42\n00:01.000 --> 00:02.000\ncaf\xc3\xa9"
            : !strcmp(mode, "crlf") ? "WEBVTT\r\n\r\n00:01.000 --> 00:02.000\r\ncaf\xc3\xa9\r\n\r\n"
            : !strcmp(mode, "bom")  ? "\xef\xbb\xbfWEBVTT\n\n00:01.000 --> 00:02.000\ncaf\xc3\xa9"
            : !strcmp(mode, "long-hour")
                ? "WEBVTT\n\n000:00:01.000 --> 000:00:02.000\ncaf\xc3\xa9\n"
                : "WEBVTT\n\n00:01.000\t --> \t00:02.000 \t\ncaf\xc3\xa9\n";
        OK(UmiCaptionTrackParse("Captions", text, strlen(text), &track));
        Cue(track, 0, 1000, 2000, "caf\xc3\xa9");
        Bytes(track, "1\n00:00:01,000 --> 00:00:02,000\ncaf\xc3\xa9\n\n");
    }
    else if (!strcmp(mode, "empty"))
    {
        OK(UmiCaptionTrackParseWebVtt("Empty", "WEBVTT\n\n", 8, &track));
        Count(track, 0);
        Bytes(track, "");
    }
    else if (!strcmp(mode, "roundtrip"))
    {
        OK(UmiCaptionTrackParseSrt("Caption project", srt, strlen(srt), &track));
        UmiCreativeAsset *asset = NULL;
        OK(UmiCaptionTrackExport(track, UMI_CAPTION_WEBVTT, NULL, &asset));
        const void *bytes = NULL;
        size_t size = 0;
        OK(UmiCreativeAssetBytes(asset, &bytes, &size));
        UmiCaptionTrack *reopened = NULL;
        OK(UmiCaptionTrackParse("Reopened", bytes, size, &reopened));
        Bytes(reopened, srt);
        UmiCreativeAssetDestroy(asset);
        /* The reader owns its cue bytes after the source asset is destroyed. */
        Cue(reopened, 1, 3000, 5000, "Another");
        UmiCaptionTrackDestroy(reopened);
    }
    else if (!strcmp(mode, "null-byte"))
    {
        CHECK(UmiCaptionTrackParseWebVtt("Captions", plain, sizeof(plain), &track) ==
              UMI_STATUS_PARSE_ERROR);
        CHECK(!track);
    }
    else if (!strcmp(mode, "text-capacity"))
    {
        const char *prefix = "WEBVTT\n\n00:01.000 --> 00:02.000\n";
        size_t n = strlen(prefix);
        char *text = calloc(n + UMI_CAPTION_TEXT_CAPACITY + 4U, 1);
        CHECK(text);
        memcpy(text, prefix, n);
        memset(text + n, 'x', UMI_CAPTION_TEXT_CAPACITY - 1U);
        OK(UmiCaptionTrackParseWebVtt("Full", text, strlen(text), &track));
        UmiCaptionTrackDestroy(track);
        track = NULL;
        strcat(text, "\ny");
        CHECK(UmiCaptionTrackParseWebVtt("Too long", text, strlen(text), &track) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!track);
        free(text);
    }
    else if (!strcmp(mode, "cue-capacity"))
    {
        const char *cue = "00:01.000 --> 00:02.000\nText\n\n";
        size_t size = 8U + strlen(cue) * (UMI_CAPTION_CUE_LIMIT + 1U);
        char *text = calloc(size + 1U, 1);
        CHECK(text);
        strcpy(text, "WEBVTT\n\n");
        for (size_t i = 0; i < UMI_CAPTION_CUE_LIMIT; ++i)
            strcat(text, cue);
        OK(UmiCaptionTrackParseWebVtt("Full", text, strlen(text), &track));
        Count(track, UMI_CAPTION_CUE_LIMIT);
        UmiCaptionTrackDestroy(track);
        track = NULL;
        strcat(text, cue);
        CHECK(UmiCaptionTrackParseWebVtt("Too many", text, strlen(text), &track) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!track);
        free(text);
    }
    else if (!strcmp(mode, "document-capacity"))
    {
        CHECK(UmiCaptionTrackParse("Too big", plain, UMI_CAPTION_DOCUMENT_LIMIT + 1U, &track) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiCaptionTrackParseWebVtt("Too big", plain, UMI_CAPTION_DOCUMENT_LIMIT + 1U,
                                         &track) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!track);
    }
    else if (!strcmp(mode, "live-output"))
    {
        OK(UmiCaptionTrackParseWebVtt("Original", plain, strlen(plain), &track));
        UmiCaptionTrack *previous = track;
        CHECK(UmiCaptionTrackParse("No overwrite", srt, strlen(srt), &track) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCaptionTrackParseWebVtt("No overwrite", plain, strlen(plain), &track) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(track == previous);
        Bytes(track, srt);
    }
    else if (!strcmp(mode, "arguments"))
    {
        CHECK(UmiCaptionTrackParseWebVtt("Bad", NULL, 1, &track) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCaptionTrackParse("Bad", NULL, 1, &track) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCaptionTrackParseWebVtt("Bad", plain, strlen(plain), NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCaptionTrackParseWebVtt("Empty", NULL, 0, &track) == UMI_STATUS_PARSE_ERROR);
        CHECK(!track);
    }
    else
        CHECK(false);
    UmiCaptionTrackDestroy(track);
    return 0;
}
