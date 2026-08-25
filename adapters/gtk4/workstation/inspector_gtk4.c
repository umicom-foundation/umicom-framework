/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/inspector_gtk4.c
 *
 * PURPOSE:
 *   Render generic property/inspector models as editable/read-only GTK4 property rows for designers and operational panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/inspector.h"

GtkWidget *umi_gtk4_ws_inspector_create(const UmiWsInspectorModel *model) {
    GtkWidget *grid = gtk_grid_new();
    size_t i;
    gtk_widget_add_css_class(grid, "umicom-inspector");
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    if (model == NULL) return grid;
    for (i = 0U; i < model->count; ++i) {
        GtkWidget *name = gtk_label_new(model->properties[i].label);
        GtkWidget *value = model->properties[i].editable ? gtk_entry_new() : gtk_label_new(model->properties[i].value);
        gtk_label_set_xalign(GTK_LABEL(name), 0.0F);
        if (model->properties[i].editable) gtk_editable_set_text(GTK_EDITABLE(value), model->properties[i].value);
        else gtk_label_set_xalign(GTK_LABEL(value), 0.0F);
        gtk_widget_set_hexpand(value, TRUE);
        gtk_grid_attach(GTK_GRID(grid), name, 0, (int)i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), value, 1, (int)i, 1, 1);
    }
    return grid;
}
