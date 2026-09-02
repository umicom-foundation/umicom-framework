/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/clipboard_gtk4.c
 *
 * PURPOSE:
 *   Provide private GTK4 clipboard synchronisation helpers for future workbench
 *   commands while the public clipboard contract stays toolkit-neutral.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

/*
 * Provide the gtk4 clipboard set text operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_clipboard_set_text(UmiGtk4Adapter *adapter, const char *text)
{
    GdkClipboard *clipboard;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->window == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    clipboard = gdk_display_get_clipboard(
        gtk_widget_get_display(GTK_WIDGET(adapter->window)));
    gdk_clipboard_set_text(clipboard, text);
    return UMI_STATUS_OK;
}
