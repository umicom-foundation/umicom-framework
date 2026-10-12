/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_soundtrack.c
 * PURPOSE: Check immutable soundtrack capture and explicit stream-selection policy.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../src/creative_workspace/video_render_internal.h"
#include "fixture.h"
#include "umicom/creative_workspace/video_soundtrack.h"

int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  UmiVideoRenderPlan *plan = NULL;
  if (strcmp(argv[1], "invalid") == 0) {
    CHECK(UmiVideoSoundtrackPrepare("relative.mp4", VIDEO_PATH, VIDEO_TOOL, VIDEO_DIRECTORY,
                                    &plan) != UMI_STATUS_OK);
    CHECK(plan == NULL);
    CHECK(UmiVideoSoundtrackPrepare(VIDEO_PATH, "https://example.invalid/audio", VIDEO_TOOL,
                                    VIDEO_DIRECTORY, &plan) != UMI_STATUS_OK);
    CHECK(plan == NULL);
    CHECK(UmiVideoSoundtrackPrepare(VIDEO_PATH, VIDEO_PATH, VIDEO_TOOL, "relative", &plan) !=
          UMI_STATUS_OK);
    CHECK(plan == NULL);
    CHECK(UmiVideoSoundtrackPrepare(VIDEO_PATH, VIDEO_PATH, VIDEO_TOOL, VIDEO_DIRECTORY, NULL) !=
          UMI_STATUS_OK);
  } else if (strcmp(argv[1], "arguments") == 0) {
    char video[UMI_PATH_CAPACITY] = VIDEO_DIRECTORY "/a ; clip.mp4";
    char audio[UMI_PATH_CAPACITY] = VIDEO_DIRECTORY "/song ; one.wav";
    CHECK(UmiVideoSoundtrackPrepare(video, audio, VIDEO_TOOL, VIDEO_DIRECTORY, &plan) ==
          UMI_STATUS_OK);
    video[0] = 'X';
    audio[0] = 'X';
    size_t inputs = 0U, mappings = 0U;
    bool copyVideo = false, pad = false, shortest = false;
    for (size_t i = 0U; i < plan->count; ++i) {
      if (strcmp(plan->arguments[i], "-i") == 0) {
        CHECK(i + 1U < plan->count);
        CHECK(strcmp(plan->arguments[i + 1U], inputs == 0U ? VIDEO_DIRECTORY "/a ; clip.mp4"
                                                           : VIDEO_DIRECTORY
                                                  "/song ; one.wav") == 0);
        ++inputs;
      }
      if (strcmp(plan->arguments[i], "-map") == 0) {
        CHECK(i + 1U < plan->count);
        CHECK(strcmp(plan->arguments[i + 1U], mappings == 0U ? "0:v:0" : "1:a:0") == 0);
        ++mappings;
      }
      if (strcmp(plan->arguments[i], "-c:v") == 0)
        copyVideo = i + 1U < plan->count && strcmp(plan->arguments[i + 1U], "copy") == 0;
      if (strcmp(plan->arguments[i], "-af") == 0)
        pad = i + 1U < plan->count && strcmp(plan->arguments[i + 1U], "apad") == 0;
      if (strcmp(plan->arguments[i], "-shortest") == 0)
        shortest = true;
    }
    CHECK(inputs == 2U && mappings == 2U && copyVideo && pad && shortest);
    CHECK(strcmp(plan->arguments[plan->count - 1U], "pipe:1") == 0);
    CHECK(UmiVideoSoundtrackPrepare(VIDEO_PATH, VIDEO_PATH, VIDEO_TOOL, VIDEO_DIRECTORY, &plan) ==
          UMI_STATUS_INVALID_STATE);
    UmiVideoRenderPlanDestroy(plan);
  } else
    return 2;
  return 0;
}
