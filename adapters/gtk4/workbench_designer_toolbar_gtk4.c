/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_designer_toolbar_gtk4.c
 *
 * PURPOSE:
 *   Build the visual designer toolbar and project Framework command availability
 *   into GTK4 controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workbench_designer_gtk4_internal.h"


/*
 * Provide the toolbar button clicked operation used by this module and its client
 * applications.
 */
static void toolbar_button_clicked(GtkButton *button, gpointer user_data)
{
    UmiWorkbenchDesignerGtk4 *designer = user_data;
    const char *command_id = g_object_get_data(
        G_OBJECT(button), "umicom-command-id");
    umi_workbench_designer_gtk4_dispatch(designer, command_id);
}

/* Provide the toolbar button operation used by this module and its client applications. */
static GtkWidget *toolbar_button(
    UmiWorkbenchDesignerGtk4 *designer,
    const char *label,
    const char *command_id,
    const char *tooltip)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    g_object_set_data_full(
        G_OBJECT(button), "umicom-command-id",
        g_strdup(command_id), g_free);
    gtk_widget_set_tooltip_text(button, tooltip);
    g_signal_connect(
        button, "clicked", G_CALLBACK(toolbar_button_clicked), designer);
    return button;
}

/*
 * Provide the workbench designer gtk4 build toolbar operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_designer_gtk4_build_toolbar(
    UmiWorkbenchDesignerGtk4 *designer)
{
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_add_css_class(toolbar, "toolbar");
    gtk_box_append(GTK_BOX(toolbar), toolbar_button(
        designer, "Browser", "designer.browser", "Show the Layout Browser"));
    gtk_box_append(GTK_BOX(toolbar), gtk_separator_new(GTK_ORIENTATION_VERTICAL));
    gtk_box_append(GTK_BOX(toolbar), toolbar_button(
        designer, "Select", "designer.tool.select", "Select and move panels"));
    gtk_box_append(GTK_BOX(toolbar), toolbar_button(
        designer, "Split H", "designer.split-horizontal", "Split horizontally"));
    gtk_box_append(GTK_BOX(toolbar), toolbar_button(
        designer, "Split V", "designer.split-vertical", "Split vertically"));
    gtk_box_append(GTK_BOX(toolbar), toolbar_button(
        designer, "Tabs", "designer.tab-group", "Create a tab group"));
    gtk_box_append(GTK_BOX(toolbar), toolbar_button(
        designer, "Float", "designer.float", "Float the selected panel"));
    gtk_box_append(GTK_BOX(toolbar), gtk_separator_new(GTK_ORIENTATION_VERTICAL));
    gtk_box_append(GTK_BOX(toolbar), toolbar_button(
        designer, "Undo", "designer.undo", "Undo the last layout edit"));
    gtk_box_append(GTK_BOX(toolbar), toolbar_button(
        designer, "Redo", "designer.redo", "Redo the last undone edit"));
    gtk_box_append(GTK_BOX(toolbar), toolbar_button(
        designer, "Save", "designer.save", "Save through the Data Server"));
    gtk_box_append(GTK_BOX(toolbar), toolbar_button(
        designer, "Preview", "designer.preview", "Preview the active layout"));
    return toolbar;
}

/*
 * Provide the workbench designer gtk4 refresh toolbar operation used by this module and
 * its client applications.
 */
void umi_workbench_designer_gtk4_refresh_toolbar(
    UmiWorkbenchDesignerGtk4 *designer)
{
    GtkWidget *child;
    bool mutating_enabled;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL || designer->toolbar == NULL) return;
    mutating_enabled = designer->bundle.validation_gate.can_preview;
    child = gtk_widget_get_first_child(designer->toolbar);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (child != NULL) {
        const char *command_id = g_object_get_data(
            G_OBJECT(child), "umicom-command-id");
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (command_id != NULL &&
            (g_str_has_prefix(command_id, "designer.split") ||
             g_str_has_prefix(command_id, "designer.tab") ||
             g_str_has_prefix(command_id, "designer.float"))) {
            gtk_widget_set_sensitive(child, mutating_enabled ? TRUE : FALSE);
        }
        child = gtk_widget_get_next_sibling(child);
    }
}
