/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_host_group_picker.c
 *
 * PURPOSE:
 *   Render active context groups as reusable GTK4 colour-link buttons.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workbench_context_host_internal.h"
static void group_clicked(GtkButton *button,gpointer user_data)
{
    UmiWorkbenchContextHost *host=(UmiWorkbenchContextHost*)user_data;
    const char *group=(const char*)g_object_get_data(G_OBJECT(button),UMI_WCH_DATA_GROUP);
    GtkWidget *picker=gtk_widget_get_parent(GTK_WIDGET(button));
    if(host&&group){(void)umi_workbench_context_host_set_active_group(host,group);
        if(picker)(void)umi_workbench_context_host_gtk4_group_picker_refresh(picker,host);}
}
UmiStatus umi_workbench_context_host_gtk4_group_picker_refresh(
    GtkWidget *picker,UmiWorkbenchContextHost *host)
{
    UmiWorkbenchContextHostGroupPickerProjection p;size_t i;UmiStatus s;
    if(!GTK_IS_BOX(picker)||!host)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_workbench_context_host_group_picker_projection_build(host,&p);if(s!=UMI_STATUS_OK)return s;
    umi_wch_clear_children(picker);
    for(i=0U;i<p.count;++i){
        const UmiWorkbenchContextHostGroupPickerRow *r=&p.rows[i];
        GtkWidget *button=gtk_button_new_with_label(r->title[0]?r->title:r->group_id);
        gtk_widget_add_css_class(button,"umicom-context-group-button");
        gtk_widget_add_css_class(button,umi_wch_colour_css_class(r->colour));
        if(r->active)gtk_widget_add_css_class(button,"suggested-action");
        gtk_widget_set_tooltip_text(button,r->group_id);
        g_object_set_data_full(G_OBJECT(button),UMI_WCH_DATA_GROUP,g_strdup(r->group_id),g_free);
        g_signal_connect(button,"clicked",G_CALLBACK(group_clicked),host);
        gtk_box_append(GTK_BOX(picker),button);
    }
    return UMI_STATUS_OK;
}
GtkWidget *umi_workbench_context_host_gtk4_group_picker_new(UmiWorkbenchContextHost *host)
{
    GtkWidget *picker=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,4);
    gtk_widget_add_css_class(picker,"umicom-context-group-picker");
    g_object_set_data(G_OBJECT(picker),UMI_WCH_DATA_HOST,host);
    if(host)(void)umi_workbench_context_host_gtk4_group_picker_refresh(picker,host);
    return picker;
}
