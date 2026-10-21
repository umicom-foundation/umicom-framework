/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_line_edit.inc
 * PURPOSE: Use the established native editing lifetime for atomic document-owned line commands.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/window_lifecycle.h"
/* Native source commands now preserve the originating view identity. The active-only path remains available through the wrapper below; retain its previous implementation for review. */
#if 0
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

#endif

static UmiStatus DocumentEditLinesForView(UmiGtk4Adapter *adapter, const char *viewId,
                                  UmiEditorEditCommandKind kind,
                                  const UmiDocumentLineEditOptions *options)
{
    if (adapter == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL || adapter->window == NULL ||
        !UmiGtk4WindowIsOpen(adapter->window))
        return UMI_STATUS_UNAVAILABLE;
    if (adapter->applying_document_state || adapter->edit_cancel != NULL ||
        UmiGtk4AdapterDocumentSaveAllBusy(adapter) || UmiGtk4AdapterDocumentCloseBusy(adapter) ||
        UmiGtk4AdapterReplacementReviewBusy(adapter) || UmiGtk4AdapterDocumentProposalBusy(adapter) ||
        UmiGtk4AdapterCompletionReviewBusy(adapter))
        return EditFinished(adapter, UMI_STATUS_BUSY);
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    UmiDocumentId documentId = 0U;
    UmiStatus status = EditFindDocument(adapter, viewId, &documentId);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentCoordinatorEditLinesForDocument(adapter->edit_coordinator, documentId, kind, options);
    /* A refresh can detach its host. Resolve the binding again before the
     * existing completion route refreshes native source and notifies Studio. */
    adapter = g_object_get_data(lifetime, "adapter");
    if (adapter != NULL)
        status = EditFinished(adapter, status);
    g_object_unref(lifetime);
    return status;
}


UmiStatus UmiGtk4AdapterEditLines(UmiGtk4Adapter *adapter, UmiEditorEditCommandKind kind,
    const UmiDocumentLineEditOptions *options)
{
    return DocumentEditLinesForView(adapter, NULL, kind, options);
}
