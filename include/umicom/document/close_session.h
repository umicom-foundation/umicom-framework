/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/close_session.h
 * PURPOSE:
 *   Close a captured group of source documents through the existing reviewed
 *   close contract, pausing for a separate decision on every pending draft.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CLOSE_SESSION_H
#define UMICOM_DOCUMENT_CLOSE_SESSION_H
#include "umicom/document/close.h"
#ifdef __cplusplus
extern "C" {
#endif

/** ALL includes every managed source at creation. OTHERS excludes keepDocument
 * by identity, not whichever tab happens to be active later. Explicit File
 * operations include pinned sources; tab-button pin policy remains separate. */
typedef enum UmiDocumentCloseScope {
    UMI_DOCUMENT_CLOSE_ALL = 1,
    UMI_DOCUMENT_CLOSE_OTHERS
} UmiDocumentCloseScope;

/** READY allows one Step. DECISION awaits Save/Discard/Cancel. PATH means an
 * untitled Save was selected and requires an approved absolute filename.
 * COMPLETE, FAILED and CANCELLED are terminal, requiring a fresh session. */
typedef enum UmiDocumentClosePhase {
    UMI_DOCUMENT_CLOSE_READY = 1,
    UMI_DOCUMENT_CLOSE_DECISION,
    UMI_DOCUMENT_CLOSE_PATH,
    UMI_DOCUMENT_CLOSE_COMPLETE,
    UMI_DOCUMENT_CLOSE_FAILED,
    UMI_DOCUMENT_CLOSE_CANCELLED
} UmiDocumentClosePhase;

/** Copied progress. closed counts successful reviewed-close calls;
 * already_closed counts captured documents closed separately before their turn.
 * total = closed + already_closed + remaining. remaining includes a failed or
 * unanswered current document. current describes the most recently inspected
 * target (zero identity before the first step); it contains no source text.
 * A successful save followed by a close failure can leave a saved, open file;
 * counters describe closure, not an all-or-nothing file transaction. */
typedef struct UmiDocumentCloseProgress {
    UmiDocumentClosePhase phase;
    UmiStatus last_status;
    size_t total;
    size_t closed;
    size_t already_closed;
    size_t remaining;
    UmiDocumentCloseSummary current;
} UmiDocumentCloseProgress;

typedef struct UmiDocumentCloseSession UmiDocumentCloseSession;

/** Capture document IDs, original paths and opening order without changing
 * files, drafts or selection. ALL requires keepDocument=0; OTHERS requires an
 * existing ID. New documents opened afterwards are not included. The caller
 * owns *outSession; it is cleared on failure. The session borrows coordinator
 * until the last operation, and holds at most one complete close-plan draft.
 * All calls belong to the coordinator's owner thread. Providers must not
 * reentrantly mutate/destroy the coordinator or session. See
 * examples/editor_workflow/close_documents.c for a complete Notes project. */
UmiStatus UmiDocumentCloseSessionCreate(UmiDocumentCoordinator *coordinator,
    UmiDocumentCloseScope scope, UmiDocumentId keepDocument,
    UmiDocumentCloseSession **outSession);

/** Release captures only. Does not close/save anything; safe with NULL and
 * after coordinator teardown when no further session operation is performed. */
void UmiDocumentCloseSessionDestroy(UmiDocumentCloseSession *session);

/** Copy progress; no work is advanced. Invalid arguments leave output intact. */
UmiStatus UmiDocumentCloseSessionProgress(const UmiDocumentCloseSession *session,
    UmiDocumentCloseProgress *outProgress);

/** Process at most one original ID. Close a clean document, skip an independently
 * closed ID, or prepare the current draft for a decision. Changed paths/views
 * stop the run. A draft changed before its turn is reviewed in its latest state;
 * a draft changed after its question is prepared invalidates that question.
 * OK means inspect Progress, not necessarily that everything has closed. Calling
 * Step while a decision/path is needed does not advance or replace the question. */
UmiStatus UmiDocumentCloseSessionStep(UmiDocumentCloseSession *session);

/** Recheck the current question without synchronising, saving or closing.
 * Valid only in DECISION/PATH. Uses the original reviewed-close checks. */
UmiStatus UmiDocumentCloseSessionCheck(const UmiDocumentCloseSession *session);

/** Answer the current question. Save on an untitled document with NULL path
 * enters PATH without writing; respond again with SAVE and an approved absolute
 * filename. A supplied path for a named file is invalid. DISCARD never writes.
 * CANCEL stops remaining work and preserves pending drafts. Invalid decision or
 * path arguments are rejected without consuming the question. A failure from
 * the actual reviewed operation stops the sequence; it never falls back to
 * discard. Previously closed documents remain closed, including on cancellation.
 * Writes are synchronous within a response, with no rollback or interruption. */
UmiStatus UmiDocumentCloseSessionRespond(UmiDocumentCloseSession *session,
    UmiDocumentCloseDecision decision, const char *saveAsPath);

/** Stop remaining work. Idempotent for terminal sessions. Does not reopen
 * closed documents, roll back saved files or discard unanswered drafts. */
UmiStatus UmiDocumentCloseSessionCancel(UmiDocumentCloseSession *session);

/** Produce a complete plain-text status message into caller-owned storage.
 * Reject malformed phase/count/name data; leave output unchanged on invalid
 * input or insufficient capacity. Does not access documents or a filesystem. */
UmiStatus UmiDocumentCloseProgressFormat(const UmiDocumentCloseProgress *progress,
    char *text, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif /* UMICOM_DOCUMENT_CLOSE_SESSION_H */
