/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/clipboard_gtk4.c
 *
 * PURPOSE:
 *   Provide private GTK4 clipboard synchronisation helpers for future workbench
 *   commands while the public clipboard contract stays toolkit-neutral.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

UmiStatus umi_gtk4_clipboard_set_text(UmiGtk4Adapter *adapter, const char *text)
{
    GdkClipboard *clipboard;
    if (adapter == NULL || adapter->window == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    clipboard = gdk_display_get_clipboard(
        gtk_widget_get_display(GTK_WIDGET(adapter->window)));
    gdk_clipboard_set_text(clipboard, text);
    return UMI_STATUS_OK;
}
