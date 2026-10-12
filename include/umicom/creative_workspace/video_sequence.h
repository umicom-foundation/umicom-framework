/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/video_sequence.h
 * PURPOSE: Own bounded, non-destructive video edits independently of native controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_WORKSPACE_VIDEO_SEQUENCE_H
#define UMICOM_CREATIVE_WORKSPACE_VIDEO_SEQUENCE_H
#include "umicom/creative_workspace/asset.h"
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Six inputs leave room for explicit per-input policy within the native
 * process argument limit. Times are milliseconds; intervals exclude the end.
 * A recipe names local media, it does not embed or authenticate those files. */
#define UMI_VIDEO_SEQUENCE_MAX_CLIPS 6U
#define UMI_VIDEO_SEQUENCE_MAX_TIME_MS 600000U
#define UMI_VIDEO_SEQUENCE_HISTORY_LIMIT 16U
#define UMI_VIDEO_SEQUENCE_DOCUMENT_BYTES 131072U

typedef struct UmiVideoSequenceClip {
  char id[UMI_CREATIVE_ID_CAPACITY];
  char path[UMI_PATH_CAPACITY];
  uint32_t begin_ms;
  uint32_t end_ms;
} UmiVideoSequenceClip;

typedef struct UmiVideoSequence {
  char title[UMI_CREATIVE_LABEL_CAPACITY];
  uint32_t width, height;
  size_t clip_count;
  UmiVideoSequenceClip clips[UMI_VIDEO_SEQUENCE_MAX_CLIPS];
} UmiVideoSequence;

typedef struct UmiVideoSequenceEditor UmiVideoSequenceEditor;

/** @brief Initialise an empty sequence with an explicit title and even output
 * dimensions from 16 to 1920 by 16 to 1080. No files are read. Failure retains out. */
UmiStatus UmiVideoSequenceInit(const char *title, uint32_t width, uint32_t height,
                               UmiVideoSequence *out);
/** @brief Check bounded strings, absolute local paths, unique IDs and durations.
 * Each clip and the assembled sequence are limited to ten minutes. */
UmiStatus UmiVideoSequenceValidate(const UmiVideoSequence *sequence);
/** @brief Compare validated sequence values, ignoring padding and unused clip slots.
 * Invalid or NULL values compare unequal. Use this to detect actual edits or
 * whether undo returned to a saved recipe; media file contents are not compared. */
bool UmiVideoSequenceEqual(const UmiVideoSequence *left, const UmiVideoSequence *right);
/** @brief Read the assembled duration after validation. Failure retains out. */
UmiStatus UmiVideoSequenceDuration(const UmiVideoSequence *sequence, uint32_t *out);
/** @brief Add a new clip or explicitly replace an existing ID without reordering.
 * Invalid edits leave the sequence unchanged, including when input aliases it. */
UmiStatus UmiVideoSequencePut(UmiVideoSequence *sequence, const UmiVideoSequenceClip *clip,
                              bool replace);
/** @brief Remove a clip by ID and close the timeline gap. Failure retains state. */
UmiStatus UmiVideoSequenceRemove(UmiVideoSequence *sequence, const char *id);
/** @brief Move an existing clip to a zero-based final position. No media is copied. */
UmiStatus UmiVideoSequenceMove(UmiVideoSequence *sequence, const char *id, size_t position);
/** @brief Split at a source timestamp strictly inside the clip. The right side
 * receives a new unique ID and follows the left side. Failure retains state. */
UmiStatus UmiVideoSequenceSplit(UmiVideoSequence *sequence, const char *id, uint32_t sourceTime,
                                const char *rightId);
/** @brief Create an owned editor with bounded undo history. out must point to NULL. */
UmiStatus UmiVideoSequenceEditorCreate(const UmiVideoSequence *initial,
                                       UmiVideoSequenceEditor **out);
/** @brief Borrow the editor's current value until its next mutation or destruction. */
const UmiVideoSequence *UmiVideoSequenceEditorRead(const UmiVideoSequenceEditor *editor);
/** @brief Apply a valid changed value and discard redo history. Equal values are no-ops.
 * The oldest undo state is retired when the fixed history limit is reached. */
UmiStatus UmiVideoSequenceEditorApply(UmiVideoSequenceEditor *editor,
                                      const UmiVideoSequence *sequence);
/** @brief Restore the previous edit, or return NOT_FOUND without changes. */
UmiStatus UmiVideoSequenceEditorUndo(UmiVideoSequenceEditor *editor);
/** @brief Reapply an undone edit, or return NOT_FOUND without changes. */
UmiStatus UmiVideoSequenceEditorRedo(UmiVideoSequenceEditor *editor);
/** @brief Release editor storage. NULL is accepted; media files remain untouched. */
void UmiVideoSequenceEditorDestroy(UmiVideoSequenceEditor *editor);
/** @brief Export recipe metadata to an owned JSON document asset; out must be NULL.
 * Paths remain private local paths and no media file is opened or uploaded. */
UmiStatus UmiVideoSequenceExport(const UmiVideoSequence *sequence,
                                 const UmiCancellationToken *cancel, UmiCreativeAsset **out);
/** @brief Parse a bounded strict recipe without reading referenced files.
 * Unknown or duplicate fields, invalid values and cancellation retain out. */
UmiStatus UmiVideoSequenceImport(const void *bytes, size_t size, const UmiCancellationToken *cancel,
                                 UmiVideoSequence *out);
#ifdef __cplusplus
}
#endif
#endif
