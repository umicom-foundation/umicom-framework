/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/context_channel_badge.c
 *
 * PURPOSE:
 *   Render the context channel badge using GTK4 while retaining Framework-owned state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include "umicom/context_channel/context_channel.h"
/*
 * Provide the gtk4 context channel badge new operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_context_channel_badge_new(const char *title,const char *detail)
{
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
    GtkWidget *label=gtk_label_new(title!=NULL?title:"Context");
    GtkWidget *description=gtk_label_new(detail!=NULL?detail:"");
    gtk_label_set_xalign(GTK_LABEL(label),0.0F);
    gtk_label_set_xalign(GTK_LABEL(description),0.0F);
    gtk_widget_add_css_class(box,"umicom-context-surface");
    gtk_widget_add_css_class(label,"umicom-context-title");
    gtk_widget_add_css_class(description,"umicom-context-detail");
    gtk_box_append(GTK_BOX(box),label);
    gtk_box_append(GTK_BOX(box),description);
    return box;
}
