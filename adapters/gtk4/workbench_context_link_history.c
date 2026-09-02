/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_link_history.c
 *
 * PURPOSE:
 *   Render the reusable GTK4 context-link history surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/gtk4.h"

/*
 * Provide the workbench context link gtk4 history new operation used by this module and
 * its client applications.
 */
GtkWidget *umi_workbench_context_link_gtk4_history_new(
    const UmiWorkbenchContextLinkService *service)
{
    GtkWidget *list = gtk_list_box_new();
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return list;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < service->history.count; ++index) {
        const UmiWorkbenchContextLinkHistoryItem *item = &service->history.items[index];
        GtkWidget *row = gtk_label_new(item->payload.identity.context_id);
        gtk_widget_set_tooltip_text(row, item->group_id);
        gtk_list_box_append(GTK_LIST_BOX(list), row);
    }
    return list;
}
