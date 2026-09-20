/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/save_session.h
 *
 * PURPOSE:
 *   Save a captured list of working copies one at a time, allowing a graphical
 *   host to ask for the names of untitled documents without changing tabs.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_SAVE_SESSION_H
#define UMICOM_DOCUMENT_SAVE_SESSION_H
#include "umicom/document/coordinator.h"
#ifdef __cplusplus
extern "C" {
#endif

/** READY permits one Step; NEEDS_PATH awaits the user's filename decision.
 * COMPLETE, FAILED and CANCELLED are terminal. Earlier saved files remain
 * saved when a later operation fails or the user cancels. */
typedef enum UmiDocumentSavePhase {
    UMI_DOCUMENT_SAVE_READY = 1,
    UMI_DOCUMENT_SAVE_NEEDS_PATH,
    UMI_DOCUMENT_SAVE_COMPLETE,
    UMI_DOCUMENT_SAVE_FAILED,
    UMI_DOCUMENT_SAVE_CANCELLED
} UmiDocumentSavePhase;

/** Copied progress. total is the original pending-document count; remaining
 * includes an unsuccessful current item. saved counts confirmed successful
 * coordinator calls, not merely filename choices. unchanged counts documents
 * saved separately before their turn. current identifies the most recently
 * inspected item (zero for an empty run). Strings contain no document text. */
typedef struct UmiDocumentSaveProgress {
    UmiDocumentSavePhase phase;
    UmiStatus last_status;
    size_t total;
    size_t saved;
    size_t unchanged;
    size_t remaining;
    UmiDocumentId current_document;
    char display_name[UMI_DOCUMENT_NAME_CAPACITY];
} UmiDocumentSaveProgress;

typedef struct UmiDocumentSaveSession UmiDocumentSaveSession;

/** Capture the identities, order and paths of currently pending documents.
 * No files are written and no tabs selected. Text itself is not captured:
 * each step saves the latest draft of its original document on the owner
 * thread. New documents opened after creation belong to a future run.
 * The caller owns the session, which borrows coordinator until destruction.
 * Use all functions on the coordinator's owning thread; do not destroy either
 * owner during a call. See examples/editor_workflow/save_all.c. */
UmiStatus UmiDocumentSaveSessionCreate(UmiDocumentCoordinator *coordinator,
    UmiDocumentSaveSession **outSession);
void UmiDocumentSaveSessionDestroy(UmiDocumentSaveSession *session);

/** Copy progress without performing work. On invalid arguments output is
 * unchanged. Safe to call between steps, including after completion. */
UmiStatus UmiDocumentSaveSessionProgress(const UmiDocumentSaveSession *session,
    UmiDocumentSaveProgress *outProgress);

/** Process at most one captured document. A named document uses the existing
 * conflict-aware SaveAs path with its unchanged original filename. An untitled
 * document enters NEEDS_PATH without writing. A closed or renamed destination
 * fails the run, rather than redirecting to another tab. Inspect Progress after
 * OK: OK in NEEDS_PATH means that input is needed, not that a file was saved.
 * This is synchronous I/O; a host may return to its event loop between calls. */
UmiStatus UmiDocumentSaveSessionStep(UmiDocumentSaveSession *session);

/** Supply an explicitly chosen absolute filename for the current untitled
 * document. This uses existing Save As destination/overwrite semantics; the
 * caller must obtain user approval for the selected destination. A destination
 * owned by another open document is rejected. Relative or empty paths leave
 * the run in NEEDS_PATH for correction. Closing/renaming the original target
 * or a provider failure stops the run. The draft remains open on failure.
 * Cancellation or a later failure does not roll back earlier successful saves. */
UmiStatus UmiDocumentSaveSessionProvidePath(UmiDocumentSaveSession *session,
    const char *path);

/** Cancel remaining work. Does not discard documents or undo earlier saves.
 * Repeated cancellation is harmless. If called from a provider callback during
 * a step, cancellation takes effect after the current synchronous call returns. */
UmiStatus UmiDocumentSaveSessionCancel(UmiDocumentSaveSession *session);
#ifdef __cplusplus
}
#endif
#endif /* UMICOM_DOCUMENT_SAVE_SESSION_H */
