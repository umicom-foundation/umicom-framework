/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_encoder.c
 * PURPOSE: Qualify a real FFmpeg render by decoding two ordered colour segments.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#undef CHECK
#include "fixture.h"
#include "umicom/creative_workspace/captions.h"
#include "umicom/creative_workspace/video_captions.h"
#include "umicom/creative_workspace/video_render.h"
#include "umicom/creative_workspace/video_soundtrack.h"
#include "umicom/platform/process.h"
typedef struct Pixels {
  uint64_t count;
  unsigned char first[3], last[3];
} Pixels;
static void Capture(const char *bytes, size_t size, void *context) {
  Pixels *pixels = context;
  for (size_t i = 0U; i < size; ++i) {
    if (pixels->count < 3U)
      pixels->first[pixels->count] = (unsigned char)bytes[i];
    pixels->last[pixels->count % 3U] = (unsigned char)bytes[i];
    ++pixels->count;
  }
}
static void Source(const char *tool, const char *directory, const char *colour, const char *path) {
  char filter[128];
  (void)snprintf(filter, sizeof(filter), "color=c=%s:s=48x32:r=30:d=1", colour);
  const char *args[] = {"-nostdin", "-v",  "error", "-n",   "-f", "lavfi",    "-i",
                        filter,     "-an", "-c:v",  "ffv1", "-f", "matroska", path};
  UmiProcessRequest request = {0};
  request.program = tool;
  request.working_directory = directory;
  request.arguments = args;
  request.argument_count = sizeof(args) / sizeof(args[0]);
  request.capture_stdout = 1;
  request.capture_stderr = 1;
  request.timeout_ms = 15000U;
  request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
  UmiProcessResult result;
  CHECK(umi_process_execute(&request, &result) == UMI_STATUS_OK && result.exit_code == 0);
}
typedef struct Samples {
  uint64_t bytes, active_sum, quiet_sum, active_count, quiet_count;
  unsigned char low;
} Samples;
/* Count PCM windows independently of AAC frame padding. The tone must be
 * audible early and the padded portion near the end must be effectively quiet. */
static void AudioCapture(const char *bytes, size_t size, void *context) {
  Samples *samples = context;
  for (size_t i = 0U; i < size; ++i) {
    if (samples->bytes % 2U == 0U)
      samples->low = (unsigned char)bytes[i];
    else {
      unsigned raw = samples->low | ((unsigned)(unsigned char)bytes[i] << 8U);
      int value = raw < 32768U ? (int)raw : (int)raw - 65536;
      uint64_t magnitude = (uint64_t)(value < 0 ? -value : value);
      uint64_t index = samples->bytes / 2U;
      if (index >= 800U && index < 6400U) {
        samples->active_sum += magnitude;
        ++samples->active_count;
      }
      if (index >= 12000U && index < 14400U) {
        samples->quiet_sum += magnitude;
        ++samples->quiet_count;
      }
    }
    ++samples->bytes;
  }
}
/* Decode the exported track back to SRT independently of the mux operation.
 * A container header alone cannot establish that captions survived encoding. */
