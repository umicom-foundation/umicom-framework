/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_host_history.c
 *
 * PURPOSE:
 *   Render recent linked contexts as a reverse-chronological GTK4 list.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workbench_context_host_internal.h"
GtkWidget *umi_workbench_context_host_gtk4_history_new(UmiWorkbenchContextHost *host,const char *group_id)
{
    GtkWidget *list=gtk_list_box_new();UmiWorkbenchContextHostHistoryProjection p;size_t i;
    if(!host||umi_workbench_context_host_history_projection_build(host,group_id,&p)!=UMI_STATUS_OK)return list;
    for(i=0U;i<p.count;++i){
        const UmiWorkbenchContextHostHistoryProjectionRow *r=&p.rows[i];
        GtkWidget *box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);GtkWidget *kind=gtk_label_new(umi_context_kind_text(r->kind));
        GtkWidget *id=gtk_label_new(r->context_id);gtk_widget_set_size_request(kind,110,-1);gtk_widget_set_hexpand(id,TRUE);
        gtk_widget_set_halign(id,GTK_ALIGN_START);gtk_box_append(GTK_BOX(box),kind);gtk_box_append(GTK_BOX(box),id);
        gtk_list_box_append(GTK_LIST_BOX(list),box);
    }
    return list;
}
