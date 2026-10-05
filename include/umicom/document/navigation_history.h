/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/navigation_history.h
 * PURPOSE: Expose session navigation through the authoritative document coordinator.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_NAVIGATION_HISTORY_H
#define UMICOM_DOCUMENT_NAVIGATION_HISTORY_H
#include "umicom/document/coordinator.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_NAVIGATION_CAPACITY 64U
/** History stores locations in live managed documents, never copies of text.
 * last_record_status reports an optional history capture failure; it does not
 * change the result of the original Open or Go To operation. */
typedef struct UmiDocumentNavigationHistorySnapshot {
    size_t count;
    uint64_t revision;
    int can_go_back;
    int can_go_forward;
    int busy;
    UmiStatus last_record_status;
} UmiDocumentNavigationHistorySnapshot;

/** Read navigation availability without reading files or changing drafts. */
UmiStatus UmiDocumentCoordinatorNavigationSnapshot(const UmiDocumentCoordinator *coordinator,
    UmiDocumentNavigationHistorySnapshot *out_snapshot);

/** Travel backward (-1) or forward (+1) using the observed history revision.
 * Open, Go To, tab cycling and successful search navigation record jumps.
 * Mouse caret movement is captured as the departure when travelling. This is
 * session history of live documents: closing a destination makes it unavailable;
 * use Reopen Closed Document separately to read a saved file again.
 * Positions are one-based lines and UTF-8 byte columns, resolved in the current
 * draft. A missing line or closed destination fails without moving the history
 * cursor or active tab. Long columns clamp to the line end. No text, undo stack,
 * file, pin or group is changed. Read-only documents permit navigation.
 * Use the owning thread; stale revision returns INVALID_STATE, overlapping
 * save/reopen/navigation returns BUSY. out_document is optional, unchanged on
 * failure. Availability cannot promise that a destination is still open. */
UmiStatus UmiDocumentCoordinatorTravel(UmiDocumentCoordinator *coordinator,
    int direction, uint64_t expected_revision, UmiDocumentId *out_document);

/** Forget location metadata after checking the observed revision. This does
 * not close a tab or discard text. Empty history can also be cleared. */
UmiStatus UmiDocumentCoordinatorClearNavigation(UmiDocumentCoordinator *coordinator,
    uint64_t expected_revision);
#ifdef __cplusplus
}
#endif
#endif
