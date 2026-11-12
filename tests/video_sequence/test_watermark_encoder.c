/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_watermark_encoder.c
 *
 * PURPOSE:
 *   Decode branded exports to check every frame and retained audio/caption streams with an explicitly selected encoder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Reuse the existing real-encoder fixture's owned source, subtitle and audio
 * helpers. Its entry point remains available for review and ordinary coverage;
 * this executable adds independent pixel checks for the branded paths. */
int UnbrandedEncoderEntry(int argc, char **argv);
#define main UnbrandedEncoderEntry
#include "test_encoder.c"
#undef main

typedef struct MarkPixels {
    uint64_t bytes, frames, marked_frames, blue_centres;
    unsigned char pixel[3];
    unsigned white, red;
} MarkPixels;
static void InspectMark(const char *bytes, size_t size, void *context)
{
    MarkPixels *p = context;
    for (size_t i = 0U; i < size; ++i) {
        p->pixel[p->bytes % 3U] = (unsigned char)bytes[i];
        ++p->bytes;
        if (p->bytes % 3U != 0U) continue;
        uint64_t index = (p->bytes / 3U - 1U) % (256U * 144U);
        unsigned x = (unsigned)(index % 256U), y = (unsigned)(index / 256U);
        if (x >= 192U && y >= 108U) {
            if (p->pixel[0] > 170U && p->pixel[1] > 170U && p->pixel[2] > 170U) ++p->white;
            if (p->pixel[0] > 80U && p->pixel[0] > p->pixel[1] * 2U &&
                p->pixel[0] > p->pixel[2] * 2U) ++p->red;
        }
        if (x == 128U && y == 72U && p->pixel[2] > 180U &&
            p->pixel[0] < 60U && p->pixel[1] < 60U) ++p->blue_centres;
        if (index == 256U * 144U - 1U) {
            ++p->frames;
            if (p->white >= 4U && p->red >= 1U) ++p->marked_frames;
            p->white = p->red = 0U;
        }
    }
}
static void Render(UmiVideoRenderPlan *plan, const char *output, const char *logo)
{
    UmiVideoRenderReport report;
    if (logo != NULL) CHECK(UmiVideoRenderApplyWatermark(plan, logo) == UMI_STATUS_OK);
    CHECK(UmiVideoRenderExecute(plan, output, 30000U, NULL, &report) == UMI_STATUS_OK);
    CHECK(report.completed);
    UmiVideoRenderPlanDestroy(plan);
}
int main(int argc, char **argv)
{
    if (argc != 4) return 2;
    const char *mode = argv[1], *tool = argv[2], *logo = argv[3];
    CHECK(strcmp(mode, "sequence") == 0 || strcmp(mode, "soundtrack") == 0 ||
          strcmp(mode, "captions") == 0);
    char directory[UMI_PATH_CAPACITY], source[UMI_PATH_CAPACITY], base[UMI_PATH_CAPACITY],
         extra[UMI_PATH_CAPACITY], output[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(source, directory, "blue source.mkv");
    FixturePath(base, directory, "base.mp4");
    FixturePath(extra, directory, strcmp(mode, "soundtrack") == 0 ? "tone.wav" : "caption.srt");
    FixturePath(output, directory, "branded.mp4");
    Source(tool, directory, "blue", source);
    UmiVideoSequence sequence;
    CHECK(UmiVideoSequenceInit("Canonical logo acceptance", 256U, 144U, &sequence) == UMI_STATUS_OK);
    UmiVideoSequenceClip clip = {0};
    strcpy(clip.id, "blue"); strcpy(clip.path, source); clip.end_ms = 1000U;
    CHECK(UmiVideoSequencePut(&sequence, &clip, false) == UMI_STATUS_OK);
    UmiVideoRenderPlan *plan = NULL;
    CHECK(UmiVideoRenderPrepare(&sequence, tool, directory, &plan) == UMI_STATUS_OK);
    if (strcmp(mode, "sequence") != 0) {
        Render(plan, base, NULL); plan = NULL;
        if (strcmp(mode, "soundtrack") == 0) {
            const char *args[] = {"-nostdin", "-v", "error", "-n", "-f", "lavfi", "-i",
                "sine=frequency=440:sample_rate=8000:duration=1", "-c:a", "pcm_s16le", extra};
            UmiProcessRequest request = {0}; UmiProcessResult result;
            request.program = tool; request.working_directory = directory;
            request.arguments = args; request.argument_count = sizeof(args) / sizeof(args[0]);
            request.capture_stderr = 1; request.timeout_ms = 15000U;
            request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
            CHECK(umi_process_execute(&request, &result) == UMI_STATUS_OK && result.exit_code == 0);
            CHECK(UmiVideoSoundtrackPrepare(base, extra, tool, directory, &plan) == UMI_STATUS_OK);
        } else {
            static const char text[] = "1\n00:00:00,100 --> 00:00:00,800\nCanonical identity\n\n";
            UmiOutputFile *file = NULL;
            CHECK(UmiOutputFileCreate(extra, &file) == UMI_STATUS_OK);
            CHECK(UmiOutputFileWrite(file, text, sizeof(text) - 1U) == UMI_STATUS_OK);
            CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK); UmiOutputFileDestroy(file);
            CHECK(UmiVideoCaptionsPrepare(base, extra, tool, directory, &plan) == UMI_STATUS_OK);
        }
    }
    Render(plan, output, logo);
    const char *args[] = {"-nostdin", "-v", "error", "-i", output, "-map", "0:v:0",
        "-an", "-sn", "-f", "rawvideo", "-pix_fmt", "rgb24", "pipe:1"};
    UmiProcessRequest request = {0}; UmiProcessResult result;
    request.program = tool; request.working_directory = directory;
    request.arguments = args; request.argument_count = sizeof(args) / sizeof(args[0]);
    request.capture_stdout = 1; request.timeout_ms = 15000U;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    MarkPixels pixels = {0};
    CHECK(UmiProcessExecuteStreamed(&request, InspectMark, &pixels, &result) == UMI_STATUS_OK &&
          result.exit_code == 0);
    CHECK(pixels.bytes == UINT64_C(30) * 256U * 144U * 3U);
    CHECK(pixels.frames == 30U && pixels.marked_frames == 30U && pixels.blue_centres == 30U);
    if (strcmp(mode, "captions") == 0) {
        const char *sub[] = {"-nostdin", "-v", "error", "-i", output, "-map", "0:s:0", "-f", "srt", "pipe:1"};
        request.arguments = sub; request.argument_count = sizeof(sub) / sizeof(sub[0]);
        CHECK(umi_process_execute(&request, &result) == UMI_STATUS_OK &&
              result.exit_code == 0 && !result.output_truncated);
        CHECK(strstr(result.output, "Canonical identity") != NULL);
    } else if (strcmp(mode, "soundtrack") == 0) {
        const char *audio[] = {"-nostdin", "-v", "error", "-i", output, "-map", "0:a:0",
            "-vn", "-sn", "-ar", "8000", "-ac", "1", "-f", "s16le", "pipe:1"};
        request.arguments = audio; request.argument_count = sizeof(audio) / sizeof(audio[0]);
        Samples samples = {0};
        CHECK(UmiProcessExecuteStreamed(&request, AudioCapture, &samples, &result) == UMI_STATUS_OK &&
              result.exit_code == 0);
        CHECK(samples.active_count == 5600U && samples.active_sum / samples.active_count > 100U);
    }
    return 0;
}
