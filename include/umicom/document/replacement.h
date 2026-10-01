/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/replacement.h
 * PURPOSE: Capture and apply an immutable whole-document replacement review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_REPLACEMENT_H
#define UMICOM_DOCUMENT_REPLACEMENT_H
#include "umicom/document/coordinator.h"
#ifdef __cplusplus
extern "C" {
#endif

/** An owned capture, confined to the coordinator's owner thread. The caller
 * keeps the same coordinator, store and workbench alive through Apply/Check.
 * Reading or destroying a capture needs none of those borrowed services. */
typedef struct UmiDocumentReplacementPlan UmiDocumentReplacementPlan;

/** Copied review metadata. Sizes and positions count UTF-8 bytes. A match
 * may replace text with identical bytes; match_count is not an edit count. */
typedef struct UmiDocumentReplacementSummary {
    UmiDocumentId document_id;
    char display_name[UMI_DOCUMENT_NAME_CAPACITY];
    size_t previous_bytes;
    size_t proposed_bytes;
    size_t match_count;
    int text_changes;
    int has_unsaved_changes;
} UmiDocumentReplacementSummary;

/** Prepare all non-overlapping literal replacements in the named document's
 * complete visible draft, including text not yet synchronized to the store.
 * Uses the same ASCII smart-case rule as Find/Replace All: uppercase in the
 * needle requests exact case; otherwise ASCII letters ignore case. Non-ASCII
 * UTF-8 bytes compare exactly. No regex, escapes or replacement templates.
 * needle must be nonempty UTF-8; replacement may be empty (delete matches).
 * Both are NUL-terminated; an invalid UTF-8 draft returns INVALID_STATE.
 * No file I/O, selection, history or store mutation
 * occurs. Read-only targets fail. Excess input/output exceeds the existing
 * document text limit. No matches is a successful unchanged review.
 * Clears *outPlan on failure; the caller destroys every successful plan. */
UmiStatus UmiDocumentCoordinatorPrepareReplacement(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, const char *needle, const char *replacement,
    UmiDocumentReplacementPlan **outPlan);

/** Copy metadata. Leaves output unchanged on error; consumed plans fail. */
UmiStatus UmiDocumentReplacementPlanSummary(const UmiDocumentReplacementPlan *plan,
    UmiDocumentReplacementSummary *outSummary);

/** Borrow the exact before/after UTF-8 buffers displayed for review. They stay
 * valid until successful Apply or Destroy. Clears all supplied outputs on
 * invalid/consumed plans. The buffers are never prefixes of a longer draft. */
UmiStatus UmiDocumentReplacementPlanTexts(const UmiDocumentReplacementPlan *plan,
    const char **outPrevious, size_t *outPreviousBytes,
    const char **outProposed, size_t *outProposedBytes);

/** Check document/view identity, path, store/saved/text revisions, dirty and
 * external-change markers, and the captured bytes. Tab selection and caret
 * movement alone do not invalidate whole-document replacement. Closing the
 * target returns NOT_FOUND; changed state returns INVALID_STATE; read-only
 * state returns PERMISSION_DENIED. Never refreshes or changes the capture. */
UmiStatus UmiDocumentCoordinatorCheckReplacement(UmiDocumentCoordinator *coordinator,
    const UmiDocumentReplacementPlan *plan);

/** Apply exactly the reviewed text to the captured document, leaving the
 * active tab alone. A stale question must be discarded and prepared again.
 * One Undo restores the reviewed draft; earlier pending typing keeps its own
 * history step. Replacements clear selection and clamp the current caret to
 * a UTF-8 boundary. Unchanged text creates no edit or history and keeps the
 * selection. Success consumes the plan; failure leaves it and outCount intact.
 * No saved file is written. Saving remains a separate coordinator action. */
UmiStatus UmiDocumentCoordinatorApplyReplacement(UmiDocumentCoordinator *coordinator,
    UmiDocumentReplacementPlan *plan, size_t *outCount);

/** Release owned text and metadata. Accepts NULL, including after Apply. */
void UmiDocumentReplacementPlanDestroy(UmiDocumentReplacementPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
