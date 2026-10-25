/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/indentation.inc
 *
 * PURPOSE:
 *   Keep indentation preferences with the document owner and apply Tab through existing
 * captured-edit transactions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

UmiStatus UmiDocumentCoordinatorSetIndentation(UmiDocumentCoordinator *coordinator,
                                               const UmiEditorIndentation *options) {
  if (coordinator == NULL || UmiEditorIndentationValidate(options) != UMI_STATUS_OK)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (coordinator->indentation.tab_width == options->tab_width &&
      coordinator->indentation.insert_spaces == options->insert_spaces)
    return UMI_STATUS_OK;
  if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
      coordinator->navigation_depth != 0U)
    return UMI_STATUS_BUSY;
  /* Count policy changes so an older document review cannot survive an ABA change. */
  if (coordinator->indentation_revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
  coordinator->indentation = *options;
  ++coordinator->indentation_revision;
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentCoordinatorGetIndentation(const UmiDocumentCoordinator *coordinator,
                                               UmiEditorIndentation *out) {
  if (coordinator == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = coordinator->indentation;
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentCoordinatorInsertIndentation(UmiDocumentCoordinator *coordinator,
                                                  UmiDocumentId documentId) {
  if (coordinator == NULL || documentId == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
      coordinator->navigation_depth != 0U)
    return UMI_STATUS_BUSY;
  size_t index = EditDocumentIndex(coordinator, documentId);
  if (index == SIZE_MAX)
    return UMI_STATUS_NOT_FOUND;
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
  else if (plan->view.selection_length != 0U) {
    /* Tab on a selection is a line command, never a destructive replacement. */
    UmiDocumentEditPlanDestroy(plan);
    return UmiDocumentCoordinatorEditLinesForDocument(coordinator, documentId,
                                                      UMI_EDITOR_EDIT_COMMAND_INDENT_LINES, NULL);
  }
  char token[UMI_EDITOR_INDENTATION_MAXIMUM_WIDTH + 1U];
  size_t bytes = 0U;
  /* The former global-only lookup is retained for comparison. Named working
   * copies now resolve their override through the same owner as Tab history. */
#if 0
  if (status == UMI_STATUS_OK)
    status = UmiEditorIndentationAt(&coordinator->indentation, plan->text, plan->length,
                                    plan->view.cursor_offset, token, sizeof token, &bytes);
#endif
  UmiDocumentIndentationReview indentation;
  if (status == UMI_STATUS_OK)
    status = UmiDocumentCoordinatorInspectIndentation(coordinator, documentId, &indentation);
  if (status == UMI_STATUS_OK)
    status = UmiEditorIndentationAt(&indentation.effective, plan->text, plan->length,
                                    plan->view.cursor_offset, token, sizeof token, &bytes);
  if (status == UMI_STATUS_OK)
    status = UmiDocumentCoordinatorApplyEdit(coordinator, plan, token, bytes);
  UmiDocumentEditPlanDestroy(plan);
  return status;
}
