/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/editor_folding_controls.inc
 * PURPOSE: Route Studio and other editor hosts to buffer-owned folding without a second document service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/text_folding.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
static UmiStatus EditorFoldingActive(UmiGtk4Adapter *adapter, int reveal)
{
    if (adapter == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->shell == NULL || adapter->window == NULL ||
        !UmiGtk4WindowIsOpen(adapter->window))
        return UMI_STATUS_UNAVAILABLE;
    if (adapter->applying_document_state)
        return UMI_STATUS_BUSY;
    if (!GTK_IS_NOTEBOOK(adapter->document_notebook) ||
        !GTK_IS_NOTEBOOK(adapter->secondary_document_notebook))
        return UMI_STATUS_UNAVAILABLE;
    UmiDocumentWorkingCopySnapshot source;
    UmiStatus status = umi_document_coordinator_active_snapshot(adapter->edit_coordinator, &source);
    if (status != UMI_STATUS_OK)
        return status;
    GtkWidget *notebook = NULL, *page = find_document_page(adapter, source.view_id, &notebook);
    GtkWidget *view = page == NULL ? NULL : g_object_get_data(G_OBJECT(page), "umicom-editor-view");
    if (!GTK_IS_TEXT_VIEW(view))
        return UMI_STATUS_NOT_FOUND;
    /* The generic service retains its native view/buffer before notifications.
     * Do not access the borrowed adapter again after this call: a host observer
     * may close the editor while its caret or presentation changes. */
    return reveal ? UmiGtk4TextFoldClear(GTK_TEXT_VIEW(view)) : UmiGtk4TextFoldSelection(GTK_TEXT_VIEW(view));
}
UmiStatus UmiGtk4AdapterFoldSelectedCode(UmiGtk4Adapter *adapter) { return EditorFoldingActive(adapter, 0); }
UmiStatus UmiGtk4AdapterRevealFoldedCode(UmiGtk4Adapter *adapter) { return EditorFoldingActive(adapter, 1); }
