/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/newline.h
 *
 * PURPOSE:
 *   Expose newline insertion through the existing document transaction and history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_NEWLINE_H
#define UMICOM_DOCUMENT_NEWLINE_H
#include "umicom/document/coordinator.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Replace the named draft's selection with its line ending and optional
 * literal leading whitespace. The caret finishes after the inserted token.
 * One shared history edit, no file writes and no activation of another tab.
 * Read-only, busy and invalid identity/state refuse without changing text.
 * Use on the coordinator's owning thread. inherit_indentation is zero or one. */
UmiStatus UmiDocumentCoordinatorInsertNewline(UmiDocumentCoordinator *coordinator,
                                              UmiDocumentId document_id, int inherit_indentation);
#ifdef __cplusplus
}
#endif
#endif
