/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/selection_replace.h
 *
 * PURPOSE:
 *   Prepare a reviewed literal replacement limited to the captured source selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_SELECTION_REPLACE_H
#define UMICOM_DOCUMENT_SELECTION_REPLACE_H
#include "umicom/document/proposal.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Prepare a nonempty selected range with exact whole-document context. Empty
 * selection is rejected; no-match produces an unchanged proposal. NULL options
 * retain document smart-case defaults. Needle/replacement must be UTF-8 strings.
 * Selection endpoints must preserve scalar and CRLF boundaries. No navigation,
 * history, save or synchronization occurs. Failure clears out and leaves count.
 * Use the existing Proposal inspect/text/check/apply/destroy API thereafter.
 * The proposal is already populated, but no approval or Apply is implicit. */
UmiStatus UmiDocumentPrepareSelectionReplacement(UmiDocumentCoordinator *owner, UmiDocumentId id,
                                                 const char *needle, const char *replacement,
                                                 const UmiEditorSearchOptions *options,
                                                 UmiDocumentProposal **out, size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
