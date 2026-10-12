/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/video_sequence.c
 * PURPOSE: Apply video edits atomically before a host publishes a changed sequence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/video_sequence.h"
#include "internal.h"
#include "umicom/platform/output_file.h"
#include <string.h>

/* Check termination before passing paths to platform helpers. The model's
 * ordinary-file syntax excludes URLs and implicit working-directory lookup. */
static bool ClipValid(const UmiVideoSequenceClip *clip) {
  return clip != NULL && UmiCreativeIdValid(clip->id) &&
         memchr(clip->path, 0, sizeof(clip->path)) != NULL &&
         UmiOutputFileValidatePath(clip->path) == UMI_STATUS_OK && clip->begin_ms < clip->end_ms &&
         clip->end_ms <= UMI_VIDEO_SEQUENCE_MAX_TIME_MS;
}
UmiStatus UmiVideoSequenceValidate(const UmiVideoSequence *sequence) {
  if (sequence == NULL || !UmiCreativeTextValid(sequence->title, sizeof(sequence->title), false) ||
      sequence->width < 16U || sequence->width > 1920U || sequence->width % 2U != 0U ||
      sequence->height < 16U || sequence->height > 1080U || sequence->height % 2U != 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (sequence->clip_count > UMI_VIDEO_SEQUENCE_MAX_CLIPS)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  uint32_t total = 0U;
  for (size_t i = 0U; i < sequence->clip_count; ++i) {
    const UmiVideoSequenceClip *clip = &sequence->clips[i];
    if (!ClipValid(clip))
      return UMI_STATUS_INVALID_ARGUMENT;
    /* Subtract only after checking the range, then bound the sum before
     * adding. This policy also bounds future render and timeline work. */
    uint32_t duration = clip->end_ms - clip->begin_ms;
    if (duration > UMI_VIDEO_SEQUENCE_MAX_TIME_MS - total)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    total += duration;
    for (size_t j = 0U; j < i; ++j)
      if (strcmp(sequence->clips[j].id, clip->id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
  }
  return UMI_STATUS_OK;
}
UmiStatus UmiVideoSequenceInit(const char *title, uint32_t width, uint32_t height,
                               UmiVideoSequence *out) {
  if (out == NULL || !UmiCreativeTextValid(title, UMI_CREATIVE_LABEL_CAPACITY, false))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiVideoSequence value = {0};
  memcpy(value.title, title, strlen(title) + 1U);
  value.width = width;
  value.height = height;
  UmiStatus status = UmiVideoSequenceValidate(&value);
  if (status == UMI_STATUS_OK)
    *out = value;
  return status;
}
UmiStatus UmiVideoSequenceDuration(const UmiVideoSequence *sequence, uint32_t *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = UmiVideoSequenceValidate(sequence);
  if (status != UMI_STATUS_OK)
    return status;
  uint32_t total = 0U;
  for (size_t i = 0U; i < sequence->clip_count; ++i)
    total += sequence->clips[i].end_ms - sequence->clips[i].begin_ms;
  *out = total;
  return UMI_STATUS_OK;
}
static size_t Find(const UmiVideoSequence *sequence, const char *id) {
  for (size_t i = 0U; i < sequence->clip_count; ++i)
    if (strcmp(sequence->clips[i].id, id) == 0)
      return i;
  return sequence->clip_count;
}
/* Publication is a single value assignment after validation. A GUI may keep
 * its previous preview and undo entry intact when any proposed edit fails. */
static UmiStatus Publish(UmiVideoSequence *destination, const UmiVideoSequence *candidate) {
  UmiStatus status = UmiVideoSequenceValidate(candidate);
  if (status == UMI_STATUS_OK)
    *destination = *candidate;
  return status;
}
UmiStatus UmiVideoSequencePut(UmiVideoSequence *sequence, const UmiVideoSequenceClip *clip,
                              bool replace) {
  UmiStatus status = UmiVideoSequenceValidate(sequence);
  if (status != UMI_STATUS_OK)
    return status;
  if (!ClipValid(clip))
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t index = Find(sequence, clip->id);
  if (replace && index == sequence->clip_count)
    return UMI_STATUS_NOT_FOUND;
  if (!replace && index != sequence->clip_count)
    return UMI_STATUS_ALREADY_EXISTS;
  if (index == UMI_VIDEO_SEQUENCE_MAX_CLIPS)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiVideoSequence candidate = *sequence;
  candidate.clips[index] = *clip;
  if (index == candidate.clip_count)
    ++candidate.clip_count;
  return Publish(sequence, &candidate);
}
UmiStatus UmiVideoSequenceRemove(UmiVideoSequence *sequence, const char *id) {
  UmiStatus status = UmiVideoSequenceValidate(sequence);
  if (status != UMI_STATUS_OK)
    return status;
  if (!UmiCreativeIdValid(id))
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t index = Find(sequence, id);
  if (index == sequence->clip_count)
    return UMI_STATUS_NOT_FOUND;
  UmiVideoSequence candidate = *sequence;
  --candidate.clip_count;
  memmove(&candidate.clips[index], &candidate.clips[index + 1U],
          (candidate.clip_count - index) * sizeof(candidate.clips[0]));
  memset(&candidate.clips[candidate.clip_count], 0, sizeof(candidate.clips[0]));
  return Publish(sequence, &candidate);
}
UmiStatus UmiVideoSequenceMove(UmiVideoSequence *sequence, const char *id, size_t position) {
  UmiStatus status = UmiVideoSequenceValidate(sequence);
  if (status != UMI_STATUS_OK)
    return status;
  if (!UmiCreativeIdValid(id) || position >= sequence->clip_count)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t index = Find(sequence, id);
  if (index == sequence->clip_count)
    return UMI_STATUS_NOT_FOUND;
  UmiVideoSequence candidate = *sequence;
  UmiVideoSequenceClip moved = candidate.clips[index];
  if (position < index)
    memmove(&candidate.clips[position + 1U], &candidate.clips[position],
            (index - position) * sizeof(moved));
  else if (position > index)
    memmove(&candidate.clips[index], &candidate.clips[index + 1U],
            (position - index) * sizeof(moved));
  candidate.clips[position] = moved;
  return Publish(sequence, &candidate);
}
UmiStatus UmiVideoSequenceSplit(UmiVideoSequence *sequence, const char *id, uint32_t sourceTime,
                                const char *rightId) {
  UmiStatus status = UmiVideoSequenceValidate(sequence);
  if (status != UMI_STATUS_OK)
    return status;
  if (!UmiCreativeIdValid(id) || !UmiCreativeIdValid(rightId))
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t index = Find(sequence, id);
  if (index == sequence->clip_count)
    return UMI_STATUS_NOT_FOUND;
  if (Find(sequence, rightId) != sequence->clip_count)
    return UMI_STATUS_ALREADY_EXISTS;
  if (sequence->clip_count == UMI_VIDEO_SEQUENCE_MAX_CLIPS)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (sourceTime <= sequence->clips[index].begin_ms || sourceTime >= sequence->clips[index].end_ms)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiVideoSequence candidate = *sequence;
  /* Copy before moving slots so even an ID borrowed from the original value
   * remains valid. The split preserves the exact total source interval. */
  UmiVideoSequenceClip right = candidate.clips[index];
  memcpy(right.id, rightId, strlen(rightId) + 1U);
  right.begin_ms = sourceTime;
  candidate.clips[index].end_ms = sourceTime;
  memmove(&candidate.clips[index + 2U], &candidate.clips[index + 1U],
          (candidate.clip_count - index - 1U) * sizeof(right));
  candidate.clips[index + 1U] = right;
  ++candidate.clip_count;
  return Publish(sequence, &candidate);
}

/* Compare named fields, never struct padding or unused slots. Equivalent
 * imports should not consume undo capacity simply because their padding differs. */
bool UmiVideoSequenceEqual(const UmiVideoSequence *left, const UmiVideoSequence *right) {
  if (UmiVideoSequenceValidate(left) != UMI_STATUS_OK ||
      UmiVideoSequenceValidate(right) != UMI_STATUS_OK)
    return false;
  if (strcmp(left->title, right->title) != 0 || left->width != right->width ||
      left->height != right->height || left->clip_count != right->clip_count)
    return false;
  for (size_t i = 0U; i < left->clip_count; ++i) {
    const UmiVideoSequenceClip *a = &left->clips[i], *b = &right->clips[i];
    if (strcmp(a->id, b->id) != 0 || strcmp(a->path, b->path) != 0 || a->begin_ms != b->begin_ms ||
        a->end_ms != b->end_ms)
      return false;
  }
  return true;
}
