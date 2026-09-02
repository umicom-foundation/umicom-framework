/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_host_strip.c
 *
 * PURPOSE:
 *   Render group selection, navigation, pinning and status as a reusable workbench strip.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workbench_context_host_internal.h"
#include <stdio.h>
/* Provide the refresh parent operation used by this module and its client applications. */
static void refresh_parent(GtkWidget *button,UmiWorkbenchContextHost *host)
{
    GtkWidget *strip=gtk_widget_get_parent(button);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while(strip&&g_object_get_data(G_OBJECT(strip),UMI_WCH_DATA_HOST)!=host)
        strip=gtk_widget_get_parent(strip);
    /* Apply this branch only when its contract condition is satisfied. */
    if(strip)(void)umi_workbench_context_host_gtk4_strip_refresh(strip,host);
}
/* Provide the back clicked operation used by this module and its client applications. */
static void back_clicked(GtkButton *button,gpointer data)
{UmiWorkbenchContextHost *host=(UmiWorkbenchContextHost*)data;/* Apply this branch only when its contract condition is satisfied. */ if(host)(void)umi_workbench_context_host_back(host,0U);refresh_parent(GTK_WIDGET(button),host);}
/* Provide the forward clicked operation used by this module and its client applications. */
static void forward_clicked(GtkButton *button,gpointer data)
{UmiWorkbenchContextHost *host=(UmiWorkbenchContextHost*)data;/* Apply this branch only when its contract condition is satisfied. */ if(host)(void)umi_workbench_context_host_forward(host,0U);refresh_parent(GTK_WIDGET(button),host);}
/* Provide the pin clicked operation used by this module and its client applications. */
static void pin_clicked(GtkButton *button,gpointer data)
{
    UmiWorkbenchContextHost *host=(UmiWorkbenchContextHost*)data;char pin[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];int n;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL) return;
    n = snprintf(
        pin,
        sizeof(pin),
        "pin-%llu",
        (unsigned long long)host->revision);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (n >= 0 && (size_t)n < sizeof(pin)) {
        (void)umi_workbench_context_host_pin(host, pin, 0U);
    }
    refresh_parent(GTK_WIDGET(button), host);
}
/*
 * Provide the workbench context host gtk4 strip refresh operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_gtk4_strip_refresh(GtkWidget *strip,UmiWorkbenchContextHost *host)
{
    GtkWidget *picker,*status;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!GTK_IS_BOX(strip)||!host)return UMI_STATUS_INVALID_ARGUMENT;
    picker=(GtkWidget*)g_object_get_data(G_OBJECT(strip),"umicom-context-picker");
    status=(GtkWidget*)g_object_get_data(G_OBJECT(strip),UMI_WCH_DATA_STATUS);
    /* Apply this branch only when its contract condition is satisfied. */
    if(picker)(void)umi_workbench_context_host_gtk4_group_picker_refresh(picker,host);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status)(void)umi_workbench_context_host_gtk4_status_refresh(status,host);
    return UMI_STATUS_OK;
}
/*
 * Provide the workbench context host gtk4 strip new operation used by this module and its
 * client applications.
 */
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
