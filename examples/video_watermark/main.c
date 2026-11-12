/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/video_watermark/main.c
 *
 * PURPOSE:
 *   Build a branded video plan through an installed Framework SDK and execute it only on explicit request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/video_render.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    bool self = argc == 2 && (strcmp(argv[1], "self-check") == 0 ||
        strcmp(argv[1], "reject-relative") == 0 || strcmp(argv[1], "duplicate") == 0);
    if (!self && (argc != 6 || (strcmp(argv[1], "review") != 0 && strcmp(argv[1], "render") != 0))) {
        fputs("Umicom Framework video watermark example\n"
              "Usage: video-watermark review|render FFMPEG INPUT SVG OUTPUT\n"
              "       video-watermark self-check|reject-relative|duplicate\n", stderr);
        return 2;
    }
#ifdef _WIN32
    const char *tool = self ? "C:/practice/ffmpeg.exe" : argv[2];
    const char *input = self ? "C:/practice/input.mp4" : argv[3];
    const char *logo = self ? "C:/practice/umicom-logo-on-dark.svg" : argv[4];
    const char *output = self ? "C:/practice/branded.mp4" : argv[5];
#else
    const char *tool = self ? "/practice/ffmpeg" : argv[2];
    const char *input = self ? "/practice/input.mp4" : argv[3];
    const char *logo = self ? "/practice/umicom-logo-on-dark.svg" : argv[4];
    const char *output = self ? "/practice/branded.mp4" : argv[5];
#endif
    char directory[UMI_PATH_CAPACITY];
    UmiStatus status = UmiOutputFileValidatePath(output);
    if (status == UMI_STATUS_OK) status = umi_path_parent(output, directory, sizeof(directory));
    UmiVideoSequence sequence;
    if (status == UMI_STATUS_OK) status = UmiVideoSequenceInit("First second", 640U, 360U, &sequence);
    UmiVideoSequenceClip clip = {0};
    strcpy(clip.id, "source");
    if (strlen(input) >= sizeof(clip.path)) status = UMI_STATUS_CAPACITY_EXCEEDED;
    else strcpy(clip.path, input);
    clip.end_ms = 1000U;
    if (status == UMI_STATUS_OK) status = UmiVideoSequencePut(&sequence, &clip, false);
    UmiVideoRenderPlan *plan = NULL;
    if (status == UMI_STATUS_OK) status = UmiVideoRenderPrepare(&sequence, tool, directory, &plan);
    if (status == UMI_STATUS_OK) {
        status = UmiVideoRenderApplyWatermark(plan,
            self && strcmp(argv[1], "reject-relative") == 0 ? "logo.svg" : logo);
        if (self && strcmp(argv[1], "reject-relative") == 0) {
            UmiVideoRenderPlanDestroy(plan);
            return status == UMI_STATUS_INVALID_ARGUMENT ? 0 : 1;
        }
    }
    if (status == UMI_STATUS_OK && self && strcmp(argv[1], "duplicate") == 0) {
        status = UmiVideoRenderApplyWatermark(plan, logo);
        UmiVideoRenderPlanDestroy(plan);
        return status == UMI_STATUS_ALREADY_EXISTS ? 0 : 1;
    }
    /* Review ends before any file access. Explicit render uses the existing
     * Framework output owner and never replaces an existing destination. */
    if (status == UMI_STATUS_OK && !self && strcmp(argv[1], "render") == 0) {
        UmiVideoRenderReport report;
        status = UmiVideoRenderExecute(plan, output, 30000U, NULL, &report);
        printf("Umicom render: %s; created=%d; completed=%d\n", umi_status_text(status),
            (int)report.output_created, (int)report.completed);
        if (report.process.diagnostics[0] != '\0') fputs(report.process.diagnostics, stderr);
        puts("Inspect the MP4 in a player; process completion does not verify artwork or playback.");
    } else if (status == UMI_STATUS_OK) {
        puts("Umicom watermark plan accepted. No files opened and no encoder started.");
    }
    if (status != UMI_STATUS_OK) fprintf(stderr, "Umicom watermark: %s\n", umi_status_text(status));
    UmiVideoRenderPlanDestroy(plan);
    return status == UMI_STATUS_OK ? 0 : 1;
}