static void Captions(const char *tool, const char *directory, const char *video, bool hasAudio) {
  char subtitles[UMI_PATH_CAPACITY], output[UMI_PATH_CAPACITY];
  FixturePath(subtitles, directory, hasAudio ? "sound captions.srt" : "silent captions.srt");
  FixturePath(output, directory, hasAudio ? "captioned sound.mp4" : "captioned silent.mp4");
  static const char content[] = "1\n00:00:00,250 --> 00:00:00,750\nFirst colour\n\n"
                                "2\n00:00:01,250 --> 00:00:01,750\nSecond colour\n\n";
  UmiOutputFile *file = NULL;
  CHECK(UmiOutputFileCreate(subtitles, &file) == UMI_STATUS_OK);
  CHECK(UmiOutputFileWrite(file, content, sizeof(content) - 1U) == UMI_STATUS_OK);
  CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK);
  UmiOutputFileDestroy(file);
  UmiVideoRenderPlan *plan = NULL;
  UmiVideoRenderReport report;
  CHECK(UmiVideoCaptionsPrepare(video, subtitles, tool, directory, &plan) == UMI_STATUS_OK);
  CHECK(UmiVideoRenderExecute(plan, output, 30000U, NULL, &report) == UMI_STATUS_OK &&
        report.completed);
  UmiVideoRenderPlanDestroy(plan);
  const char *args[] = {"-nostdin", "-v",    "error", "-i",  output,
                        "-map",     "0:s:0", "-f",    "srt", "pipe:1"};
  UmiProcessRequest request = {0};
  UmiProcessResult result;
  request.program = tool;
  request.working_directory = directory;
  request.arguments = args;
  request.argument_count = sizeof(args) / sizeof(args[0]);
  request.capture_stdout = 1;
  request.timeout_ms = 15000U;
  request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
  CHECK(umi_process_execute(&request, &result) == UMI_STATUS_OK && result.exit_code == 0 &&
        !result.output_truncated);
  UmiCaptionTrack *track = NULL;
  CHECK(UmiCaptionTrackParseSrt("Decoded captions", result.output, strlen(result.output), &track) ==
        UMI_STATUS_OK);
  UmiCaptionInfo info;
  CHECK(UmiCaptionTrackInspect(track, &info) == UMI_STATUS_OK && info.cue_count == 2U);
  UmiCaptionCue cue;
  for (size_t index = 0U; index < 2U; ++index) {
    CHECK(UmiCaptionTrackCue(track, index, &cue) == UMI_STATUS_OK);
    CHECK(strcmp(cue.text, index == 0U ? "First colour" : "Second colour") == 0);
    uint32_t expectedBegin = index == 0U ? 250U : 1250U;
    /* MP4 may shift initial timestamps to accommodate video decode delay.
     * Bound that shift, and verify the half-second cue durations separately. */
    CHECK(cue.begin_ms >= expectedBegin - 100U && cue.begin_ms <= expectedBegin + 100U);
    CHECK(cue.end_ms - cue.begin_ms >= 480U && cue.end_ms - cue.begin_ms <= 520U);
  }
  UmiCaptionTrackDestroy(track);
  const char *pixelArgs[] = {"-nostdin", "-v",  "error", "-i",       output,     "-map",  "0:v:0",
                             "-an",      "-sn", "-f",    "rawvideo", "-pix_fmt", "rgb24", "pipe:1"};
  request.arguments = pixelArgs;
  request.argument_count = sizeof(pixelArgs) / sizeof(pixelArgs[0]);
  Pixels pixels = {0};
  CHECK(UmiProcessExecuteStreamed(&request, Capture, &pixels, &result) == UMI_STATUS_OK &&
        result.exit_code == 0);
  CHECK(pixels.count == UINT64_C(60) * 48U * 32U * 3U);
  CHECK(pixels.first[0] > 200U && pixels.last[2] > 200U);
  if (hasAudio) {
    const char *audioArgs[] = {"-nostdin", "-v",  "error", "-i",    output, "-map",
                               "0:a:0",    "-vn", "-sn",   "-ar",   "8000", "-ac",
                               "1",        "-f",  "s16le", "pipe:1"};
    request.arguments = audioArgs;
    request.argument_count = sizeof(audioArgs) / sizeof(audioArgs[0]);
    Samples samples = {0};
    CHECK(UmiProcessExecuteStreamed(&request, AudioCapture, &samples, &result) == UMI_STATUS_OK &&
          result.exit_code == 0);
    CHECK(samples.bytes >= 32000U && samples.bytes <= 36864U);
    CHECK(samples.active_count == 5600U && samples.active_sum / samples.active_count > 100U);
  }
}
static void Soundtrack(const char *tool, const char *directory, const char *video) {
  char audio[UMI_PATH_CAPACITY], output[UMI_PATH_CAPACITY];
  FixturePath(audio, directory, "tone.wav");
  FixturePath(output, directory, "with sound.mp4");
  const char *sourceArgs[] = {
      "-nostdin", "-v",        "error", "-n",
      "-f",       "lavfi",     "-i",    "sine=frequency=440:sample_rate=8000:duration=1",
      "-c:a",     "pcm_s16le", audio};
  UmiProcessRequest request = {0};
  UmiProcessResult result;
  request.program = tool;
  request.working_directory = directory;
  request.arguments = sourceArgs;
  request.argument_count = sizeof(sourceArgs) / sizeof(sourceArgs[0]);
  request.capture_stdout = 1;
  request.capture_stderr = 1;
  request.timeout_ms = 15000U;
  request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
  CHECK(umi_process_execute(&request, &result) == UMI_STATUS_OK && result.exit_code == 0);
  UmiVideoRenderPlan *plan = NULL;
  UmiVideoRenderReport report;
  CHECK(UmiVideoSoundtrackPrepare(video, audio, tool, directory, &plan) == UMI_STATUS_OK);
  CHECK(UmiVideoRenderExecute(plan, output, 30000U, NULL, &report) == UMI_STATUS_OK &&
        report.completed);
  UmiVideoRenderPlanDestroy(plan);
  const char *decodeArgs[] = {"-nostdin", "-v",   "error", "-i", output, "-map",  "0:a:0", "-vn",
                              "-ar",      "8000", "-ac",   "1",  "-f",   "s16le", "pipe:1"};
  request.arguments = decodeArgs;
  request.argument_count = sizeof(decodeArgs) / sizeof(decodeArgs[0]);
  request.capture_stderr = 0;
  Samples samples = {0};
  CHECK(UmiProcessExecuteStreamed(&request, AudioCapture, &samples, &result) == UMI_STATUS_OK &&
        result.exit_code == 0);
  CHECK(samples.bytes >= 32000U && samples.bytes <= 36864U);
  CHECK(samples.active_count == 5600U && samples.active_sum / samples.active_count > 100U);
  CHECK(samples.quiet_count == 2400U && samples.quiet_sum / samples.quiet_count < 30U);
  /* Video copying must preserve both segments and the total decoded frame count. */
  const char *pixelArgs[] = {"-nostdin", "-v", "error",    "-i",       output,  "-map",  "0:v:0",
                             "-an",      "-f", "rawvideo", "-pix_fmt", "rgb24", "pipe:1"};
  request.arguments = pixelArgs;
  request.argument_count = sizeof(pixelArgs) / sizeof(pixelArgs[0]);
  Pixels pixels = {0};
  CHECK(UmiProcessExecuteStreamed(&request, Capture, &pixels, &result) == UMI_STATUS_OK &&
        result.exit_code == 0);
  CHECK(pixels.count == UINT64_C(60) * 48U * 32U * 3U);
  CHECK(pixels.first[0] > 200U && pixels.last[2] > 200U);
  Captions(tool, directory, output, true);
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  char directory[UMI_PATH_CAPACITY], red[UMI_PATH_CAPACITY], blue[UMI_PATH_CAPACITY],
      output[UMI_PATH_CAPACITY];
  FixtureDirectory(directory);
  FixturePath(red, directory, "red clip.mkv");
  FixturePath(blue, directory, "blue clip.mkv");
  FixturePath(output, directory, "sequence.mp4");
  Source(argv[1], directory, "red", red);
  Source(argv[1], directory, "blue", blue);
  UmiVideoSequence sequence;
  CHECK(UmiVideoSequenceInit("Colour order", 48U, 32U, &sequence) == UMI_STATUS_OK);
  UmiVideoSequenceClip clip = {0};
  strcpy(clip.id, "red");
  strcpy(clip.path, red);
  clip.end_ms = 1000U;
  CHECK(UmiVideoSequencePut(&sequence, &clip, false) == UMI_STATUS_OK);
  strcpy(clip.id, "blue");
  strcpy(clip.path, blue);
  CHECK(UmiVideoSequencePut(&sequence, &clip, false) == UMI_STATUS_OK);
  UmiVideoRenderPlan *plan = NULL;
  UmiVideoRenderReport report;
  CHECK(UmiVideoRenderPrepare(&sequence, argv[1], directory, &plan) == UMI_STATUS_OK);
  CHECK(UmiVideoRenderExecute(plan, output, 30000U, NULL, &report) == UMI_STATUS_OK &&
        report.completed);
  UmiVideoRenderPlanDestroy(plan);
  /* Decode independently of the renderer. Check duration, frame dimensions
   * and segment order, not merely that an MP4-shaped file was created. */
  const char *args[] = {"-nostdin", "-v", "error",    "-i",       output,  "-map",  "0:v:0",
                        "-an",      "-f", "rawvideo", "-pix_fmt", "rgb24", "pipe:1"};
  UmiProcessRequest request = {0};
  UmiProcessResult result;
  request.program = argv[1];
  request.working_directory = directory;
  request.arguments = args;
  request.argument_count = sizeof(args) / sizeof(args[0]);
  request.capture_stdout = 1;
  request.capture_stderr = 0;
  request.timeout_ms = 30000U;
  request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
  Pixels pixels = {0};
  CHECK(UmiProcessExecuteStreamed(&request, Capture, &pixels, &result) == UMI_STATUS_OK &&
        result.exit_code == 0);
  CHECK(pixels.count == UINT64_C(60) * 48U * 32U * 3U);
  CHECK(pixels.first[0] > 200U && pixels.first[1] < 50U && pixels.first[2] < 50U);
  CHECK(pixels.last[2] > 200U && pixels.last[0] < 50U && pixels.last[1] < 50U);
  Captions(argv[1], directory, output, false);
  Soundtrack(argv[1], directory, output);
  return 0;
}
