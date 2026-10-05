/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_delimiter_navigation.inc
 * PURPOSE: Route native delimiter navigation through captured source and the document editing lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/window_lifecycle.h"
/* Language-profile dispatch moved to the document owner so command search and native hosts use one policy.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiGtk4AdapterNavigateDelimiter(UmiGtk4Adapter *adapter, UmiDocumentDelimiterAction action)
{
    if (adapter == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL || adapter->window == NULL ||
        !UmiGtk4WindowIsOpen(adapter->window))
        return UMI_STATUS_UNAVAILABLE;
    if (adapter->applying_document_state)
        return UMI_STATUS_BUSY;
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    UmiDocumentWorkingCopySnapshot document;
    UmiEditorDelimiterSyntax syntax = UMI_EDITOR_DELIMITER_LITERAL;
    UmiStatus status = umi_document_coordinator_active_snapshot(adapter->edit_coordinator, &document);
    if (status == UMI_STATUS_OK)
    {
        /* Do not infer a compiler grammar from a filename. Known C and JSON
         * identities receive their lexical profile; other languages keep
         * explicit literal matching until an appropriate profile is added. */
        if (strcmp(document.language_id, "c") == 0)
            syntax = UMI_EDITOR_DELIMITER_C;
        else if (strcmp(document.language_id, "json") == 0)
            syntax = UMI_EDITOR_DELIMITER_JSON;
        status = UmiDocumentCoordinatorNavigateDelimiter(adapter->edit_coordinator, document.document_id,
                                                         action, syntax);
    }
    adapter = g_object_get_data(lifetime, "adapter");
    if (adapter != NULL)
        status = EditFinished(adapter, status);
    g_object_unref(lifetime);
    return status;
}
#endif
UmiStatus UmiGtk4AdapterNavigateDelimiter(UmiGtk4Adapter *adapter, UmiDocumentDelimiterAction action)
{
    if (adapter == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL || adapter->window == NULL ||
        !UmiGtk4WindowIsOpen(adapter->window))
        return UMI_STATUS_UNAVAILABLE;
    if (adapter->applying_document_state)
        return UMI_STATUS_BUSY;
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    /* The shared document owner chooses the language profile so command
     * search and this native action cannot develop different matching rules. */
    UmiStatus status = UmiDocumentCoordinatorNavigateActiveDelimiter(adapter->edit_coordinator, action);
    adapter = g_object_get_data(lifetime, "adapter");
    if (adapter != NULL)
        status = EditFinished(adapter, status);
    g_object_unref(lifetime);
    return status;
}
