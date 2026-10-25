/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/copy_capture.inc
 *
 * PURPOSE:
 *   Capture authoritative visible text without synchronizing or retargeting the source document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "copy_internal.h"
UmiStatus UmiDocumentCoordinatorCaptureCopy(UmiDocumentCoordinator *coordinator,
                                            UmiDocumentId document, UmiDocumentCopy **outCopy) {
  if (outCopy == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *outCopy = NULL;
  if (coordinator == NULL || document == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
      coordinator->navigation_depth != 0U)
    return UMI_STATUS_BUSY;
  size_t index = EditDocumentIndex(coordinator, document);
  if (index == SIZE_MAX)
    return UMI_STATUS_NOT_FOUND;
  UmiDocumentSnapshot stored;
  UmiStatus status = umi_document_store_snapshot(coordinator->store, document, &stored);
  if (status != UMI_STATUS_OK)
    return status;
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
  UmiUiDocumentViewSnapshot view;
  status = umi_ui_document_view_model_find(views, coordinator->entries[index].view_id, &view);
  if (status != UMI_STATUS_OK)
    return status;
  char identity[UMI_UI_ID_CAPACITY];
  (void)snprintf(identity, sizeof identity, "document.%llu", (unsigned long long)document);
  if (strcmp(view.document_id, identity) != 0)
    return UMI_STATUS_INVALID_STATE;
  UmiDocumentCopyInfo info = {0};
  info.document_id = document;
  info.encoding = coordinator->entries[index].encoding;
  /* New virtual documents use UTF-8; unknown legacy format metadata must
   * never turn a copy into binary output. The normal Save uses this policy. */
  if (info.encoding == UMI_DOCUMENT_ENCODING_UNKNOWN)
    info.encoding = UMI_DOCUMENT_ENCODING_UTF8;
  if (stored.has_path)
    memcpy(info.source_path, stored.path, sizeof info.source_path);
  /* Store names use a smaller capacity than exported metadata. Copy text,
   * not the destination array size, to avoid reading beyond the store name. */
#if 0
    memcpy(info.display_name, stored.display_name, sizeof info.display_name);
#endif
  (void)snprintf(info.display_name, sizeof info.display_name, "%s", stored.display_name);
  char *text = NULL;
  size_t bytes = 0U;
  status = UmiUiDocumentViewModelCopyText(views, view.view_id, &text, &bytes);
  if (status == UMI_STATUS_OK)
    status = DocumentCopyCreate(&info, text, bytes, outCopy);
  UmiUiDocumentViewModelFreeText(text);
  return status;
}
