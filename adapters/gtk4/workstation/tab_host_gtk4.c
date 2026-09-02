/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/tab_host_gtk4.c
 *
 * PURPOSE:
 *   Render semantic tab stacks as GTK4 notebook hosts with reusable document/tool-surface tab behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/tab_host.h"

/*
 * Initialise gtk4 ws tab host from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_tab_host_create(const UmiWsTabStack *stack) {
    GtkWidget *notebook = gtk_notebook_new();
    gtk_widget_add_css_class(notebook, "umicom-tab-host");
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
    gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stack != NULL && stack->count > 0U && stack->active_index < stack->count)
        gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), (int)stack->active_index);
    return notebook;
}

/* Add gtk4 ws tab host only after its inputs and available capacity have been checked. */
UmiStatus umi_gtk4_ws_tab_host_append(GtkWidget *host, const char *label, GtkWidget *child) {
    GtkWidget *tab_label;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL || !GTK_IS_NOTEBOOK(host) || label == NULL || child == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    tab_label = gtk_label_new(label);
    (void)gtk_notebook_append_page(GTK_NOTEBOOK(host), child, tab_label);
    return UMI_STATUS_OK;
}
