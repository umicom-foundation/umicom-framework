/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/language_comment.inc
 *
 * PURPOSE:
 *   Resolve authoritative view language before passing a literal prefix to the existing line-edit
 * transaction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/language_comment.h"
UmiStatus UmiDocumentCoordinatorLineCommentStyle(UmiDocumentCoordinator *owner, UmiDocumentId id,
                                                 const UmiEditorLineCommentStyle **out) {
  if (owner == NULL || id == 0U || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t index = EditDocumentIndex(owner, id);
  if (index == SIZE_MAX)
    return UMI_STATUS_NOT_FOUND;
  UmiUiDocumentViewSnapshot view;
  UmiStatus status = umi_ui_document_view_model_find(umi_ui_workbench_documents(owner->workbench),
                                                     owner->entries[index].view_id, &view);
  if (status != UMI_STATUS_OK)
    return status;
  char identity[UMI_UI_ID_CAPACITY];
  (void)snprintf(identity, sizeof identity, "document.%llu", (unsigned long long)id);
  if (strcmp(identity, view.document_id) != 0)
    return UMI_STATUS_INVALID_STATE;
  return UmiEditorLineCommentStyleFind(view.language_id, out);
}
UmiStatus UmiDocumentCoordinatorToggleLanguageComment(UmiDocumentCoordinator *owner,
                                                      UmiDocumentId id) {
  const UmiEditorLineCommentStyle *style = NULL;
  UmiStatus status = UmiDocumentCoordinatorLineCommentStyle(owner, id, &style);
  if (status != UMI_STATUS_OK)
    return status;
  UmiDocumentLineEditOptions options = {NULL, style->prefix};
  return UmiDocumentCoordinatorEditLinesForDocument(
      owner, id, UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT, &options);
}
