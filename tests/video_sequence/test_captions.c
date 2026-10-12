/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_captions.c
 * PURPOSE: Check caption stream selection, captured path ownership and rejected export inputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../src/creative_workspace/video_render_internal.h"
#include "fixture.h"
#include "umicom/creative_workspace/video_captions.h"

int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  UmiVideoRenderPlan *plan = NULL;
  if (strcmp(argv[1], "invalid") == 0) {
    CHECK(UmiVideoCaptionsPrepare("relative.mp4", VIDEO_PATH, VIDEO_TOOL, VIDEO_DIRECTORY, &plan) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiVideoCaptionsPrepare(VIDEO_PATH, "https://example.invalid/captions.srt", VIDEO_TOOL,
                                  VIDEO_DIRECTORY, &plan) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiVideoCaptionsPrepare(VIDEO_PATH, VIDEO_PATH, VIDEO_TOOL, "relative", &plan) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiVideoCaptionsPrepare(VIDEO_PATH, VIDEO_PATH, "ffmpeg", VIDEO_DIRECTORY, &plan) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(plan == NULL);
  } else if (strcmp(argv[1], "arguments") == 0) {
    char video[UMI_PATH_CAPACITY] = VIDEO_DIRECTORY "/clip ; one.mp4";
    char text[UMI_PATH_CAPACITY] = VIDEO_DIRECTORY "/captions ; one.srt";
    CHECK(UmiVideoCaptionsPrepare(video, text, VIDEO_TOOL, VIDEO_DIRECTORY, &plan) ==
          UMI_STATUS_OK);
    video[0] = 'X';
    text[0] = 'X';
    size_t inputs = 0U, mappings = 0U, formats = 0U, protocols = 0U;
    bool textCodec = false, copyVideo = false, copyAudio = false;
    const char *expectedMappings[] = {"0:v:0", "0:a:0?", "1:s:0"};
    for (size_t index = 0U; index < plan->count; ++index) {
      const char *argument = plan->arguments[index];
      CHECK(strcmp(argument, "-shortest") != 0);
      CHECK(strcmp(argument, "-sn") != 0);
      if (strcmp(argument, "-i") == 0) {
        CHECK(index + 1U < plan->count && inputs < 2U);
        CHECK(strcmp(plan->arguments[index + 1U], inputs == 0U ? VIDEO_DIRECTORY "/clip ; one.mp4"
                                                               : VIDEO_DIRECTORY
                                                      "/captions ; one.srt") == 0);
        ++inputs;
      } else if (strcmp(argument, "-map") == 0) {
        CHECK(index + 1U < plan->count && mappings < 3U);
        CHECK(strcmp(plan->arguments[index + 1U], expectedMappings[mappings++]) == 0);
      } else if (strcmp(argument, "-protocol_whitelist") == 0) {
        CHECK(index + 1U < plan->count && strcmp(plan->arguments[index + 1U], "file") == 0);
        ++protocols;
      } else if (strcmp(argument, "-f") == 0) {
        CHECK(index + 1U < plan->count && formats < 2U);
        CHECK(strcmp(plan->arguments[index + 1U], formats++ == 0U ? "srt" : "mp4") == 0);
      } else if (strcmp(argument, "-c:s") == 0)
        textCodec =
            index + 1U < plan->count && strcmp(plan->arguments[index + 1U], "mov_text") == 0;
      else if (strcmp(argument, "-c:v") == 0)
        copyVideo = index + 1U < plan->count && strcmp(plan->arguments[index + 1U], "copy") == 0;
      else if (strcmp(argument, "-c:a") == 0)
        copyAudio = index + 1U < plan->count && strcmp(plan->arguments[index + 1U], "copy") == 0;
    }
    CHECK(inputs == 2U && mappings == 3U && formats == 2U && protocols == 2U);
    CHECK(textCodec && copyVideo && copyAudio &&
          strcmp(plan->arguments[plan->count - 1U], "pipe:1") == 0);
    CHECK(UmiVideoCaptionsPrepare(VIDEO_PATH, VIDEO_PATH, VIDEO_TOOL, VIDEO_DIRECTORY, &plan) ==
          UMI_STATUS_INVALID_STATE);
    UmiVideoRenderPlanDestroy(plan);
  } else
    return 2;
  return 0;
}
