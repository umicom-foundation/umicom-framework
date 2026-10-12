/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_plan.c
 * PURPOSE: Inspect captured argv for bounded inputs and absence of path interpolation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../src/creative_workspace/video_render_internal.h"
#include "fixture.h"
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  UmiVideoSequence sequence = Sequence();
  UmiVideoRenderPlan *plan = NULL;
  if (strcmp(argv[1], "limits") == 0) {
    sequence.clip_count = 0U;
    CHECK(UmiVideoRenderPrepare(&sequence, VIDEO_TOOL, VIDEO_DIRECTORY, &plan) != UMI_STATUS_OK);
    sequence = Sequence();
    for (size_t i = 1U; i < UMI_VIDEO_SEQUENCE_MAX_CLIPS; ++i) {
      UmiVideoSequenceClip clip = sequence.clips[0];
      (void)snprintf(clip.id, sizeof(clip.id), "clip-%zu", i);
      CHECK(UmiVideoSequencePut(&sequence, &clip, false) == UMI_STATUS_OK);
    }
    CHECK(UmiVideoRenderPrepare(&sequence, VIDEO_TOOL, VIDEO_DIRECTORY, &plan) == UMI_STATUS_OK);
    CHECK(plan->count <= UMI_CHANNEL_MAX_ARGUMENTS);
    CHECK(strstr(plan->graph, "concat=n=6:v=1:a=0") != NULL);
  } else if (strcmp(argv[1], "arguments") == 0 || strcmp(argv[1], "captured") == 0) {
    CHECK(UmiVideoRenderPrepare(&sequence, VIDEO_TOOL, VIDEO_DIRECTORY, &plan) == UMI_STATUS_OK);
    strcpy(sequence.clips[0].path, VIDEO_DIRECTORY "/changed.mp4");
    bool input = false, output = false;
    for (size_t i = 0U; i < plan->count; ++i) {
      if (strcmp(plan->arguments[i], "-i") == 0) {
        CHECK(i + 1U < plan->count && strcmp(plan->arguments[i + 1U], VIDEO_PATH) == 0);
        input = true;
      }
      if (strcmp(plan->arguments[i], "pipe:1") == 0)
        output = true;
    }
    CHECK(input && output && strstr(plan->graph, VIDEO_PATH) == NULL);
    CHECK(strstr(plan->graph, "trim=duration=4.000") != NULL);
  } else
    return 2;
  UmiVideoRenderPlanDestroy(plan);
  return 0;
}
