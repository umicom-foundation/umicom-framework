/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/video_relink.h
 * PURPOSE: Relink reviewed media references without changing timeline edits or touching source files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_WORKSPACE_VIDEO_RELINK_H
#define UMICOM_CREATIVE_WORKSPACE_VIDEO_RELINK_H
#include "umicom/creative_workspace/video_sequence.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Strings are borrowed for this call and may refer to the original sequence.
 * Paths are compared exactly, without filesystem normalization or case folding.
 * They identify recipe references, not authenticated file contents. */
typedef struct UmiVideoSequenceRelink {
  const char *clip_id;
  const char *expected_path;
  const char *replacement_path;
} UmiVideoSequenceRelink;

/** @brief Apply one to six reviewed reference replacements as one value change.
 * Each clip ID must occur once. A missing ID returns NOT_FOUND; a changed
 * expected path returns INVALID_STATE; duplicate requests return ALREADY_EXISTS.
 * Invalid input, cancellation or any refused request leaves the whole sequence
 * unchanged. IDs, timing, order, dimensions and title are retained.
 * No files are read, moved, written or probed. An absolute replacement path
 * alone does not prove the media exists or still covers the selected interval.
 * The caller serializes edits on its owning thread. Cancellation is checked
 * before publication; cancellation arriving after publication does not undo it. */
UmiStatus UmiVideoSequenceRelinkSources(UmiVideoSequence *sequence,
    const UmiVideoSequenceRelink *requests, size_t count,
    const UmiCancellationToken *cancel);

/** @brief Relink through an editor as one undoable edit.
 * Failure retains current, undo and redo state. Replacing a reference with its
 * existing path is a no-op and retains redo history. All ownership and media
 * validation limits of UmiVideoSequenceRelinkSources also apply here. */
UmiStatus UmiVideoSequenceEditorRelink(UmiVideoSequenceEditor *editor,
    const UmiVideoSequenceRelink *requests, size_t count,
    const UmiCancellationToken *cancel);
#ifdef __cplusplus
}
#endif
#endif
