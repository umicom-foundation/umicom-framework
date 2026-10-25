/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/whitespace_review.inc
 *
 * PURPOSE:
 *   Bind pure whitespace proposals to exact document identity and one history transaction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/whitespace.h"
/* Included after replacement_review.inc: the replacement capture is the
 * authoritative source-identity check. Whitespace owns only its pure plan and
 * summary; publication continues through the coordinator's one Undo owner. */
struct UmiDocumentWhitespaceReview {
  UmiDocumentReplacementPlan *target;
  UmiEditorWhitespacePlan *plan;
  UmiDocumentWhitespaceSummary summary;
};
void UmiDocumentWhitespaceReviewDestroy(UmiDocumentWhitespaceReview *review) {
  if (review != NULL) {
    UmiDocumentReplacementPlanDestroy(review->target);
    UmiEditorWhitespacePlanDestroy(review->plan);
    free(review);
  }
}
UmiStatus UmiDocumentWhitespaceReviewCreate(UmiDocumentCoordinator *owner, UmiDocumentId id,
                                            const UmiEditorWhitespaceOptions *options,
                                            UmiDocumentWhitespaceReview **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (owner == NULL || id == 0U || options == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t index = EditDocumentIndex(owner, id);
  if (index == SIZE_MAX)
    return UMI_STATUS_NOT_FOUND;
  UmiDocumentWhitespaceReview *review = calloc(1U, sizeof *review);
  if (review == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  review->target = calloc(1U, sizeof *review->target);
  if (review->target == NULL) {
    free(review);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  UmiDocumentReplacementPlan *target = review->target;
  UmiStatus status = UmiDocumentCoordinatorPrepareEdit(owner, id, &target->capture);
  char identity[UMI_UI_ID_CAPACITY];
  (void)snprintf(identity, sizeof identity, "document.%llu", (unsigned long long)id);
  if (status == UMI_STATUS_OK && strcmp(target->capture->view.document_id, identity) != 0)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK && target->capture->view.read_only)
    status = UMI_STATUS_PERMISSION_DENIED;
  if (status == UMI_STATUS_OK)
    status = UmiEditorWhitespacePlanCreate(
        target->capture->text, target->capture->length, target->capture->view.cursor_offset,
        target->capture->view.selection_length, options, NULL, &review->plan);
  if (status == UMI_STATUS_OK)
    status = UmiEditorWhitespacePlanInspect(review->plan, &review->summary.text);
  if (status != UMI_STATUS_OK) {
    UmiDocumentWhitespaceReviewDestroy(review);
    return status;
  }
  const UmiDocumentCoordinatorEntry *entry = &owner->entries[index];
  target->baseline = entry->baseline;
  target->encoding = entry->encoding;
  target->lineEnding = entry->line_ending;
  target->conflict = entry->conflict;
  review->summary.document_id = id;
  review->summary.options = *options;
  (void)snprintf(review->summary.display_name, sizeof review->summary.display_name, "%s",
                 target->capture->stored.display_name);
  *out = review;
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentWhitespaceReviewInspect(const UmiDocumentWhitespaceReview *review,
                                             UmiDocumentWhitespaceSummary *out) {
  if (review == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = review->summary;
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentWhitespaceReviewTexts(const UmiDocumentWhitespaceReview *review,
                                           const char **before, size_t *before_bytes,
                                           const char **after, size_t *after_bytes) {
  if (before != NULL)
    *before = NULL;
  if (after != NULL)
    *after = NULL;
  if (before_bytes != NULL)
    *before_bytes = 0U;
  if (after_bytes != NULL)
    *after_bytes = 0U;
  if (review == NULL || before == NULL || before_bytes == NULL || after == NULL ||
      after_bytes == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = UmiEditorWhitespacePlanRead(review->plan, after, after_bytes);
  if (status == UMI_STATUS_OK) {
    *before = review->target->capture->text;
    *before_bytes = review->target->capture->length;
  }
  return status;
}
UmiStatus UmiDocumentWhitespaceReviewCheck(UmiDocumentCoordinator *owner,
                                           const UmiDocumentWhitespaceReview *review) {
  if (review == NULL || owner == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (owner->save_in_progress || owner->reopen_in_progress || owner->navigation_depth != 0U)
    return UMI_STATUS_BUSY;
  if (review->summary.applied)
    return UMI_STATUS_ALREADY_EXISTS;
  return UmiDocumentCoordinatorCheckReplacement(owner, review->target);
}
UmiStatus UmiDocumentWhitespaceReviewApply(UmiDocumentCoordinator *owner,
                                           UmiDocumentWhitespaceReview *review, int approved) {
  if (approved != 1)
    return UMI_STATUS_PERMISSION_DENIED;
  UmiStatus status = UmiDocumentWhitespaceReviewCheck(owner, review);
  size_t index = 0U;
  UmiUiDocumentViewSnapshot before;
  if (status == UMI_STATUS_OK)
    status = ReplacementCurrent(owner, review->target, &index, &before);
  if (status == UMI_STATUS_OK && review->summary.text.changed) {
    const char *text = NULL;
    size_t bytes = 0U;
    status = UmiEditorWhitespacePlanRead(review->plan, &text, &bytes);
    if (status == UMI_STATUS_OK) {
      UmiUiDocumentViewSnapshot after = before;
      after.cursor_offset = review->summary.text.cursor_offset;
      after.selection_length = review->summary.text.selection_bytes;
      after.preview = 0;
      status = CommitViewText(owner, index, &before, &after, review->target->capture->text,
                              review->target->capture->length, text, bytes);
    }
  }
  if (status == UMI_STATUS_OK)
    review->summary.applied = 1;
  return status;
}
