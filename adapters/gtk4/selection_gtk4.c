/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/selection_gtk4.c
 *
 * PURPOSE:
 *   Synchronise Framework selection state with the active GTK4 document tab.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
    int primary_page = 0;
    int secondary_page = 0;
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    selection = umi_ui_workbench_selection(workbench);
    if (umi_ui_selection_count(selection) == 0U ||
        umi_ui_selection_at(selection, 0U, selected, sizeof(selected)) != UMI_STATUS_OK) {
        return UMI_STATUS_OK;
    }
    documents = umi_ui_workbench_documents(workbench);
    for (index = 0U; index < umi_ui_document_view_model_count(documents); ++index) {
        UmiUiDocumentViewSnapshot item;
        if (umi_ui_document_view_model_at(documents, index, &item) ==
            UMI_STATUS_OK) {
            int is_secondary = strcmp(
                item.group_id, UMI_UI_SECONDARY_EDITOR_GROUP_ID) == 0;
            int page = is_secondary ? secondary_page : primary_page;
            if (strcmp(item.view_id, selected) == 0) {
                GtkWidget *notebook = is_secondary
                    ? adapter->secondary_document_notebook
                    : adapter->document_notebook;
                gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page);
                break;
            }
            if (is_secondary) {
                ++secondary_page;
            } else {
                ++primary_page;
            }
        }
    }
    return UMI_STATUS_OK;
}
