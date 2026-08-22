/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_host_inbox.c
 *
 * PURPOSE:
 *   Render pending endpoint delivery queues and drop counts for diagnostics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "workbench_context_host_internal.h"
#include <stdio.h>
GtkWidget *umi_workbench_context_host_gtk4_inbox_new(UmiWorkbenchContextHost *host)
{
    GtkWidget *list=gtk_list_box_new();UmiWorkbenchContextHostInboxProjection p;size_t i;
    if(!host||umi_workbench_context_host_inbox_projection_build(host,&p)!=UMI_STATUS_OK)return list;
    for(i=0U;i<p.count;++i){
        const UmiWorkbenchContextHostInboxProjectionRow *r=&p.rows[i];char counts[96U];
        GtkWidget *box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);GtkWidget *panel=gtk_label_new(r->panel_id);
        (void)snprintf(counts,sizeof(counts),"%zu queued · %llu dropped",r->queued_count,(unsigned long long)r->dropped_count);
        GtkWidget *status=gtk_label_new(counts);gtk_widget_set_hexpand(panel,TRUE);gtk_widget_set_halign(panel,GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(box),panel);gtk_box_append(GTK_BOX(box),status);gtk_list_box_append(GTK_LIST_BOX(list),box);
    }
    return list;
}
