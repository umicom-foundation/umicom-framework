/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_link_group_picker.c
 *
 * PURPOSE:
 *   Render the reusable GTK4 context-link group picker surface.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/gtk4.h"

GtkWidget *umi_workbench_context_link_gtk4_group_picker_new(
    const UmiWorkbenchContextLinkService *service)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    size_t index;
    if (service == NULL) return box;
    for (index = 0U; index < service->groups.count; ++index) {
        const UmiWorkbenchContextLinkGroupProfile *profile = &service->groups.items[index];
        GtkWidget *button = gtk_button_new_with_label(
            profile->title[0] != '\0' ? profile->title : profile->group_id);
        gtk_widget_set_tooltip_text(button, umi_context_colour_text(profile->colour));
        gtk_box_append(GTK_BOX(box), button);
    }
    return box;
}
