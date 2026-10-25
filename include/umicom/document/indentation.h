/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/indentation.h
 *
 * PURPOSE:
 *   Expose document-owned indentation preferences and one history-preserving Tab operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_INDENTATION_H
#define UMICOM_DOCUMENT_INDENTATION_H
#include "umicom/document/coordinator.h"
#include "umicom/editor/indentation.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Owner-thread operations. The coordinator copies validated options, initially
 * four spaces. Updating preferences does not change text, history or files.
 * A changed policy is refused while a document save/reload/navigation is busy.
 * Reapplying an identical policy is a no-op. Get leaves output unchanged on error. */
UmiStatus UmiDocumentCoordinatorSetIndentation(UmiDocumentCoordinator *coordinator,
                                               const UmiEditorIndentation *options);
UmiStatus UmiDocumentCoordinatorGetIndentation(const UmiDocumentCoordinator *coordinator,
                                               UmiEditorIndentation *out_options);
/* With a selection, indent the selected lines without deleting selected text.
 * Without one, insert spaces to the next logical tab stop or one literal tab.
 * Read-only, foreign projections and busy owners refuse mutation. Existing
 * document history owns the single edit; inactive documents remain inactive.
 * No file is saved. Shift+Tab uses the existing OUTDENT_LINES operation. */
UmiStatus UmiDocumentCoordinatorInsertIndentation(UmiDocumentCoordinator *coordinator,
                                                  UmiDocumentId document_id);
#ifdef __cplusplus
}
#endif
#endif
