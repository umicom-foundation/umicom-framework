/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/language_connection.h
 * PURPOSE: Present an explicit worker-based native language-server connection check.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_LANGUAGE_CONNECTION_H
#define UMICOM_UI_GTK4_LANGUAGE_CONNECTION_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Create a floating panel on the GTK thread and place it in a parent container.
 * Opening it executes nothing. The optional workspace folder is copied into an
 * editable draft. Check starts the selected local executable on a worker,
 * initializes its protocol and closes it; no document text is sent and no
 * editor session is configured. The executable can inspect its working folder.
 * Unmapping or destroying the panel requests cancellation and suppresses stale
 * completion. A worker may finish its native cleanup after the UI disappears.
 * Callers retain no controller handle or callback context. */
    UmiStatus UmiGtk4LanguageConnectionPanelCreate(const char *workspaceDirectory, GtkWidget **outPanel);
#ifdef __cplusplus
}
#endif
#endif
