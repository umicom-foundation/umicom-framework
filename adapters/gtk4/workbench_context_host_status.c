/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_host_status.c
 *
 * PURPOSE:
 *   Render concise active-group and routing state in GTK4.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workbench_context_host_internal.h"
UmiStatus umi_workbench_context_host_gtk4_status_refresh(GtkWidget *status,UmiWorkbenchContextHost *host)
{
    UmiWorkbenchContextHostStatusProjection p;UmiStatus s;
    if(!GTK_IS_LABEL(status)||!host)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_workbench_context_host_status_projection_build(host,&p);if(s!=UMI_STATUS_OK)return s;
    gtk_label_set_text(GTK_LABEL(status),p.summary);
    gtk_widget_set_tooltip_text(status,p.active_context_id[0]?p.active_context_id:"No active context");
    return UMI_STATUS_OK;
}
GtkWidget *umi_workbench_context_host_gtk4_status_new(UmiWorkbenchContextHost *host)
{
    GtkWidget *label=gtk_label_new("Context links");
    gtk_widget_add_css_class(label,"umicom-context-status");
    gtk_widget_set_hexpand(label,TRUE);gtk_widget_set_halign(label,GTK_ALIGN_END);
    if(host)(void)umi_workbench_context_host_gtk4_status_refresh(label,host);
    return label;
}
