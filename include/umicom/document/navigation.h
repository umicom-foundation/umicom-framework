/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/navigation.h
 * PURPOSE:
 *   Interpret a requested source location and navigate through the existing
 *   document coordinator without changing or saving source text.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_NAVIGATION_H
#define UMICOM_DOCUMENT_NAVIGATION_H
#include "umicom/document/coordinator.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_LOCATION_TEXT_MAX 96U

/** One-based source location. column counts UTF-8 bytes, as in the existing
 * compiler-navigation contract, not screen cells. An omitted column is 1.
 * Example: "12:5" requests byte column 5 of line 12 in an Umicom Notes file.
 * See examples/editor_workflow/navigation.c for a complete runnable caller. */
typedef struct UmiDocumentLocation {
    size_t line;
    size_t column;
} UmiDocumentLocation;

/** Parse exactly byteCount readable bytes as line or line:column.
 * Surrounding ASCII spaces/tabs are allowed; signs, internal whitespace,
 * zero, embedded zero bytes, extra separators and size_t overflow are rejected.
 * The 96-byte limit is checked before reading the input. No allocation occurs.
 * INVALID_ARGUMENT reports syntax/range errors; CAPACITY_EXCEEDED reports an
 * overlong input. outLocation is required and changes only on success. */
UmiStatus UmiDocumentLocationParse(const char *text, size_t byteCount,
    UmiDocumentLocation *outLocation);

/** Navigate only while expectedDocument is still the active document.
 * Capture its nonzero ID before showing an input form. A changed active tab
 * returns INVALID_STATE; no active document returns NOT_FOUND. This is a guard
 * for delayed UI input, not a document store or a history implementation.
 * After parsing, this delegates to UmiDocumentCoordinatorGoToPosition. The
 * position is resolved against the current visible draft: missing lines fail,
 * long columns clamp to the line end, and a byte inside a UTF-8 character moves
 * to that character's first byte. Selection clears only on success. Read-only
 * documents permit navigation. No save, history operation or text edit occurs.
 * All calls use the coordinator's owning thread. outOffset is optional and
 * remains unchanged on failure. A form must also protect its owner's lifetime.
 * Example: capture active.document_id, then pass "12:5" with byteCount 4. */
UmiStatus UmiDocumentCoordinatorNavigate(UmiDocumentCoordinator *coordinator,
    UmiDocumentId expectedDocument, const char *text, size_t byteCount,
    size_t *outOffset);

/** Select the next (+1) or previous (-1) managed source document in opening
 * order, wrapping at the ends. Not a most-recently-used or visual-tab-order list.
 * Dirty/read-only/pinned documents remain open; text, selection, history and
 * saved files are unchanged. A single source is a successful no-op.
 * No active managed source or an empty set returns NOT_FOUND. Invalid direction
 * returns INVALID_ARGUMENT. outDocument is optional, unchanged on failure.
 * Owner-thread only. Example: Cycle(coordinator, +1, NULL) switches Notes tabs. */
UmiStatus UmiDocumentCoordinatorCycle(UmiDocumentCoordinator *coordinator,
    int direction, UmiDocumentId *outDocument);
#ifdef __cplusplus
}
#endif
#endif
