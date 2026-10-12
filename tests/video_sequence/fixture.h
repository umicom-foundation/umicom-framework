/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/fixture.h
 * PURPOSE: Build small deterministic edit fixtures without opening media files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VIDEO_SEQUENCE_TEST_FIXTURE_H
#define UMICOM_VIDEO_SEQUENCE_TEST_FIXTURE_H
#include "umicom/creative_workspace/video_sequence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition)                                                                           \
  do {                                                                                             \
    if (!(condition)) {                                                                            \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition);                              \
      exit(1);                                                                                     \
    }                                                                                              \
  } while (0)
#ifdef _WIN32
#define VIDEO_PATH "C:/video clips/input.mp4"
#define VIDEO_TOOL "C:/tools/ffmpeg.exe"
#define VIDEO_DIRECTORY "C:/video clips"
#else
#define VIDEO_PATH "/video clips/input.mp4"
#define VIDEO_TOOL "/tools/ffmpeg"
#define VIDEO_DIRECTORY "/video clips"
#endif
static inline UmiVideoSequence Sequence(void) {
  UmiVideoSequence sequence = {0};
  CHECK(UmiVideoSequenceInit("Example sequence", 1280U, 720U, &sequence) == UMI_STATUS_OK);
  UmiVideoSequenceClip clip = {0};
  strcpy(clip.id, "first");
  strcpy(clip.path, VIDEO_PATH);
  clip.begin_ms = 1000U;
  clip.end_ms = 5000U;
  CHECK(UmiVideoSequencePut(&sequence, &clip, false) == UMI_STATUS_OK);
  return sequence;
}
#endif
