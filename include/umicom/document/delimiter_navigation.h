/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/delimiter_navigation.h
 * PURPOSE: Navigate or select a delimiter pair through the authoritative draft and navigation history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_DELIMITER_NAVIGATION_H
#define UMICOM_DOCUMENT_DELIMITER_NAVIGATION_H
#include "umicom/document/source_navigation.h"
#include "umicom/editor/delimiter_navigation.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum UmiDocumentDelimiterAction
    {
        UMI_DOCUMENT_DELIMITER_MATCH = 0,
        UMI_DOCUMENT_DELIMITER_CONTENTS = 1,
        UMI_DOCUMENT_DELIMITER_PAIR = 2
    } UmiDocumentDelimiterAction;
    /* Use the identified draft's caret and complete unsaved text. MATCH jumps to
 * the opposite bracket at/before the caret. CONTENTS/PAIR select the innermost
 * complete enclosing pair, excluding/including its two delimiters.
 * Read-only drafts are supported. This owner-thread operation uses existing
 * navigation history and does not save, change text or create text Undo.
 * Existing selection length does not change the caret used for discovery.
 * The caller supplies the lexical profile explicitly; source is rechecked
 * before navigation. Keep the coordinator/store/workbench alive throughout. */
    UmiStatus UmiDocumentCoordinatorNavigateDelimiter(UmiDocumentCoordinator *coordinator,
                                                      UmiDocumentId document,
                                                      UmiDocumentDelimiterAction action,
                                                      UmiEditorDelimiterSyntax syntax);
    /* Use the active draft and its exact language identity. C and JSON choose
 * their lexical profiles; other identities use literal brackets. This is the
 * common entry point for command search, native toolbars and other hosts. */
    UmiStatus UmiDocumentCoordinatorNavigateActiveDelimiter(UmiDocumentCoordinator *coordinator,
                                                            UmiDocumentDelimiterAction action);
#ifdef __cplusplus
}
#endif
#endif
