/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/source_selection.h
 *
 * PURPOSE:
 *   Select a source word or line through the authoritative document and navigation owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_SOURCE_SELECTION_H
#define UMICOM_DOCUMENT_SOURCE_SELECTION_H
#include "umicom/document/coordinator.h"
#include "umicom/editor/source_selection.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Owner-thread operation. Capture current draft/caret of the named document,
 * resolve a range, verify the capture and publish through existing navigation.
 * A named inactive document becomes active only on success. Read-only is
 * allowed. Text, dirty state, text Undo and saved files remain unchanged;
 * normal source-navigation history records the jump. A missing word or invalid
 * caret leaves the current selection unchanged. No source provider is invoked. */
UmiStatus UmiDocumentCoordinatorSelectSourceUnit(UmiDocumentCoordinator *owner, UmiDocumentId id,
                                                 UmiEditorSourceSelectionKind kind);
UmiStatus UmiDocumentCoordinatorSelectActiveSourceUnit(UmiDocumentCoordinator *owner,
                                                       UmiEditorSourceSelectionKind kind);
typedef struct UmiDocumentSourceSelectionCommand {
  const char *id;
  const char *label;
  UmiEditorSourceSelectionKind kind;
} UmiDocumentSourceSelectionCommand;
/* Immutable shared catalogue; no mutation or command execution. */
size_t UmiDocumentSourceSelectionCommandCount(void);
const UmiDocumentSourceSelectionCommand *UmiDocumentSourceSelectionCommandAt(size_t index);
UmiStatus UmiDocumentSourceSelectionCommandFromId(const char *id,
                                                  UmiEditorSourceSelectionKind *out);
#ifdef __cplusplus
}
#endif
#endif
