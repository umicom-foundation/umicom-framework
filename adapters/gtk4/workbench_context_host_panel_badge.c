/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_host_panel_badge.c
 *
 * PURPOSE:
 *   Render one panel's active link-group badge with semantic colour classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "workbench_context_host_internal.h"
static void remove_colour_classes(GtkWidget *badge)
{
    static const char *classes[]={"umicom-context-group-none","umicom-context-group-red","umicom-context-group-orange",
        "umicom-context-group-yellow","umicom-context-group-green","umicom-context-group-cyan",
        "umicom-context-group-blue","umicom-context-group-purple","umicom-context-group-magenta"};
    size_t i;for(i=0U;i<sizeof(classes)/sizeof(classes[0]);++i)gtk_widget_remove_css_class(badge,classes[i]);
}
UmiStatus umi_workbench_context_host_gtk4_panel_badge_refresh(
    GtkWidget *badge,UmiWorkbenchContextHost *host,const char *endpoint_id)
{
    const UmiWorkbenchContextHostEndpoint *e;const UmiWorkbenchContextLinkGroupProfile *g=NULL;
    if(!GTK_IS_LABEL(badge)||!host||!endpoint_id)return UMI_STATUS_INVALID_ARGUMENT;
    e=umi_workbench_context_host_endpoint_registry_find_const(&host->endpoints,endpoint_id);
    if (e == NULL) return UMI_STATUS_NOT_FOUND;
    if (e->group_id[0] != '\0') {
        g = umi_workbench_context_link_group_catalogue_find_const(
            &host->link_service->groups, e->group_id);
    }
    gtk_label_set_text(GTK_LABEL(badge),g?(g->title[0]?g->title:g->group_id):"Unlinked");remove_colour_classes(badge);
    gtk_widget_add_css_class(badge,umi_wch_colour_css_class(g?g->colour:UMI_CONTEXT_COLOUR_NONE));
    gtk_widget_set_tooltip_text(badge,e->panel_id);return UMI_STATUS_OK;
}
GtkWidget *umi_workbench_context_host_gtk4_panel_badge_new(UmiWorkbenchContextHost *host,const char *endpoint_id)
{
    GtkWidget *badge=gtk_label_new("Unlinked");gtk_widget_add_css_class(badge,"umicom-context-panel-badge");
    if (host != NULL && endpoint_id != NULL) {
        (void)umi_workbench_context_host_gtk4_panel_badge_refresh(
            badge, host, endpoint_id);
    }
    return badge;
}
