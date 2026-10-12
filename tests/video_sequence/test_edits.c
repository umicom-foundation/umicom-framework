/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_edits.c
 * PURPOSE: Exercise edit invariants, rollback on failure and bounded undo branches.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  UmiVideoSequence sequence = Sequence();
  UmiVideoSequence original = sequence;
  if (strcmp(argv[1], "split") == 0) {
    CHECK(UmiVideoSequenceSplit(&sequence, "first", 3000U, "right") == UMI_STATUS_OK);
    CHECK(sequence.clip_count == 2U && sequence.clips[0].end_ms == 3000U &&
          sequence.clips[1].begin_ms == 3000U && sequence.clips[1].end_ms == 5000U);
    uint32_t total = 0U;
    CHECK(UmiVideoSequenceDuration(&sequence, &total) == UMI_STATUS_OK && total == 4000U);
    original = sequence;
    CHECK(UmiVideoSequenceSplit(&sequence, "right", 3000U, "third") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&sequence, &original, sizeof(sequence)) == 0);
    CHECK(UmiVideoSequenceSplit(&sequence, "right", 4000U, "first") == UMI_STATUS_ALREADY_EXISTS);
  } else if (strcmp(argv[1], "reorder") == 0) {
    CHECK(UmiVideoSequenceSplit(&sequence, "first", 3000U, "right") == UMI_STATUS_OK);
    CHECK(UmiVideoSequenceSplit(&sequence, "right", 4000U, "last") == UMI_STATUS_OK);
    CHECK(UmiVideoSequenceMove(&sequence, "last", 0U) == UMI_STATUS_OK);
    CHECK(strcmp(sequence.clips[0].id, "last") == 0 && strcmp(sequence.clips[1].id, "first") == 0);
    CHECK(UmiVideoSequenceMove(&sequence, "last", 2U) == UMI_STATUS_OK);
    CHECK(strcmp(sequence.clips[0].id, "first") == 0 && strcmp(sequence.clips[2].id, "last") == 0);
  } else if (strcmp(argv[1], "trim") == 0) {
    UmiVideoSequenceClip clip = sequence.clips[0];
    clip.begin_ms = 2000U;
    CHECK(UmiVideoSequencePut(&sequence, &clip, true) == UMI_STATUS_OK);
    CHECK(sequence.clips[0].begin_ms == 2000U);
    original = sequence;
    clip.end_ms = 1000U;
    CHECK(UmiVideoSequencePut(&sequence, &clip, true) != UMI_STATUS_OK);
    CHECK(memcmp(&original, &sequence, sizeof(sequence)) == 0);
  } else if (strcmp(argv[1], "remove") == 0) {
    CHECK(UmiVideoSequenceRemove(&sequence, "absent") == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&sequence, &original, sizeof(sequence)) == 0);
    CHECK(UmiVideoSequenceRemove(&sequence, "first") == UMI_STATUS_OK && sequence.clip_count == 0U);
    CHECK(UmiVideoSequenceValidate(&sequence) == UMI_STATUS_OK);
  } else if (strcmp(argv[1], "bounds") == 0) {
    sequence.width = 1279U;
    CHECK(UmiVideoSequenceValidate(&sequence) == UMI_STATUS_INVALID_ARGUMENT);
    sequence = original;
    memset(sequence.clips[0].path, 'x', sizeof(sequence.clips[0].path));
    CHECK(UmiVideoSequenceValidate(&sequence) == UMI_STATUS_INVALID_ARGUMENT);
    sequence = original;
    sequence.clips[0].end_ms = UMI_VIDEO_SEQUENCE_MAX_TIME_MS;
    UmiVideoSequenceClip next = sequence.clips[0];
    strcpy(next.id, "second");
    next.begin_ms = 0U;
    next.end_ms = 2000U;
    original = sequence;
    CHECK(UmiVideoSequencePut(&sequence, &next, false) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&original, &sequence, sizeof(sequence)) == 0);
  } else if (strcmp(argv[1], "alias") == 0) {
    CHECK(UmiVideoSequencePut(&sequence, &sequence.clips[0], true) == UMI_STATUS_OK);
    CHECK(UmiVideoSequenceSplit(&sequence, sequence.clips[0].id, 3000U, "right") == UMI_STATUS_OK);
    CHECK(UmiVideoSequenceMove(&sequence, sequence.clips[1].id, 0U) == UMI_STATUS_OK);
    CHECK(UmiVideoSequenceRemove(&sequence, sequence.clips[0].id) == UMI_STATUS_OK);
    CHECK(strcmp(sequence.clips[0].id, "first") == 0);
  } else if (strcmp(argv[1], "history") == 0) {
    UmiVideoSequenceEditor *editor = NULL;
    CHECK(UmiVideoSequenceEditorCreate(&sequence, &editor) == UMI_STATUS_OK);
    for (uint32_t i = 1U; i <= 20U; ++i) {
      sequence.clips[0].begin_ms = 1000U + i;
      CHECK(UmiVideoSequenceEditorApply(editor, &sequence) == UMI_STATUS_OK);
    }
    for (size_t i = 0U; i < UMI_VIDEO_SEQUENCE_HISTORY_LIMIT; ++i)
      CHECK(UmiVideoSequenceEditorUndo(editor) == UMI_STATUS_OK);
    CHECK(UmiVideoSequenceEditorUndo(editor) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiVideoSequenceEditorRead(editor)->clips[0].begin_ms == 1004U);
    CHECK(UmiVideoSequenceEditorRedo(editor) == UMI_STATUS_OK);
    /* A no-op preserves redo; a different edit invalidates it. */
    CHECK(UmiVideoSequenceEditorApply(editor, UmiVideoSequenceEditorRead(editor)) == UMI_STATUS_OK);
    CHECK(UmiVideoSequenceEditorRedo(editor) == UMI_STATUS_OK);
    CHECK(UmiVideoSequenceEditorApply(editor, &original) == UMI_STATUS_OK);
    CHECK(UmiVideoSequenceEditorRedo(editor) == UMI_STATUS_NOT_FOUND);
    UmiVideoSequenceEditorDestroy(editor);
  } else
    return 2;
  return 0;
}
