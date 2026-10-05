/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/search.h
 * PURPOSE: Expose explicit literal-search policy while retaining document selection and undo ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_SEARCH_H
#define UMICOM_DOCUMENT_SEARCH_H
#include "umicom/document/coordinator.h"
#include "umicom/editor/search_engine.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /** Navigate using explicit case and whole-word options. NULL preserves the
 * legacy smart-case default. Only case_mode and whole_word are used; the
 * match-count and overlap fields do not limit navigation. whole_word must be
 * zero or one. Word boundaries follow the shared editor rule: ASCII letters,
 * digits, underscore and non-ASCII bytes are part of a word. This is literal
 * text matching, not language-aware symbol search or a regular expression.
 * Both directions wrap once; outputs change only on success. No save or draft
 * synchronization occurs, and read-only documents remain navigable. */
    UmiStatus UmiDocumentCoordinatorFindWithOptions(UmiDocumentCoordinator *coordinator, const char *needle,
                                                    const UmiEditorSearchOptions *options, int backwards,
                                                    size_t *outOffset, int *outWrapped);
    /** Replace the selected complete match or navigate to and replace the next.
 * A selected substring still has to meet whole-word boundaries in the full
 * document. Uses the same case/word policy, existing writable checks and one
 * undoable document transaction. Empty replacement deletes the match. */
    UmiStatus UmiDocumentCoordinatorReplaceNextWithOptions(UmiDocumentCoordinator *coordinator,
                                                           const char *needle, const char *replacement,
                                                           const UmiEditorSearchOptions *options,
                                                           size_t *outOffset);
#ifdef __cplusplus
}
#endif
#endif
