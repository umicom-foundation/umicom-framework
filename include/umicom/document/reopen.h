/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/reopen.h
 * PURPOSE: Reopen recently closed saved documents without retaining discarded drafts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_REOPEN_H
#define UMICOM_DOCUMENT_REOPEN_H
#include "umicom/document/coordinator.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_DOCUMENT_REOPEN_CAPACITY 16U

/* This session-only history stores names and absolute paths, never text or
 * undo stacks. A closed untitled or relative-path document is not recorded.
 * Newest names can be displayed without exposing the full path in a menu. */
typedef struct UmiDocumentReopenSnapshot {
    size_t count;
    uint64_t revision;
    int busy;
    char next_name[UMI_DOCUMENT_NAME_CAPACITY];
} UmiDocumentReopenSnapshot;

/* Observe the bounded history without reading a provider or changing tabs.
 * Output is unchanged on invalid arguments. Calls use the coordinator thread. */
UmiStatus UmiDocumentCoordinatorReopenSnapshot(const UmiDocumentCoordinator *coordinator,
    UmiDocumentReopenSnapshot *out_snapshot);

/* Reopen the newest path through the existing provider and open-document
 * machinery. A still-open document is activated without replacing its draft.
 * Otherwise the provider's current contents are loaded, so discarded edits
 * are not resurrected. A missing/unreadable file remains on the history for
 * retry or explicit Forget. Success consumes that entry and returns the live
 * document identity. Failures leave out_document unchanged. No writes occur.
 *
 * Pass the revision read when the action was chosen. A newer close, successful
 * reopen or Forget invalidates the old request. Provider callbacks must not
 * destroy or mutate the coordinator; nested reopening and closing are BUSY.
 * The newest 16 distinct paths are retained; the oldest is evicted on overflow. */
UmiStatus UmiDocumentCoordinatorReopenLast(UmiDocumentCoordinator *coordinator,
    uint64_t expected_revision, UmiDocumentId *out_document);

/* Skip the newest history entry without touching its file or any open draft.
 * This makes older entries reachable when the newest path no longer exists.
 * Empty history returns NOT_FOUND; stale requests preserve all entries. */
UmiStatus UmiDocumentCoordinatorForgetClosed(UmiDocumentCoordinator *coordinator,
    uint64_t expected_revision);
#ifdef __cplusplus
}
#endif
#endif
