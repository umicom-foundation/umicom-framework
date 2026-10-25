/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/text_inspection.inc
 *
 * PURPOSE:
 *   Bind an immutable text observation to authoritative document identity without retaining the
 * coordinator.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/inspection.h"
struct UmiDocumentInspection {
  UmiDocumentInspectionInfo info;
  UmiEditorTextInspection *text;
};
void UmiDocumentInspectionDestroy(UmiDocumentInspection *value) {
  if (value != NULL) {
    UmiEditorTextInspectionDestroy(value->text);
    free(value);
  }
}
UmiStatus UmiDocumentInspectionInfoRead(const UmiDocumentInspection *value,
                                        UmiDocumentInspectionInfo *out) {
  if (value == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = value->info;
  return UMI_STATUS_OK;
}
const UmiEditorTextInspection *UmiDocumentInspectionText(const UmiDocumentInspection *value) {
  return value != NULL ? value->text : NULL;
}
UmiStatus UmiDocumentCoordinatorInspectText(UmiDocumentCoordinator *owner, UmiDocumentId id,
                                            UmiDocumentInspection **out) {
  if (owner == NULL || id == 0U || out == NULL || *out != NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (owner->save_in_progress || owner->reopen_in_progress || owner->navigation_depth != 0U)
    return UMI_STATUS_BUSY;
  size_t index = EditDocumentIndex(owner, id);
  if (index == SIZE_MAX)
    return UMI_STATUS_NOT_FOUND;
  UmiDocumentEditPlan *capture = NULL;
  UmiStatus status = UmiDocumentCoordinatorPrepareEdit(owner, id, &capture);
  if (status != UMI_STATUS_OK)
    return status;
  char identity[UMI_UI_ID_CAPACITY];
  (void)snprintf(identity, sizeof identity, "document.%llu", (unsigned long long)id);
  if (strcmp(capture->view.document_id, identity) != 0)
    status = UMI_STATUS_INVALID_STATE;
  UmiDocumentInspection *value = calloc(1U, sizeof *value);
  if (value == NULL)
    status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK)
    status =
        UmiEditorTextInspectionCreate(capture->text, capture->length, capture->view.cursor_offset,
                                      capture->view.selection_length, NULL, &value->text);
  if (status == UMI_STATUS_OK) {
    value->info.document_id = id;
    value->info.encoding = owner->entries[index].encoding;
    value->info.read_only = capture->view.read_only;
    (void)snprintf(value->info.display_name, sizeof value->info.display_name, "%s",
                   capture->stored.display_name);
    *out = value;
  } else
    UmiDocumentInspectionDestroy(value);
  UmiDocumentEditPlanDestroy(capture);
  return status;
}
