/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/video_render_internal.h
 * PURPOSE: Own render arguments so asynchronous hosts cannot change an accepted job.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_VIDEO_RENDER_INTERNAL_H
#define UMICOM_CREATIVE_VIDEO_RENDER_INTERNAL_H
#include "umicom/creative_workspace/video_render.h"
struct UmiVideoRenderPlan {
  UmiVideoSequence sequence;
  char program[UMI_PATH_CAPACITY], directory[UMI_PATH_CAPACITY];
  char starts[UMI_VIDEO_SEQUENCE_MAX_CLIPS][24];
  char graph[4096];
  char source_paths[2][UMI_PATH_CAPACITY];
  const char *arguments[UMI_CHANNEL_MAX_ARGUMENTS];
  size_t count;
};
#endif
