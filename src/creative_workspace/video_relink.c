/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/video_relink.c
 * PURPOSE: Stage a complete reference change before publishing it into a recipe or its undo history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/video_relink.h"
#include "umicom/platform/output_file.h"
#include "internal.h"
#include <string.h>

UmiStatus UmiVideoSequenceRelinkSources(UmiVideoSequence *sequence,
    const UmiVideoSequenceRelink *requests, size_t count,
    const UmiCancellationToken *cancel) {
  UmiStatus status = UmiVideoSequenceValidate(sequence);
  if (status != UMI_STATUS_OK)
    return status;
  if (requests == NULL || count == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (count > UMI_VIDEO_SEQUENCE_MAX_CLIPS)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (umi_cancellation_token_is_requested(cancel))
    return UMI_STATUS_CANCELLED;
  UmiVideoSequence candidate = *sequence;
  for (size_t i = 0U; i < count; ++i) {
    const UmiVideoSequenceRelink *request = &requests[i];
    if (!UmiCreativeIdValid(request->clip_id))
      return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiOutputFileValidatePath(request->expected_path);
    if (status == UMI_STATUS_OK)
      status = UmiOutputFileValidatePath(request->replacement_path);
    if (status != UMI_STATUS_OK)
      return status;
    for (size_t previous = 0U; previous < i; ++previous)
      if (strcmp(requests[previous].clip_id, request->clip_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    size_t index = 0U;
    while (index < sequence->clip_count &&
           strcmp(sequence->clips[index].id, request->clip_id) != 0)
      ++index;
    if (index == sequence->clip_count)
      return UMI_STATUS_NOT_FOUND;
    /* Compare against the original value, so a multi-clip swap cannot depend
     * on request order. A failed later request never publishes the earlier one. */
    if (strcmp(sequence->clips[index].path, request->expected_path) != 0)
      return UMI_STATUS_INVALID_STATE;
    memset(candidate.clips[index].path, 0, sizeof(candidate.clips[index].path));
    memcpy(candidate.clips[index].path, request->replacement_path,
           strlen(request->replacement_path) + 1U);
  }
  status = UmiVideoSequenceValidate(&candidate);
  if (status != UMI_STATUS_OK)
    return status;
  if (umi_cancellation_token_is_requested(cancel))
    return UMI_STATUS_CANCELLED;
  *sequence = candidate;
  return UMI_STATUS_OK;
}

UmiStatus UmiVideoSequenceEditorRelink(UmiVideoSequenceEditor *editor,
    const UmiVideoSequenceRelink *requests, size_t count,
    const UmiCancellationToken *cancel) {
  const UmiVideoSequence *current = UmiVideoSequenceEditorRead(editor);
  if (current == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiVideoSequence candidate = *current;
  UmiStatus status = UmiVideoSequenceRelinkSources(&candidate, requests, count, cancel);
  if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
    return UMI_STATUS_CANCELLED;
  /* Reuse the existing history owner. One relink command gets one undo entry;
   * equivalent references retain redo just like an equivalent ordinary edit. */
  return status == UMI_STATUS_OK ? UmiVideoSequenceEditorApply(editor, &candidate) : status;
}
