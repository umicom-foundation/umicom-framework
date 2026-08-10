/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/theme_gtk4.c
 *
 * PURPOSE:
 *   Apply semantic Framework theme tokens through a private GTK4 CSS provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

UmiStatus umi_gtk4_apply_theme(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench)
{
    GtkCssProvider *provider;
    GdkDisplay *display;
    (void)workbench;
    if (adapter == NULL || adapter->window == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    display = gtk_widget_get_display(GTK_WIDGET(adapter->window));
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider,
        "window { background: @theme_bg_color; }"
        ".menu { padding: 4px 8px; font-weight: 600; }"
        "label { margin: 2px; }");
    gtk_style_context_add_provider_for_display(display,
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
    return UMI_STATUS_OK;
}
