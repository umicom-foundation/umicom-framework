/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_source_selection.inc
 *
 * PURPOSE:
 *   Keep source selection completion on the captured native editing lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/source_selection.h"
UmiStatus UmiGtk4AdapterSelectSourceUnit(UmiGtk4Adapter *adapter,
                                         UmiEditorSourceSelectionKind kind) {
  if (adapter == NULL || kind < UMI_EDITOR_SELECT_SOURCE_WORD ||
      kind > UMI_EDITOR_SELECT_LINE_WITH_ENDING)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL ||
      adapter->window == NULL || !UmiGtk4WindowIsOpen(adapter->window))
    return UMI_STATUS_UNAVAILABLE;
  if (adapter->applying_document_state)
    return UMI_STATUS_BUSY;
  GObject *lifetime = g_object_ref(adapter->edit_lifetime);
  UmiStatus status = UmiDocumentCoordinatorSelectActiveSourceUnit(adapter->edit_coordinator, kind);
  /* Completion can close the host. Never dereference it after that callback. */
  adapter = g_object_get_data(lifetime, "adapter");
  if (adapter != NULL)
    status = EditFinished(adapter, status);
  g_object_unref(lifetime);
  return status;
}
