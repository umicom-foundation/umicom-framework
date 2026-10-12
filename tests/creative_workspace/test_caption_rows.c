/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_caption_rows.c
 * PURPOSE: Exercise lossless caption splitting and merging, ordering, capacity and immutable failures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/captions.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiCaptionCue cues[3] = {{1000, 4000, "First"}, {3000, 5000, "Second"}, {6000, 7000, "Third"}};
    UmiCaptionTrack *source = NULL, *result = NULL;
    CHECK(UmiCaptionTrackCreate("Interview", cues, 3, &source) == UMI_STATUS_OK);
    UmiStatus status;
    if (!strcmp(mode, "split")) {
        status = UmiCaptionTrackSplit(source, 0, 2000, "Hello", "caf\xc3\xa9", &result);
        CHECK(status == UMI_STATUS_OK);
        UmiCaptionInfo info; UmiCaptionCue cue;
        CHECK(UmiCaptionTrackInspect(result, &info) == UMI_STATUS_OK && info.cue_count == 4 && info.end_ms == 7000);
        CHECK(UmiCaptionTrackCue(result, 0, &cue) == UMI_STATUS_OK);
        CHECK(cue.begin_ms == 1000 && cue.end_ms == 2000 && !strcmp(cue.text, "Hello"));
        CHECK(UmiCaptionTrackCue(result, 1, &cue) == UMI_STATUS_OK);
        CHECK(cue.begin_ms == 2000 && cue.end_ms == 4000 && !strcmp(cue.text, "caf\xc3\xa9"));
        CHECK(UmiCaptionTrackCue(result, 2, &cue) == UMI_STATUS_OK && !strcmp(cue.text, "Second"));
    } else if (!strcmp(mode, "equal-start")) {
        CHECK(UmiCaptionTrackSplit(source, 0, 3000, "A", "B", &result) == UMI_STATUS_OK);
        UmiCaptionCue cue;
        CHECK(UmiCaptionTrackCue(result, 1, &cue) == UMI_STATUS_OK && cue.begin_ms == 3000);
        CHECK(UmiCaptionTrackCue(result, 2, &cue) == UMI_STATUS_OK && cue.begin_ms == 3000);
    } else if (!strcmp(mode, "split-boundary")) {
        CHECK(UmiCaptionTrackSplit(source, 0, 1000, "A", "B", &result) == UMI_STATUS_INVALID_ARGUMENT && result == NULL);
        CHECK(UmiCaptionTrackSplit(source, 0, 4000, "A", "B", &result) == UMI_STATUS_INVALID_ARGUMENT && result == NULL);
    } else if (!strcmp(mode, "split-order")) {
        CHECK(UmiCaptionTrackSplit(source, 0, 3500, "A", "B", &result) == UMI_STATUS_INVALID_ARGUMENT && result == NULL);
    } else if (!strcmp(mode, "split-text")) {
        CHECK(UmiCaptionTrackSplit(source, 0, 2000, "", "B", &result) == UMI_STATUS_INVALID_ARGUMENT && result == NULL);
        CHECK(UmiCaptionTrackSplit(source, 0, 2000, "A", "<b>B</b>", &result) == UMI_STATUS_INVALID_ARGUMENT && result == NULL);
        CHECK(UmiCaptionTrackSplit(source, 0, 2000, "A", "\xc3", &result) == UMI_STATUS_INVALID_ARGUMENT && result == NULL);
        char large[UMI_CAPTION_TEXT_CAPACITY + 1U]; memset(large, 'a', sizeof(large)); large[sizeof(large)-1U] = '\0';
        CHECK(UmiCaptionTrackSplit(source, 0, 2000, large, "B", &result) == UMI_STATUS_CAPACITY_EXCEEDED && result == NULL);
    } else if (!strcmp(mode, "merge") || !strcmp(mode, "merge-gap")) {
        size_t index = !strcmp(mode, "merge") ? 0U : 1U;
        CHECK(UmiCaptionTrackMerge(source, index, &result) == UMI_STATUS_OK);
        UmiCaptionInfo info; UmiCaptionCue cue;
        CHECK(UmiCaptionTrackInspect(result, &info) == UMI_STATUS_OK && info.cue_count == 2);
        CHECK(UmiCaptionTrackCue(result, index, &cue) == UMI_STATUS_OK);
        CHECK(cue.begin_ms == cues[index].begin_ms && cue.end_ms == cues[index+1U].end_ms);
        CHECK(!strcmp(cue.text, index == 0U ? "First\nSecond" : "Second\nThird"));
    } else if (!strcmp(mode, "merge-enclosed")) {
        UmiCaptionCue longer = cues[0]; longer.end_ms = 6000;
        UmiCaptionTrack *overlap = NULL;
        CHECK(UmiCaptionTrackReplace(source, 0, &longer, &overlap) == UMI_STATUS_OK);
        CHECK(UmiCaptionTrackMerge(overlap, 0, &result) == UMI_STATUS_OK);
        UmiCaptionCue cue; CHECK(UmiCaptionTrackCue(result, 0, &cue) == UMI_STATUS_OK && cue.end_ms == 6000);
        UmiCaptionTrackDestroy(overlap);
    } else if (!strcmp(mode, "merge-capacity")) {
        UmiCaptionCue full = cues[0]; memset(full.text, 'x', sizeof(full.text)-1U); full.text[sizeof(full.text)-1U]='\0';
        UmiCaptionTrack *large = NULL;
        CHECK(UmiCaptionTrackReplace(source, 0, &full, &large) == UMI_STATUS_OK);
        CHECK(UmiCaptionTrackMerge(large, 0, &result) == UMI_STATUS_CAPACITY_EXCEEDED && result == NULL);
        UmiCaptionTrackDestroy(large);
    } else if (!strcmp(mode, "cue-capacity")) {
        /* A split needs a new row. Construct the maximum valid track on the heap
         * so this regression does not depend on the platform's thread stack. */
        UmiCaptionCue *full = calloc(UMI_CAPTION_CUE_LIMIT, sizeof(*full));
        CHECK(full != NULL);
        for (size_t i=0; i<UMI_CAPTION_CUE_LIMIT; ++i) {
            full[i].begin_ms = (uint32_t)i * 10U; full[i].end_ms = full[i].begin_ms + 8U;
            strcpy(full[i].text, "A");
        }
        UmiCaptionTrack *large = NULL;
        CHECK(UmiCaptionTrackCreate("Full", full, UMI_CAPTION_CUE_LIMIT, &large) == UMI_STATUS_OK);
        CHECK(UmiCaptionTrackSplit(large, 0, 4, "A", "B", &result) == UMI_STATUS_CAPACITY_EXCEEDED && result == NULL);
        UmiCaptionTrackDestroy(large); free(full);
    } else if (!strcmp(mode, "arguments")) {
        CHECK(UmiCaptionTrackSplit(NULL, 0, 2000, "A", "B", &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCaptionTrackMerge(NULL, 0, &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCaptionTrackMerge(source, SIZE_MAX, &result) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiCaptionTrackSplit(source, SIZE_MAX, 2000, "A", "B", &result) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiCaptionTrackMerge(source, 2, &result) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiCaptionTrackSplit(source, 0, 2000, NULL, "B", &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCaptionTrackMerge(source, 0, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        result = source;
        CHECK(UmiCaptionTrackMerge(source, 0, &result) == UMI_STATUS_INVALID_ARGUMENT && result == source);
        CHECK(UmiCaptionTrackSplit(source, 0, 2000, "A", "B", &result) == UMI_STATUS_INVALID_ARGUMENT && result == source);
        result = NULL;
    } else CHECK(false);

    /* Every success and failure leaves the source readable with its original
     * rows. Compare fields, not padding bytes in the public structure. */
    for (size_t i=0; i<3U; ++i) {
        UmiCaptionCue cue;
        CHECK(UmiCaptionTrackCue(source, i, &cue) == UMI_STATUS_OK);
        CHECK(cue.begin_ms == cues[i].begin_ms && cue.end_ms == cues[i].end_ms && !strcmp(cue.text,cues[i].text));
    }
    UmiCaptionTrackDestroy(result); UmiCaptionTrackDestroy(source);
    return 0;
}
