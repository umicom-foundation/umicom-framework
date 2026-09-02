/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_link_panel_host.c
 *
 * PURPOSE:
 *   Render the reusable GTK4 context-link panel host surface.
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
 * Provide the workbench context link gtk4 panel host new operation used by this module and
 * its client applications.
 */
GtkWidget *umi_workbench_context_link_gtk4_panel_host_new(
    const char *panel_title,
    const UmiWorkbenchContextLinkGroupProfile *profile,
    GtkWidget *content)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = umi_workbench_context_link_gtk4_panel_header_new(
        panel_title, profile);
    gtk_box_append(GTK_BOX(box), header);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (content != NULL) gtk_box_append(GTK_BOX(box), content);
    return box;
}
