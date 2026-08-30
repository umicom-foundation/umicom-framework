/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_host_endpoint_list.c
 *
 * PURPOSE:
 *   Render the context-aware panel endpoint inventory as a GTK4 list.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workbench_context_host_internal.h"
GtkWidget *umi_workbench_context_host_gtk4_endpoint_list_new(UmiWorkbenchContextHost *host)
{
    GtkWidget *list=gtk_list_box_new();UmiWorkbenchContextHostEndpointProjection p;size_t i;
    if(!host||umi_workbench_context_host_endpoint_projection_build(host,&p)!=UMI_STATUS_OK)return list;
    for(i=0U;i<p.count;++i){
        const UmiWorkbenchContextHostEndpointProjectionRow *r=&p.rows[i];
        GtkWidget *box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);GtkWidget *name=gtk_label_new(r->display_name);
        GtkWidget *group=gtk_label_new(r->group_id[0]?r->group_id:"Unlinked");
        gtk_widget_set_hexpand(name,TRUE);gtk_widget_set_halign(name,GTK_ALIGN_START);
        gtk_widget_set_tooltip_text(name,r->panel_id);gtk_box_append(GTK_BOX(box),name);gtk_box_append(GTK_BOX(box),group);
        gtk_list_box_append(GTK_LIST_BOX(list),box);
    }
    return list;
}
