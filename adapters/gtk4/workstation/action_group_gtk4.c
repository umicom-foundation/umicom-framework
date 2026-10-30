/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/action_group_gtk4.c
 *
 * PURPOSE:
 *   Arrange existing command buttons into rows without recreating controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/action_group.h"
#define ACTION_GROUP_MARKER "umicom-framework-action-group"
#define ACTION_GROUP_LIMIT 64U

GtkWidget *UmiGtk4ActionGroupCreate(void)
{
    GtkWidget *group = gtk_flow_box_new();
    gtk_orientable_set_orientation(GTK_ORIENTABLE(group), GTK_ORIENTATION_HORIZONTAL);
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(group), GTK_SELECTION_NONE);
    gtk_flow_box_set_activate_on_single_click(GTK_FLOW_BOX(group), FALSE);
    gtk_flow_box_set_homogeneous(GTK_FLOW_BOX(group), FALSE);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(group), 1U);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(group), ACTION_GROUP_LIMIT);
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(group), 4U);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(group), 4U);
    gtk_widget_set_hexpand(group, TRUE);
    gtk_widget_set_valign(group, GTK_ALIGN_START);
    gtk_widget_add_css_class(group, "umicom-action-group");
    g_object_set_data(G_OBJECT(group), ACTION_GROUP_MARKER, GINT_TO_POINTER(1));
    return group;
}
UmiStatus UmiGtk4ActionGroupAppend(GtkWidget *group, GtkWidget *button)
{
    if (!GTK_IS_FLOW_BOX(group) ||
        g_object_get_data(G_OBJECT(group), ACTION_GROUP_MARKER) == NULL ||
        !GTK_IS_BUTTON(button) || gtk_widget_get_parent(button) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    unsigned count = 0U;
    for (GtkWidget *child = gtk_widget_get_first_child(group); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        if (++count >= ACTION_GROUP_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* No business callback is installed on the FlowBox. The existing button is
     * the only activation endpoint, retaining the host's command admission. */
    gtk_flow_box_insert(GTK_FLOW_BOX(group), button, -1);
    return UMI_STATUS_OK;
}
