/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/selection_gtk4.c
 *
 * PURPOSE:
 *   Synchronise Framework selection state with the active GTK4 document tab.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

#include <string.h>

UmiStatus umi_gtk4_refresh_selection(UmiGtk4Adapter *adapter,
                                     UmiUiWorkbench *workbench)
{
    UmiUiSelectionModel *selection;
    char selected[UMI_UI_ID_CAPACITY];
    size_t index;
    UmiUiDocumentViewModel *documents;
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    selection = umi_ui_workbench_selection(workbench);
    if (umi_ui_selection_count(selection) == 0U ||
        umi_ui_selection_at(selection, 0U, selected, sizeof(selected)) != UMI_STATUS_OK) {
        return UMI_STATUS_OK;
    }
    documents = umi_ui_workbench_documents(workbench);
    for (index = 0U; index < umi_ui_document_view_model_count(documents); ++index) {
        UmiUiDocumentViewSnapshot item;
        if (umi_ui_document_view_model_at(documents, index, &item) == UMI_STATUS_OK &&
            strcmp(item.view_id, selected) == 0) {
            gtk_notebook_set_current_page(GTK_NOTEBOOK(adapter->document_notebook), (int)index);
            break;
        }
    }
    return UMI_STATUS_OK;
}
