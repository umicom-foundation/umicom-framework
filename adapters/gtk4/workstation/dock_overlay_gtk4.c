/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/dock_overlay_gtk4.c
 *
 * PURPOSE:
 *   Render visual docking targets and drop-preview geometry for the Framework docking/edit-layout workflow.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/dock_overlay.h"

GtkWidget *umi_gtk4_ws_dock_overlay_create(GtkWidget *content) {
    GtkWidget *overlay = gtk_overlay_new();
    gtk_widget_add_css_class(overlay, "umicom-dock-overlay");
    if (content != NULL) gtk_overlay_set_child(GTK_OVERLAY(overlay), content);
    return overlay;
}

UmiStatus umi_gtk4_ws_dock_overlay_set_preview(GtkWidget *overlay, const UmiWsDockPreview *preview, const char *label) {
    GtkWidget *indicator;
    if (overlay == NULL || !GTK_IS_OVERLAY(overlay) || preview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    indicator = gtk_label_new(label != NULL ? label : umi_ws_dock_region_text(preview->region));
    gtk_widget_add_css_class(indicator, "umicom-dock-preview");
    gtk_widget_set_halign(indicator, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(indicator, GTK_ALIGN_CENTER);
    gtk_widget_set_opacity(indicator, preview->visible ? preview->opacity : 0.0);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), indicator);
    return UMI_STATUS_OK;
}
