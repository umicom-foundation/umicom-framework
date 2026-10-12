/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/video_sequence_history.c
 * PURPOSE: Keep reusable bounded edit history outside application and widget code.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/video_sequence.h"
#include <stdlib.h>
#include <string.h>
struct UmiVideoSequenceEditor {
  UmiVideoSequence states[UMI_VIDEO_SEQUENCE_HISTORY_LIMIT + 1U];
  size_t current, count;
};
UmiStatus UmiVideoSequenceEditorCreate(const UmiVideoSequence *initial,
                                       UmiVideoSequenceEditor **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (*out != NULL)
    return UMI_STATUS_INVALID_STATE;
  UmiStatus status = UmiVideoSequenceValidate(initial);
  if (status != UMI_STATUS_OK)
    return status;
  UmiVideoSequenceEditor *editor = calloc(1U, sizeof(*editor));
  if (editor == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  editor->states[0] = *initial;
  editor->count = 1U;
  *out = editor;
  return UMI_STATUS_OK;
}
const UmiVideoSequence *UmiVideoSequenceEditorRead(const UmiVideoSequenceEditor *editor) {
  return editor == NULL ? NULL : &editor->states[editor->current];
}
UmiStatus UmiVideoSequenceEditorApply(UmiVideoSequenceEditor *editor,
                                      const UmiVideoSequence *sequence) {
  if (editor == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = UmiVideoSequenceValidate(sequence);
  if (status != UMI_STATUS_OK)
    return status;
  if (UmiVideoSequenceEqual(&editor->states[editor->current], sequence))
    return UMI_STATUS_OK;
  UmiVideoSequence copy = *sequence;
  /* A fresh edit branches from the current state. Old redo entries cease to
   * be reachable; once full, retire only the oldest historical value. */
  if (editor->current == UMI_VIDEO_SEQUENCE_HISTORY_LIMIT) {
    memmove(editor->states, editor->states + 1, UMI_VIDEO_SEQUENCE_HISTORY_LIMIT * sizeof(copy));
    --editor->current;
  }
  editor->states[++editor->current] = copy;
  editor->count = editor->current + 1U;
  return UMI_STATUS_OK;
}
UmiStatus UmiVideoSequenceEditorUndo(UmiVideoSequenceEditor *editor) {
  if (editor == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (editor->current == 0U)
    return UMI_STATUS_NOT_FOUND;
  --editor->current;
  return UMI_STATUS_OK;
}
UmiStatus UmiVideoSequenceEditorRedo(UmiVideoSequenceEditor *editor) {
  if (editor == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (editor->current + 1U == editor->count)
    return UMI_STATUS_NOT_FOUND;
  ++editor->current;
  return UMI_STATUS_OK;
}
void UmiVideoSequenceEditorDestroy(UmiVideoSequenceEditor *editor) { free(editor); }
