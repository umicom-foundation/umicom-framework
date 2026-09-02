/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_host_panel_badge.c
 *
 * PURPOSE:
 *   Render one panel's active link-group badge with semantic colour classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workbench_context_host_internal.h"
/*
 * Provide the remove colour classes operation used by this module and its client
 * applications.
 */
static void remove_colour_classes(GtkWidget *badge)
{
    static const char *classes[]={"umicom-context-group-none","umicom-context-group-red","umicom-context-group-orange",
        "umicom-context-group-yellow","umicom-context-group-green","umicom-context-group-cyan",
        "umicom-context-group-blue","umicom-context-group-purple","umicom-context-group-magenta"};
    size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<sizeof(classes)/sizeof(classes[0]);++i)gtk_widget_remove_css_class(badge,classes[i]);
}
/*
 * Provide the workbench context host gtk4 panel badge refresh operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_gtk4_panel_badge_refresh(
    GtkWidget *badge,UmiWorkbenchContextHost *host,const char *endpoint_id)
{
    const UmiWorkbenchContextHostEndpoint *e;const UmiWorkbenchContextLinkGroupProfile *g=NULL;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!GTK_IS_LABEL(badge)||!host||!endpoint_id)return UMI_STATUS_INVALID_ARGUMENT;
    e=umi_workbench_context_host_endpoint_registry_find_const(&host->endpoints,endpoint_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (e == NULL) return UMI_STATUS_NOT_FOUND;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (e->group_id[0] != '\0') {
        g = umi_workbench_context_link_group_catalogue_find_const(
            &host->link_service->groups, e->group_id);
    }
    gtk_label_set_text(GTK_LABEL(badge),g?(g->title[0]?g->title:g->group_id):"Unlinked");remove_colour_classes(badge);
    gtk_widget_add_css_class(badge,umi_wch_colour_css_class(g?g->colour:UMI_CONTEXT_COLOUR_NONE));
    gtk_widget_set_tooltip_text(badge,e->panel_id);return UMI_STATUS_OK;
}
/*
 * Provide the workbench context host gtk4 panel badge new operation used by this module
 * and its client applications.
 */
GtkWidget *umi_workbench_context_host_gtk4_panel_badge_new(UmiWorkbenchContextHost *host,const char *endpoint_id)
{
    GtkWidget *badge=gtk_label_new("Unlinked");gtk_widget_add_css_class(badge,"umicom-context-panel-badge");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host != NULL && endpoint_id != NULL) {
        (void)umi_workbench_context_host_gtk4_panel_badge_refresh(
            badge, host, endpoint_id);
    }
    return badge;
}
