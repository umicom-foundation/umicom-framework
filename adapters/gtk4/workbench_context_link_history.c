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

GtkWidget *umi_workbench_context_link_gtk4_history_new(
    const UmiWorkbenchContextLinkService *service)
{
    GtkWidget *list = gtk_list_box_new();
    size_t index;
    if (service == NULL) return list;
    for (index = 0U; index < service->history.count; ++index) {
        const UmiWorkbenchContextLinkHistoryItem *item = &service->history.items[index];
        GtkWidget *row = gtk_label_new(item->payload.identity.context_id);
        gtk_widget_set_tooltip_text(row, item->group_id);
        gtk_list_box_append(GTK_LIST_BOX(list), row);
    }
    return list;
}
