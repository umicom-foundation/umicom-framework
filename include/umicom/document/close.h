/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/close.h
 * PURPOSE:
 *   Review a source document before closing it, and bind Save/Discard decisions
 *   to that document's captured state rather than to the currently active tab.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CLOSE_H
#define UMICOM_DOCUMENT_CLOSE_H
#include "umicom/document/coordinator.h"
#ifdef __cplusplus
extern "C" {
#endif

/** A decision is explicit: closing a clean file, saving its draft, discarding
 * its draft, or cancelling the question are different operations. Never turn
 * a failed Save into DISCARD. CANCEL writes and closes nothing. */
typedef enum UmiDocumentCloseDecision {
    UMI_DOCUMENT_CLOSE_UNMODIFIED = 1,
    UMI_DOCUMENT_CLOSE_SAVE,
    UMI_DOCUMENT_CLOSE_DISCARD,
    UMI_DOCUMENT_CLOSE_CANCEL
} UmiDocumentCloseDecision;

/** A copied description for a confirmation form. text_bytes counts UTF-8
 * bytes, not characters. dirty includes unsynchronised visible edits and an
 * ordinary untitled draft. A read-only document can be inspected or explicitly
 * discarded, but cannot be saved by this operation. No text is exposed here. */
typedef struct UmiDocumentCloseSummary {
    UmiDocumentId document_id;
    size_t text_bytes;
    int dirty;
    int has_path;
    int read_only;
    char display_name[UMI_DOCUMENT_NAME_CAPACITY];
} UmiDocumentCloseSummary;

typedef struct UmiDocumentClosePlan UmiDocumentClosePlan;

/** Capture one document without selecting, saving, synchronising or closing it.
 * The plan owns a complete draft copy (bounded by the existing document limit)
 * and borrows coordinator. NULL/zero arguments fail; outPlan is cleared before
 * work. The coordinator must outlive Check/Apply, and all calls belong on its
 * owner thread. See examples/editor_workflow/close_document.c. */
UmiStatus UmiDocumentCoordinatorPrepareClose(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, UmiDocumentClosePlan **outPlan);

/** Copy the original question's description. Output is unchanged on failure.
 * The copy is not a live availability query: call CheckClose before applying. */
UmiStatus UmiDocumentClosePlanSummary(const UmiDocumentClosePlan *plan,
    UmiDocumentCloseSummary *outSummary);

/** Check that the intended document still has the captured identity, path,
 * stored revision, visible-text revision, draft text and read-only/dirty state.
 * Newer text, an independent Save/Save As, or closing/reopening invalidates an
 * old decision. Moving the caret, selecting text, changing tabs or editing an
 * unrelated document does not. Does not read a disk or alter either version.
 * INVALID_STATE includes a consumed or outdated plan; NOT_FOUND means its
 * original document or view no longer exists. No pointers are retained here. */
UmiStatus UmiDocumentCoordinatorCheckClose(UmiDocumentCoordinator *coordinator,
    const UmiDocumentClosePlan *plan);

/** Apply one decision to the captured document. UNMODIFIED refuses a pending
 * draft. DISCARD requires an explicit user decision and never writes the file.
 * SAVE uses the existing conflict-aware coordinator; an untitled draft requires
 * an explicitly approved absolute saveAsPath. A named document requires NULL
 * and keeps its original filename. Relative/empty paths are rejected before
 * saving. CANCEL consumes the plan and returns CANCELLED without touching the
 * document, even when the question has become outdated.
 *
 * Once a close or save attempt starts the plan is consumed, including failure;
 * create a fresh question before retrying. Invalid arguments and outdated-state
 * checks do not consume it. A failed Save never falls through to discard. A
 * successful write may precede a later close/presentation error: this is not a
 * filesystem transaction and no successful write is rolled back. An external
 * writer is checked by the existing Save As policy, not by CheckClose.
 *
 * This is synchronous, single-owner work. Providers must not destroy or mutate
 * the coordinator reentrantly. No callback/context may outlive its GUI binding.
 * The plan must be destroyed after use; applying it does not free it. */
UmiStatus UmiDocumentCoordinatorApplyClose(UmiDocumentCoordinator *coordinator,
    UmiDocumentClosePlan *plan, UmiDocumentCloseDecision decision,
    const char *saveAsPath);

/** Free only the captured plan. Safe with NULL and after coordinator teardown;
 * destroying an unanswered plan cancels the question without changing a file. */
void UmiDocumentClosePlanDestroy(UmiDocumentClosePlan *plan);
#ifdef __cplusplus
}
#endif
#endif /* UMICOM_DOCUMENT_CLOSE_H */
