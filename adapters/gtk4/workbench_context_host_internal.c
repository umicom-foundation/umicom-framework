/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_host_internal.c
 *
 * PURPOSE:
 *   Implement private GTK4 child-management and semantic colour CSS helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workbench_context_host_internal.h"

/* Detach only this borrowed context owner. A caller may retain old buttons
 * after their parent is refreshed or the service has been closed. */
void umi_workbench_context_host_gtk4_invalidate(
    GtkWidget *root, UmiWorkbenchContextHost *host)
{
    GtkWidget *child;
    if (root == NULL || host == NULL) return;
    g_signal_handlers_disconnect_by_data(root, host);
    if (g_object_get_data(G_OBJECT(root), UMI_WCH_DATA_HOST) == host)
        g_object_set_data(G_OBJECT(root), UMI_WCH_DATA_HOST, NULL);
    gtk_widget_set_sensitive(root, FALSE);
    for (child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        umi_workbench_context_host_gtk4_invalidate(child, host);
}
/*
 * Provide the wch colour css class operation used by this module and its client
 * applications.
 */
const char *umi_wch_colour_css_class(UmiContextChannelColour colour)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch(colour){
    case UMI_CONTEXT_COLOUR_RED:return "umicom-context-group-red";
    case UMI_CONTEXT_COLOUR_ORANGE:return "umicom-context-group-orange";
    case UMI_CONTEXT_COLOUR_YELLOW:return "umicom-context-group-yellow";
    case UMI_CONTEXT_COLOUR_GREEN:return "umicom-context-group-green";
    case UMI_CONTEXT_COLOUR_CYAN:return "umicom-context-group-cyan";
    case UMI_CONTEXT_COLOUR_BLUE:return "umicom-context-group-blue";
    case UMI_CONTEXT_COLOUR_PURPLE:return "umicom-context-group-purple";
    case UMI_CONTEXT_COLOUR_MAGENTA:return "umicom-context-group-magenta";
    default:return "umicom-context-group-none";
    }
}
/*
 * Provide the wch clear children operation used by this module and its client
 * applications.
 */
void umi_wch_clear_children(GtkWidget *widget)
{
    GtkWidget *child;/* Apply this branch only when its contract condition is satisfied. */ if(!GTK_IS_BOX(widget))return;
    child=gtk_widget_get_first_child(widget);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while(child){GtkWidget *next=gtk_widget_get_next_sibling(child);
        /* A removed picker button can outlive its parent when an accessibility
         * or acceptance client retains it; detach its borrowed service first. */
        umi_workbench_context_host_gtk4_invalidate(child,
            g_object_get_data(G_OBJECT(widget), UMI_WCH_DATA_HOST));
        gtk_box_remove(GTK_BOX(widget),child);child=next;}
}
