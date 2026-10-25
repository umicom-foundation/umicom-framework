/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_caption_text.c
 * PURPOSE: Verify native caption-to-text extraction through the canonical
 *          Creative Workspace cue owner, including unchanged failed outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/caption_text.h"

#include <stdio.h>
#include <string.h>

#define CHECK(value) do { \
    if (!(value)) { \
        fprintf(stderr, "Caption extraction assertion %d: %s\n", __LINE__, #value); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiCaptionTrack *track = NULL;
    UmiCaptionCue cues[2] = {
        {0U, 800U, "A fictional model learns from approved material."},
        {810U, 1600U, "Its evaluation set uses different sources."}
    };
    char output[256] = {0};
    size_t length = SIZE_MAX;
    const char expected[] =
        "A fictional model learns from approved material.\n"
        "Its evaluation set uses different sources.\n";

    CHECK(UmiCaptionTrackCreate("Synthetic sample", cues, 2U, &track) ==
        UMI_STATUS_OK);
    CHECK(UmiAiTrainingCaptionTextExtract(track, output, 2U, &length) ==
        UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(length == SIZE_MAX && output[0] == '\0');
    CHECK(UmiAiTrainingCaptionTextExtract(track, output, sizeof(output), &length) ==
        UMI_STATUS_OK);
    CHECK(length == sizeof(expected) - 1U);
    CHECK(strcmp(output, expected) == 0);
    UmiCaptionTrackDestroy(track);
    puts("Framework caption-to-text extraction: PASS");
    return 0;
}
