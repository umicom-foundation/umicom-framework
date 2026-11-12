/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_watermark_plan.c
 *
 * PURPOSE:
 *   Check captured watermark argv, retained tracks and failure atomicity without launching an encoder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../src/creative_workspace/video_render_internal.h"
#include "umicom/creative_workspace/video_soundtrack.h"
#include "umicom/creative_workspace/video_captions.h"
#include "fixture.h"

static size_t FindArgument(const UmiVideoRenderPlan *plan, const char *name)
{
    for (size_t i = 0U; i < plan->count; ++i)
        if (strcmp(plan->arguments[i], name) == 0) return i;
    return plan->count;
}
static bool Pair(const UmiVideoRenderPlan *plan, const char *key, const char *value)
{
    for (size_t i = 0U; i + 1U < plan->count; ++i)
        if (strcmp(plan->arguments[i], key) == 0 &&
            strcmp(plan->arguments[i + 1U], value) == 0) return true;
    return false;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    UmiVideoSequence sequence = Sequence();
    UmiVideoRenderPlan *plan = NULL;
    bool soundtrack = strcmp(mode, "soundtrack") == 0;
    bool captions = strcmp(mode, "captions") == 0;
    if (strcmp(mode, "six-clips") == 0) {
        for (size_t i = 1U; i < UMI_VIDEO_SEQUENCE_MAX_CLIPS; ++i) {
            UmiVideoSequenceClip clip = sequence.clips[0];
            (void)snprintf(clip.id, sizeof(clip.id), "clip-%zu", i);
            CHECK(UmiVideoSequencePut(&sequence, &clip, false) == UMI_STATUS_OK);
        }
    }
    if (strcmp(mode, "portrait") == 0) { sequence.width = 360U; sequence.height = 640U; }
    if (strcmp(mode, "small") == 0) { sequence.width = 16U; sequence.height = 16U; }
    if (soundtrack)
        CHECK(UmiVideoSoundtrackPrepare(VIDEO_PATH, VIDEO_DIRECTORY "/audio.wav",
            VIDEO_TOOL, VIDEO_DIRECTORY, &plan) == UMI_STATUS_OK);
    else if (captions)
        CHECK(UmiVideoCaptionsPrepare(VIDEO_PATH, VIDEO_DIRECTORY "/captions.srt",
            VIDEO_TOOL, VIDEO_DIRECTORY, &plan) == UMI_STATUS_OK);
    else
        CHECK(UmiVideoRenderPrepare(&sequence, VIDEO_TOOL, VIDEO_DIRECTORY, &plan) == UMI_STATUS_OK);
    char logo[UMI_PATH_CAPACITY] = VIDEO_DIRECTORY "/canonical logo.svg";
    const char *input = logo;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "null-plan") == 0) {
        CHECK(UmiVideoRenderApplyWatermark(NULL, logo) == UMI_STATUS_INVALID_ARGUMENT);
        UmiVideoRenderPlanDestroy(plan);
        return 0;
    }
    if (strcmp(mode, "duplicate") == 0) {
        CHECK(UmiVideoRenderApplyWatermark(plan, logo) == UMI_STATUS_OK);
        expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (strcmp(mode, "null-path") == 0) {
        input = NULL; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "relative") == 0) {
        input = "logo.svg"; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "parent-path") == 0) {
        input = VIDEO_DIRECTORY "/../logo.svg"; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "capacity-path") == 0) {
        memset(logo, 'x', sizeof(logo)); expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "graph-capacity") == 0) {
        memset(plan->graph, 'g', sizeof(plan->graph) - 1U);
        plan->graph[sizeof(plan->graph) - 1U] = '\0'; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "argument-capacity") == 0) {
        while (plan->count < UMI_CHANNEL_MAX_ARGUMENTS) plan->arguments[plan->count++] = "-nostdin";
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "missing-map") == 0) {
        plan->arguments[FindArgument(plan, "-map")] = "-metadata"; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "wrong-map") == 0) {
        plan->arguments[FindArgument(plan, "-map") + 1U] = "1:v:0"; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "missing-codec") == 0) {
        plan->arguments[FindArgument(plan, "-c:v")] = "-metadata"; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "dangling-input") == 0) {
        plan->arguments[plan->count - 1U] = "-i"; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "invalid-count") == 0) {
        plan->count = UMI_CHANNEL_MAX_ARGUMENTS + 1U; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "null-argument") == 0) {
        plan->arguments[0] = NULL; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "no-input") == 0) {
        plan->arguments[FindArgument(plan, "-i")] = "-metadata"; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "unterminated-graph") == 0) {
        memset(plan->graph, 'g', sizeof(plan->graph)); expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "path-punctuation") == 0) {
        strcpy(logo, VIDEO_DIRECTORY "/logo '[v];, euro-\xe2\x82\xac.svg");
    } else if (!soundtrack && !captions && strcmp(mode, "sequence") != 0 &&
               strcmp(mode, "six-clips") != 0 && strcmp(mode, "portrait") != 0 &&
               strcmp(mode, "small") != 0 && strcmp(mode, "path-captured") != 0) {
        UmiVideoRenderPlanDestroy(plan); return 2;
    }
    UmiVideoRenderPlan before = *plan;
    CHECK(UmiVideoRenderApplyWatermark(plan, input) == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(memcmp(plan, &before, sizeof(before)) == 0);
    } else {
        CHECK(plan->watermarked && plan->count <= UMI_CHANNEL_MAX_ARGUMENTS);
        CHECK(strcmp(plan->program, before.program) == 0 && strcmp(plan->directory, before.directory) == 0);
        CHECK(memcmp(&plan->sequence, &before.sequence, sizeof(plan->sequence)) == 0);
        CHECK(strcmp(plan->watermark_path, logo) == 0);
        strcpy(logo, VIDEO_DIRECTORY "/changed.svg");
        CHECK(strcmp(plan->watermark_path, logo) != 0);
        CHECK(Pair(plan, "-i", plan->watermark_path));
        CHECK(Pair(plan, "-c:v", "librsvg") && Pair(plan, "-c:v", "libx264"));
        CHECK(Pair(plan, "-f", "image2pipe") && Pair(plan, "-f", "mp4"));
        CHECK(Pair(plan, "-map", "[umi_branded]"));
        CHECK(Pair(plan, "-pix_fmt", "yuv420p"));
        CHECK(strstr(plan->graph, plan->watermark_path) == NULL);
        CHECK(strstr(plan->graph, "rw/5") && strstr(plan->graph, "rh/5*dar"));
        CHECK(strstr(plan->graph, "eof_action=repeat:repeatlast=1:shortest=0"));
        CHECK(strcmp(plan->arguments[plan->count - 1U], "pipe:1") == 0);
        for (size_t i = 0U; i < plan->count; ++i) {
            CHECK(plan->arguments[i] != NULL);
            if (strcmp(plan->arguments[i], "-filter_complex") == 0)
                CHECK(plan->arguments[i + 1U] == plan->graph);
        }
        if (soundtrack) CHECK(Pair(plan, "-map", "1:a:0") && Pair(plan, "-af", "apad"));
        if (captions) CHECK(Pair(plan, "-map", "0:a:0?") && Pair(plan, "-map", "1:s:0") &&
                            Pair(plan, "-c:s", "mov_text"));
        if (!soundtrack && !captions) {
            CHECK(strstr(plan->graph, "trim=duration=4.000") != NULL);
            CHECK(Pair(plan, "-i", VIDEO_PATH));
        }
        if (strcmp(mode, "six-clips") == 0) CHECK(plan->count == 63U);
    }
    UmiVideoRenderPlanDestroy(plan);
    return 0;
}
