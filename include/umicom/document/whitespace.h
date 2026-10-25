/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/whitespace.h
 *
 * PURPOSE:
 *   Review whitespace changes against a captured document and retain existing Undo ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_WHITESPACE_H
#define UMICOM_DOCUMENT_WHITESPACE_H
#include "umicom/document/coordinator.h"
#include "umicom/editor/whitespace.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDocumentWhitespaceReview UmiDocumentWhitespaceReview;
typedef struct UmiDocumentWhitespaceSummary {
  UmiDocumentId document_id;
  char display_name[UMI_DOCUMENT_NAME_CAPACITY];
  UmiEditorWhitespaceOptions options;
  UmiEditorWhitespaceSummary text;
  int applied;
} UmiDocumentWhitespaceSummary;
/* Capture a named writable draft and prepare exact proposed source. This does
 * not select another tab, synchronize pending typing, change settings or write
 * a file. Keep the owner coordinator alive through Check/Apply; all operations
 * use its owner thread. Failure clears out_review. */
UmiStatus UmiDocumentWhitespaceReviewCreate(UmiDocumentCoordinator *owner, UmiDocumentId id,
                                            const UmiEditorWhitespaceOptions *options,
                                            UmiDocumentWhitespaceReview **out_review);
void UmiDocumentWhitespaceReviewDestroy(UmiDocumentWhitespaceReview *review);
UmiStatus UmiDocumentWhitespaceReviewInspect(const UmiDocumentWhitespaceReview *review,
                                             UmiDocumentWhitespaceSummary *out_summary);
/* Borrow captured full source and proposed full source until Destroy. Their
 * lifetime does not end on Apply. Supplied outputs clear on error. */
UmiStatus UmiDocumentWhitespaceReviewTexts(const UmiDocumentWhitespaceReview *review,
                                           const char **before, size_t *before_bytes,
                                           const char **after, size_t *after_bytes);
/* Compare document identity, path, metadata, revisions and exact draft. Moving
 * the caret or changing another tab is allowed; this review retains the
 * captured selection. A second Apply fails even if the text was unchanged. */
UmiStatus UmiDocumentWhitespaceReviewCheck(UmiDocumentCoordinator *owner,
                                           const UmiDocumentWhitespaceReview *review);
/* approved must equal 1. One Undo restores the exact departure draft and
 * current selection; earlier pending typing remains a separate history step.
 * Successful changed text restores the mapped captured selection. Unchanged
 * reviews keep current selection and add no history. No file/settings write. */
UmiStatus UmiDocumentWhitespaceReviewApply(UmiDocumentCoordinator *owner,
                                           UmiDocumentWhitespaceReview *review, int approved);
#ifdef __cplusplus
}
#endif
#endif
