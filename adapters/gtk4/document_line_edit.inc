/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_line_edit.inc
 * PURPOSE: Use the established native editing lifetime for atomic document-owned line commands.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/window_lifecycle.h"
UmiStatus UmiGtk4AdapterEditLines(UmiGtk4Adapter *adapter, UmiEditorEditCommandKind kind,
                                  const UmiDocumentLineEditOptions *options)
{
    if (adapter == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL || adapter->window == NULL ||
        !UmiGtk4WindowIsOpen(adapter->window))
        return UMI_STATUS_UNAVAILABLE;
    if (adapter->applying_document_state || adapter->edit_cancel != NULL ||
        UmiGtk4AdapterDocumentSaveAllBusy(adapter) || UmiGtk4AdapterDocumentCloseBusy(adapter) ||
        UmiGtk4AdapterReplacementReviewBusy(adapter) || UmiGtk4AdapterDocumentProposalBusy(adapter))
        return UMI_STATUS_BUSY;
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    UmiStatus status = UmiDocumentCoordinatorEditLines(adapter->edit_coordinator, kind, options);
    /* A refresh can detach its host. Resolve the binding again before the
     * existing completion route refreshes native source and notifies Studio. */
    adapter = g_object_get_data(lifetime, "adapter");
    if (adapter != NULL)
        status = EditFinished(adapter, status);
    g_object_unref(lifetime);
    return status;
}
