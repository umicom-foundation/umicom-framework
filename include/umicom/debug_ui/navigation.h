/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_ui/navigation.h
 * PURPOSE: Open a copied stack-frame location through the shared document coordinator.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_UI_NAVIGATION_H
#define UMICOM_DEBUG_UI_NAVIGATION_H
#include "umicom/debug/stack_frame.h"
#include "umicom/document/coordinator.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Open local source after the host validates its captured debugger selection.
 * Uses one-based lines and UTF-8 byte columns; zero column means line start.
 * A relative path requires the session's absolute launch directory. Never uses
 * the host process working directory or starts a URI handler. A failed file
 * open leaves the previously active document and its caret unchanged. If the
 * file opens but the line is missing, its tab stays open and NOT_FOUND is
 * returned. Drafts are retained; no text is edited or saved. outOffset is
 * optional and changes only on success. This function does not contact DAP
 * or prove that the source still matches the compiled program. */
UmiStatus UmiDebugFrameOpenSource(UmiDocumentCoordinator *documents,
    const UmiDebugStackFrameSnapshot *frame, const char *baseDirectory,
    size_t *outOffset);
#ifdef __cplusplus
}
#endif
#endif
