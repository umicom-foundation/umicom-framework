/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/editor_gtk4.c
 *
 * PURPOSE:
 *   Map toolkit-neutral document tabs to GTK4 notebook pages using standard text
 *   views as the reference editor widget.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

UmiStatus umi_gtk4_refresh_documents(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench)
{
    UmiUiDocumentViewModel *documents;
    int pages;
    size_t index;
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(adapter->document_notebook));
    while (pages > 0) {
        gtk_notebook_remove_page(GTK_NOTEBOOK(adapter->document_notebook), 0);
        pages -= 1;
    }
    documents = umi_ui_workbench_documents(workbench);
    for (index = 0U; index < umi_ui_document_view_model_count(documents); ++index) {
        UmiUiDocumentViewSnapshot document;
        if (umi_ui_document_view_model_at(documents, index, &document) == UMI_STATUS_OK) {
            GtkWidget *view = gtk_text_view_new();
            GtkWidget *scroll = gtk_scrolled_window_new();
            GtkWidget *tab = gtk_label_new(document.title);
            gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
            (void)gtk_notebook_append_page(GTK_NOTEBOOK(adapter->document_notebook), scroll, tab);
            if (document.active) {
                gtk_notebook_set_current_page(GTK_NOTEBOOK(adapter->document_notebook), (int)index);
            }
        }
    }
    return UMI_STATUS_OK;
}
