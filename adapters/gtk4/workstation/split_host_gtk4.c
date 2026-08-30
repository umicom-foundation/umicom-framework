/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/split_host_gtk4.c
 *
 * PURPOSE:
 *   Render semantic split regions as GTK4 paned hosts with ratio-aware child placement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/split_host.h"

GtkWidget *umi_gtk4_ws_split_host_create(const UmiWsSplitRegion *split, UmiUiOrientation orientation, GtkWidget *first, GtkWidget *second) {
    GtkOrientation gtk_orientation = orientation == UMI_UI_VERTICAL ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL;
    GtkWidget *paned = gtk_paned_new(gtk_orientation);
    int initial = 500;
    gtk_widget_add_css_class(paned, "umicom-split-host");
    if (split != NULL) initial = (int)(umi_ws_clamp_unit(split->ratio) * 1000.0);
    if (first != NULL) gtk_paned_set_start_child(GTK_PANED(paned), first);
    if (second != NULL) gtk_paned_set_end_child(GTK_PANED(paned), second);
    gtk_paned_set_resize_start_child(GTK_PANED(paned), TRUE);
    gtk_paned_set_resize_end_child(GTK_PANED(paned), TRUE);
    gtk_paned_set_position(GTK_PANED(paned), initial);
    return paned;
}
