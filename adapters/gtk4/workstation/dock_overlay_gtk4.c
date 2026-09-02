/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/dock_overlay_gtk4.c
 *
 * PURPOSE:
 *   Render visual docking targets and drop-preview geometry for the Framework docking/edit-layout workflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/dock_overlay.h"

/*
 * Initialise gtk4 ws dock overlay from caller-provided values so later operations receive
 * a known state.
 */
GtkWidget *umi_gtk4_ws_dock_overlay_create(GtkWidget *content) {
    GtkWidget *overlay = gtk_overlay_new();
    gtk_widget_add_css_class(overlay, "umicom-dock-overlay");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (content != NULL) gtk_overlay_set_child(GTK_OVERLAY(overlay), content);
    return overlay;
}

/*
 * Provide the gtk4 ws dock overlay set preview operation used by this module and its
 * client applications.
 */
UmiStatus umi_gtk4_ws_dock_overlay_set_preview(GtkWidget *overlay, const UmiWsDockPreview *preview, const char *label) {
    GtkWidget *indicator;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (overlay == NULL || !GTK_IS_OVERLAY(overlay) || preview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    indicator = gtk_label_new(label != NULL ? label : umi_ws_dock_region_text(preview->region));
    gtk_widget_add_css_class(indicator, "umicom-dock-preview");
    gtk_widget_set_halign(indicator, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(indicator, GTK_ALIGN_CENTER);
    gtk_widget_set_opacity(indicator, preview->visible ? preview->opacity : 0.0);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), indicator);
    return UMI_STATUS_OK;
}
