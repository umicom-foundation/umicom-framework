/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/source_selection.c
 *
 * PURPOSE:
 *   Compose immutable source requests and canonical range navigation for editor selection commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/source_selection.h"
#include "umicom/document/source_request.h"
UmiStatus UmiDocumentCoordinatorSelectSourceUnit(UmiDocumentCoordinator *owner, UmiDocumentId id,
                                                 UmiEditorSourceSelectionKind kind) {
  if (owner == NULL || id == 0U || kind < UMI_EDITOR_SELECT_SOURCE_WORD ||
      kind > UMI_EDITOR_SELECT_LINE_WITH_ENDING)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiDocumentSourceRequest *request = NULL;
  UmiDocumentSourceRequestSummary summary;
  UmiStatus status = UmiDocumentSourceRequestCreateInspection(owner, id, &request);
  const char *text = NULL;
  size_t bytes = 0U;
  if (status == UMI_STATUS_OK)
    status = UmiDocumentSourceRequestInspect(request, &summary);
  if (status == UMI_STATUS_OK)
    status = UmiDocumentSourceRequestRead(request, &text, &bytes);
  UmiEditorTextBufferView view = {0};
  view.struct_size = sizeof view;
  view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
  view.bytes = text;
  view.byte_count = bytes;
  view.capacity = bytes;
  UmiEditorSourceSelection range;
  UmiEditorTextPosition first, last;
  if (status == UMI_STATUS_OK)
    status = UmiEditorSourceSelectionAt(&view, summary.cursor_offset, kind, &range);
  if (status == UMI_STATUS_OK)
    status = UmiEditorTextViewPositionAt(&view, range.offset, &first);
  if (status == UMI_STATUS_OK)
    status = UmiEditorTextViewPositionAt(&view, range.offset + range.byte_count, &last);
  /* Selection is metadata owned by navigation, never a draft replacement.
   * Reuse its tab/history notifications so every frontend sees one result. */
  if (status == UMI_STATUS_OK)
    status = UmiDocumentSourceRequestCheck(owner, request);
  if (status == UMI_STATUS_OK)
    status = UmiDocumentCoordinatorSelectSourceRange(owner, id, first, last, NULL, NULL);
  UmiDocumentSourceRequestDestroy(request);
  return status;
}
UmiStatus UmiDocumentCoordinatorSelectActiveSourceUnit(UmiDocumentCoordinator *owner,
                                                       UmiEditorSourceSelectionKind kind) {
  if (owner == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiDocumentWorkingCopySnapshot active;
  UmiStatus status = umi_document_coordinator_active_snapshot(owner, &active);
  return status == UMI_STATUS_OK
             ? UmiDocumentCoordinatorSelectSourceUnit(owner, active.document_id, kind)
             : status;
}
