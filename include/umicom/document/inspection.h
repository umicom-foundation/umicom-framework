/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/inspection.h
 *
 * PURPOSE:
 *   Capture complete visible source for read-only inspection without synchronizing or saving the
 * draft.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_INSPECTION_H
#define UMICOM_DOCUMENT_INSPECTION_H
#include "umicom/document/coordinator.h"
#include "umicom/editor/text_inspection.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDocumentInspection UmiDocumentInspection;
typedef struct UmiDocumentInspectionInfo {
  UmiDocumentId document_id;
  char display_name[UMI_DOCUMENT_NAME_CAPACITY];
  UmiDocumentTextEncoding encoding;
  int read_only;
} UmiDocumentInspectionInfo;
/** Capture the named document's complete visible UTF-8 draft and selection on
 * the coordinator owner thread. No save, sync, activation, history entry, file
 * read or provider call occurs. Read-only and untitled documents are supported.
 * The result owns all data and remains readable after the document or its owner
 * closes. It is a historical observation, not an edit or navigation token.
 * out must address NULL; failure preserves it. The text inspection's limits
 * and exact scalar/CRLF boundary requirements apply. */
UmiStatus UmiDocumentCoordinatorInspectText(UmiDocumentCoordinator *owner, UmiDocumentId id,
                                            UmiDocumentInspection **out);
UmiStatus UmiDocumentInspectionInfoRead(const UmiDocumentInspection *inspection,
                                        UmiDocumentInspectionInfo *out);
/** Borrow the immutable analysis until this document inspection is destroyed.
 * Do not destroy the borrowed object independently. */
const UmiEditorTextInspection *UmiDocumentInspectionText(const UmiDocumentInspection *inspection);
void UmiDocumentInspectionDestroy(UmiDocumentInspection *inspection);
#ifdef __cplusplus
}
#endif
#endif
