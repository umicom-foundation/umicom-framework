/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_indentation.inc
 *
 * PURPOSE:
 *   Route native Tab intent through shared document admission and invalidatable editing lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/indentation.h"
static UmiStatus DocumentIndentationForView(UmiGtk4Adapter *adapter, const char *viewId) {
  if (adapter == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL ||
      adapter->window == NULL || !UmiGtk4WindowIsOpen(adapter->window))
    return UMI_STATUS_UNAVAILABLE;
  if (adapter->applying_document_state || adapter->edit_cancel != NULL ||
      UmiGtk4AdapterDocumentSaveAllBusy(adapter) || UmiGtk4AdapterDocumentCloseBusy(adapter) ||
      UmiGtk4AdapterReplacementReviewBusy(adapter) || UmiGtk4AdapterDocumentProposalBusy(adapter) ||
      UmiGtk4AdapterCompletionReviewBusy(adapter))
    return EditFinished(adapter, UMI_STATUS_BUSY);
  GObject *lifetime = g_object_ref(adapter->edit_lifetime);
  UmiDocumentId id = 0U;
  UmiStatus status = EditFindDocument(adapter, viewId, &id);
  if (status == UMI_STATUS_OK)
    status = UmiDocumentCoordinatorInsertIndentation(adapter->edit_coordinator, id);
  adapter = g_object_get_data(lifetime, "adapter");
  if (adapter != NULL)
    status = EditFinished(adapter, status);
  else
    status = UMI_STATUS_CANCELLED;
  g_object_unref(lifetime);
  return status;
}
