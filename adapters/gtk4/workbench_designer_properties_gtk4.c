/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_designer_properties_gtk4.c
 *
 * PURPOSE:
 *   Render stable property groups and values in a GTK4 grid suitable for later
 *   typed editor controls.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "workbench_designer_gtk4_internal.h"


GtkWidget *umi_workbench_designer_gtk4_build_properties(
    UmiWorkbenchDesignerGtk4 *designer)
{
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *heading = gtk_label_new("Properties");
    gtk_label_set_xalign(GTK_LABEL(heading), 0.0F);
    gtk_widget_add_css_class(heading, "heading");
    designer->property_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(designer->property_grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(designer->property_grid), 5);
    gtk_box_append(GTK_BOX(root), heading);
    gtk_box_append(GTK_BOX(root), designer->property_grid);
    return root;
}

static void clear_grid(GtkWidget *grid)
{
    GtkWidget *child = gtk_widget_get_first_child(grid);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_grid_remove(GTK_GRID(grid), child);
        child = next;
    }
}

void umi_workbench_designer_gtk4_refresh_properties(
    UmiWorkbenchDesignerGtk4 *designer)
{
    UmiWorkbenchDesignerSession *session;
    const UmiWorkbenchDesignerPropertyModel *model;
    size_t index;
    if (designer == NULL || designer->property_grid == NULL) return;
    clear_grid(designer->property_grid);
    session = umi_workbench_designer_service_active(
        designer->config.controller->service);
    if (session == NULL) return;
    model = umi_workbench_designer_session_properties(session);
    if (model == NULL) return;
    for (index = 0U; index < model->count; ++index) {
        const UmiWorkbenchDesignerProperty *property = &model->properties[index];
        GtkWidget *label = gtk_label_new(property->label);
        GtkWidget *value;
        char number[64];
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        if (property->kind == UMI_WORKBENCH_DESIGNER_PROPERTY_BOOLEAN) {
            value = gtk_check_button_new();
            gtk_check_button_set_active(
                GTK_CHECK_BUTTON(value), property->boolean_value ? TRUE : FALSE);
        } else if (property->kind == UMI_WORKBENCH_DESIGNER_PROPERTY_NUMBER) {
            g_snprintf(number, sizeof(number), "%.3f", property->number_value);
            value = gtk_entry_new();
            gtk_editable_set_text(GTK_EDITABLE(value), number);
        } else {
            value = gtk_entry_new();
            gtk_editable_set_text(GTK_EDITABLE(value), property->text_value);
        }
        gtk_widget_set_sensitive(value, property->editable ? TRUE : FALSE);
        gtk_grid_attach(GTK_GRID(designer->property_grid), label, 0, (int)index, 1, 1);
        gtk_grid_attach(GTK_GRID(designer->property_grid), value, 1, (int)index, 1, 1);
    }
}
