/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/caption_text.h
 * PURPOSE:
 *   Obtain plain-text transcript candidates from an already validated
 *   Framework caption track without implementing another SRT/VTT parser.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_CAPTION_TEXT_H
#define UMICOM_AI_TRAINING_CAPTION_TEXT_H

#include <stddef.h>
#include "umicom/creative_workspace/captions.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Join each reviewed caption cue's exact UTF-8 text with one LF separator.
 * The caller supplies the destination; no file is written. This generates a
 * CANDIDATE for human review, not automatically admitted training data.
 * The caller must subsequently verify source rights, privacy, transcript
 * correctness and digest identity through the source-admission policy.
 * Failure, including insufficient capacity, preserves outText and *outBytes.
 * Source lifetime and concurrent access remain governed by UmiCaptionTrack. */
UmiStatus UmiAiTrainingCaptionTextExtract(
    const UmiCaptionTrack *track,
    char *outText, size_t capacity, size_t *outBytes);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_CAPTION_TEXT_H */
