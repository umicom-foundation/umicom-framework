/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/newline.inc
 *
 * PURPOSE:
 *   Apply newline tokens to a captured document selection using the shared Undo owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/newline.h"
#include "umicom/editor/newline.h"
UmiStatus UmiDocumentCoordinatorInsertNewline(UmiDocumentCoordinator *coordinator,
                                              UmiDocumentId documentId, int inherit) {
  if (coordinator == NULL || documentId == 0U || (inherit != 0 && inherit != 1))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
      coordinator->navigation_depth != 0U)
    return UMI_STATUS_BUSY;
  UmiDocumentEditPlan *plan = NULL;
  UmiStatus status = UmiDocumentCoordinatorPrepareEdit(coordinator, documentId, &plan);
  if (status != UMI_STATUS_OK)
    return status;
  char identity[UMI_UI_ID_CAPACITY];
  (void)snprintf(identity, sizeof identity, "document.%llu", (unsigned long long)documentId);
  if (strcmp(plan->view.document_id, identity) != 0)
    status = UMI_STATUS_INVALID_STATE;
  else if (plan->view.read_only)
    status = UMI_STATUS_PERMISSION_DENIED;
  char *token = NULL;
  size_t length = 0U;
  if (status == UMI_STATUS_OK)
    status = UmiEditorNewlineCreate(plan->text, plan->length, plan->view.cursor_offset,
                                    plan->view.selection_length, inherit, &token, &length);
  if (status == UMI_STATUS_OK)
    status = UmiDocumentCoordinatorApplyEdit(coordinator, plan, token, length);
  UmiEditorNewlineFree(token);
  UmiDocumentEditPlanDestroy(plan);
  return status;
}
