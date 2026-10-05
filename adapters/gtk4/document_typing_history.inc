/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_typing_history.inc
 * PURPOSE: Bridge native typing captures to the active shared document binding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

UmiStatus UmiGtk4EditorCaptureTyping(UmiGtk4Adapter *adapter, const char *view_id,
                                     UmiDocumentEditPlan **out_capture)
{
    if (out_capture == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_capture = NULL;
    UmiDocumentId id = 0U;
    UmiStatus status = EditFindDocument(adapter, view_id, &id);
    return status == UMI_STATUS_OK
               ? UmiDocumentCoordinatorPrepareEdit(adapter->edit_coordinator, id, out_capture)
               : status;
}
UmiStatus UmiGtk4EditorCommitTyping(UmiGtk4Adapter *adapter, UmiDocumentEditPlan *capture)
{
    if (adapter == NULL || adapter->edit_coordinator == NULL)
        return UMI_STATUS_UNAVAILABLE;
    /* Coordinator ownership in the capture rejects rebinding during an action.
     * Native typing already updated the buffer: do not force a buffer refresh
     * while GTK is still dispatching end-user-action. */
    return UmiDocumentCoordinatorCommitTyping(adapter->edit_coordinator, capture);
}
