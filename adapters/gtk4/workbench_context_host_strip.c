/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_host_strip.c
 *
 * PURPOSE:
 *   Render group selection, navigation, pinning and status as a reusable workbench strip.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "workbench_context_host_internal.h"
#include <stdio.h>
static void refresh_parent(GtkWidget *button,UmiWorkbenchContextHost *host)
{
    GtkWidget *strip=gtk_widget_get_parent(button);
    while(strip&&g_object_get_data(G_OBJECT(strip),UMI_WCH_DATA_HOST)!=host)
        strip=gtk_widget_get_parent(strip);
    if(strip)(void)umi_workbench_context_host_gtk4_strip_refresh(strip,host);
}
static void back_clicked(GtkButton *button,gpointer data)
{UmiWorkbenchContextHost *host=(UmiWorkbenchContextHost*)data;if(host)(void)umi_workbench_context_host_back(host,0U);refresh_parent(GTK_WIDGET(button),host);}
static void forward_clicked(GtkButton *button,gpointer data)
{UmiWorkbenchContextHost *host=(UmiWorkbenchContextHost*)data;if(host)(void)umi_workbench_context_host_forward(host,0U);refresh_parent(GTK_WIDGET(button),host);}
static void pin_clicked(GtkButton *button,gpointer data)
{
    UmiWorkbenchContextHost *host=(UmiWorkbenchContextHost*)data;char pin[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];int n;
    if (host == NULL) return;
    n = snprintf(
        pin,
        sizeof(pin),
        "pin-%llu",
        (unsigned long long)host->revision);
    if (n >= 0 && (size_t)n < sizeof(pin)) {
        (void)umi_workbench_context_host_pin(host, pin, 0U);
    }
    refresh_parent(GTK_WIDGET(button), host);
}
UmiStatus umi_workbench_context_host_gtk4_strip_refresh(GtkWidget *strip,UmiWorkbenchContextHost *host)
{
    GtkWidget *picker,*status;if(!GTK_IS_BOX(strip)||!host)return UMI_STATUS_INVALID_ARGUMENT;
    picker=(GtkWidget*)g_object_get_data(G_OBJECT(strip),"umicom-context-picker");
    status=(GtkWidget*)g_object_get_data(G_OBJECT(strip),UMI_WCH_DATA_STATUS);
    if(picker)(void)umi_workbench_context_host_gtk4_group_picker_refresh(picker,host);
    if(status)(void)umi_workbench_context_host_gtk4_status_refresh(status,host);
    return UMI_STATUS_OK;
}
GtkWidget *umi_workbench_context_host_gtk4_strip_new(UmiWorkbenchContextHost *host)
{
    GtkWidget *strip=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
    GtkWidget *back=gtk_button_new_with_label("‹");
    GtkWidget *forward=gtk_button_new_with_label("›");
    GtkWidget *pin=gtk_button_new_with_label("Pin");
    GtkWidget *picker=umi_workbench_context_host_gtk4_group_picker_new(host);
    GtkWidget *status=umi_workbench_context_host_gtk4_status_new(host);
    gtk_widget_add_css_class(strip,"umicom-context-link-strip");
    gtk_widget_set_tooltip_text(back,"Previous linked context");gtk_widget_set_tooltip_text(forward,"Next linked context");
    gtk_widget_set_tooltip_text(pin,"Pin active context");
    g_signal_connect(back,"clicked",G_CALLBACK(back_clicked),host);g_signal_connect(forward,"clicked",G_CALLBACK(forward_clicked),host);
    g_signal_connect(pin,"clicked",G_CALLBACK(pin_clicked),host);
    gtk_box_append(GTK_BOX(strip),back);gtk_box_append(GTK_BOX(strip),forward);gtk_box_append(GTK_BOX(strip),pin);
    gtk_box_append(GTK_BOX(strip),picker);gtk_box_append(GTK_BOX(strip),status);
    g_object_set_data(G_OBJECT(strip),UMI_WCH_DATA_HOST,host);
    g_object_set_data(G_OBJECT(strip),"umicom-context-picker",picker);g_object_set_data(G_OBJECT(strip),UMI_WCH_DATA_STATUS,status);
    return strip;
}
