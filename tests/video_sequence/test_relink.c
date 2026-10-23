/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_relink.c
 * PURPOSE: Verify path-only edits, stale review refusal, alias safety and whole-history rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/creative_workspace/video_relink.h"
#define NEW_PATH VIDEO_DIRECTORY "/moved clip.mp4"
#define OTHER_PATH VIDEO_DIRECTORY "/other clip.mp4"

static UmiVideoSequence ThreeClips(void) {
  UmiVideoSequence value = Sequence();
  CHECK(UmiVideoSequenceSplit(&value, "first", 3000U, "second") == UMI_STATUS_OK);
  UmiVideoSequenceClip third = value.clips[1];
  strcpy(third.id, "third");
  strcpy(third.path, OTHER_PATH);
  CHECK(UmiVideoSequencePut(&value, &third, false) == UMI_STATUS_OK);
  return value;
}
static void SameEdits(const UmiVideoSequence *before, const UmiVideoSequence *after) {
  CHECK(strcmp(before->title, after->title) == 0);
  CHECK(before->width == after->width && before->height == after->height);
  CHECK(before->clip_count == after->clip_count);
  for (size_t i = 0U; i < before->clip_count; ++i) {
    CHECK(strcmp(before->clips[i].id, after->clips[i].id) == 0);
    CHECK(before->clips[i].begin_ms == after->clips[i].begin_ms);
    CHECK(before->clips[i].end_ms == after->clips[i].end_ms);
  }
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  UmiVideoSequence sequence = ThreeClips(), original = sequence;
  UmiVideoSequenceRelink requests[] = {
      {"first", VIDEO_PATH, NEW_PATH}, {"second", VIDEO_PATH, NEW_PATH}};
  if (strcmp(argv[1], "single") == 0 || strcmp(argv[1], "batch") == 0) {
    size_t count = strcmp(argv[1], "batch") == 0 ? 2U : 1U;
    CHECK(UmiVideoSequenceRelinkSources(&sequence, requests, count, NULL) == UMI_STATUS_OK);
    SameEdits(&original, &sequence);
    CHECK(strcmp(sequence.clips[0].path, NEW_PATH) == 0);
    CHECK(strcmp(sequence.clips[1].path, count == 2U ? NEW_PATH : VIDEO_PATH) == 0);
    CHECK(strcmp(sequence.clips[2].path, OTHER_PATH) == 0);
  } else if (strcmp(argv[1], "swap-alias") == 0) {
    /* Requests borrow both old paths. A replacement must not overwrite the
     * second request's source before the whole proposal has been checked. */
    UmiVideoSequenceRelink swap[] = {
        {sequence.clips[0].id, sequence.clips[0].path, sequence.clips[2].path},
        {sequence.clips[2].id, sequence.clips[2].path, sequence.clips[0].path}};
    CHECK(UmiVideoSequenceRelinkSources(&sequence, swap, 2U, NULL) == UMI_STATUS_OK);
    CHECK(strcmp(sequence.clips[0].path, OTHER_PATH) == 0);
    CHECK(strcmp(sequence.clips[2].path, VIDEO_PATH) == 0);
    SameEdits(&original, &sequence);
  } else if (strcmp(argv[1], "cancelled") == 0) {
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    umi_cancellation_token_request(cancel);
    CHECK(UmiVideoSequenceRelinkSources(&sequence, requests, 2U, cancel) == UMI_STATUS_CANCELLED);
    CHECK(memcmp(&sequence, &original, sizeof(sequence)) == 0);
    umi_cancellation_token_destroy(cancel);
  } else if (strcmp(argv[1], "roundtrip") == 0) {
    CHECK(UmiVideoSequenceRelinkSources(&sequence, requests, 2U, NULL) == UMI_STATUS_OK);
    UmiCreativeAsset *asset = NULL;
    CHECK(UmiVideoSequenceExport(&sequence, NULL, &asset) == UMI_STATUS_OK);
    const void *bytes = NULL;
    size_t size = 0U;
    CHECK(UmiCreativeAssetBytes(asset, &bytes, &size) == UMI_STATUS_OK);
    UmiVideoSequence loaded = {0};
    CHECK(UmiVideoSequenceImport(bytes, size, NULL, &loaded) == UMI_STATUS_OK);
    CHECK(UmiVideoSequenceEqual(&loaded, &sequence));
    UmiCreativeAssetDestroy(asset);
  } else if (strcmp(argv[1], "history") == 0 || strcmp(argv[1], "noop-redo") == 0 ||
             strcmp(argv[1], "failed-history") == 0 || strcmp(argv[1], "branch-history") == 0) {
    UmiVideoSequenceEditor *editor = NULL;
    CHECK(UmiVideoSequenceEditorCreate(&sequence, &editor) == UMI_STATUS_OK);
    CHECK(UmiVideoSequenceEditorRelink(editor, requests, 2U, NULL) == UMI_STATUS_OK);
    UmiVideoSequence changed = *UmiVideoSequenceEditorRead(editor);
    CHECK(UmiVideoSequenceEditorUndo(editor) == UMI_STATUS_OK);
    CHECK(UmiVideoSequenceEqual(UmiVideoSequenceEditorRead(editor), &original));
    CHECK(UmiVideoSequenceEditorUndo(editor) == UMI_STATUS_NOT_FOUND);
    if (strcmp(argv[1], "noop-redo") == 0) {
      requests[0].replacement_path = VIDEO_PATH;
      CHECK(UmiVideoSequenceEditorRelink(editor, requests, 1U, NULL) == UMI_STATUS_OK);
    } else if (strcmp(argv[1], "failed-history") == 0) {
      requests[1].expected_path = OTHER_PATH;
      CHECK(UmiVideoSequenceEditorRelink(editor, requests, 2U, NULL) == UMI_STATUS_INVALID_STATE);
      CHECK(UmiVideoSequenceEqual(UmiVideoSequenceEditorRead(editor), &original));
    } else if (strcmp(argv[1], "branch-history") == 0) {
      requests[0].replacement_path = OTHER_PATH;
      CHECK(UmiVideoSequenceEditorRelink(editor, requests, 1U, NULL) == UMI_STATUS_OK);
      CHECK(UmiVideoSequenceEditorRedo(editor) == UMI_STATUS_NOT_FOUND);
      CHECK(UmiVideoSequenceEditorUndo(editor) == UMI_STATUS_OK);
      CHECK(UmiVideoSequenceEqual(UmiVideoSequenceEditorRead(editor), &original));
      UmiVideoSequenceEditorDestroy(editor);
      return 0;
    }
    CHECK(UmiVideoSequenceEditorRedo(editor) == UMI_STATUS_OK);
    CHECK(UmiVideoSequenceEqual(UmiVideoSequenceEditorRead(editor), &changed));
    UmiVideoSequenceEditorDestroy(editor);
  } else if (strcmp(argv[1], "null") == 0) {
    CHECK(UmiVideoSequenceRelinkSources(NULL, requests, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiVideoSequenceRelinkSources(&sequence, NULL, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiVideoSequenceEditorRelink(NULL, requests, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    requests[0].replacement_path = NULL;
    CHECK(UmiVideoSequenceRelinkSources(&sequence, requests, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&sequence, &original, sizeof(sequence)) == 0);
  } else if (strcmp(argv[1], "count") == 0) {
    CHECK(UmiVideoSequenceRelinkSources(&sequence, requests, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiVideoSequenceRelinkSources(&sequence, requests, UMI_VIDEO_SEQUENCE_MAX_CLIPS + 1U,
                                      NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&sequence, &original, sizeof(sequence)) == 0);
  } else {
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    char unbounded[UMI_PATH_CAPACITY];
    memset(unbounded, 'x', sizeof(unbounded));
    if (strcmp(argv[1], "stale") == 0) {
      requests[1].expected_path = OTHER_PATH;
      expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(argv[1], "missing") == 0) {
      requests[1].clip_id = "missing";
      expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(argv[1], "duplicate") == 0) {
      requests[1].clip_id = "first";
      expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (strcmp(argv[1], "relative") == 0) {
      requests[1].replacement_path = "relative.mp4";
    } else if (strcmp(argv[1], "unterminated") == 0) {
      requests[1].replacement_path = unbounded;
      expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(argv[1], "invalid-id") == 0) {
      requests[1].clip_id = "";
    } else if (strcmp(argv[1], "invalid-sequence") == 0) {
      sequence.width = 1U;
      original = sequence;
    } else
      return 2;
    CHECK(UmiVideoSequenceRelinkSources(&sequence, requests, 2U, NULL) == expected);
    CHECK(memcmp(&sequence, &original, sizeof(sequence)) == 0);
  }
  return 0;
}
