/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_host_inspector.c
 *
 * PURPOSE:
 *   Render active typed context identity and value rows in GTK4.
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
 * Provide the workbench context host gtk4 inspector refresh operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_gtk4_inspector_refresh(
    GtkWidget *inspector,UmiWorkbenchContextHost *host,const char *group_id)
{
    UmiWorkbenchContextHostInspectorProjection p;size_t i;UmiStatus s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!GTK_IS_BOX(inspector)||!host||!group_id)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_workbench_context_host_inspector_projection_build(host,group_id,&p);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    umi_wch_clear_children(inspector);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!p.available){gtk_box_append(GTK_BOX(inspector),gtk_label_new("No active context"));return UMI_STATUS_OK;}
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<p.row_count;++i){
        GtkWidget *row=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);
        GtkWidget *name=gtk_label_new(p.rows[i].name);GtkWidget *value=gtk_label_new(p.rows[i].value);
        gtk_widget_set_size_request(name,140,-1);gtk_widget_set_halign(name,GTK_ALIGN_START);
        gtk_widget_set_hexpand(value,TRUE);gtk_widget_set_halign(value,GTK_ALIGN_START);gtk_label_set_wrap(GTK_LABEL(value),TRUE);
        gtk_box_append(GTK_BOX(row),name);gtk_box_append(GTK_BOX(row),value);gtk_box_append(GTK_BOX(inspector),row);
    }
    return UMI_STATUS_OK;
}
/*
 * Provide the workbench context host gtk4 inspector new operation used by this module and
 * its client applications.
 */
GtkWidget *umi_workbench_context_host_gtk4_inspector_new(UmiWorkbenchContextHost *host,const char *group_id)
{
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,4);gtk_widget_add_css_class(box,"umicom-context-inspector");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host != NULL && group_id != NULL) {
        (void)umi_workbench_context_host_gtk4_inspector_refresh(
            box, host, group_id);
    }
    return box;
}
