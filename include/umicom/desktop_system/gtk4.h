/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop_system/gtk4.h
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESKTOP_SYSTEM_GTK4_H
#define UMICOM_DESKTOP_SYSTEM_GTK4_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Call these entry points on the owning GTK main context.
 * A floating read-only panel. Construction does not collect data. Refresh uses
 * one worker at a time; no worker holds or finalises GTK objects. */
GtkWidget *UmiDesktopSystemGtk4PanelNew(void);
UmiStatus UmiDesktopSystemGtk4Refresh(GtkWidget *panel);
/* Add a System Centre launcher without replacing any existing Desk content. */
UmiStatus UmiDesktopSystemGtk4Attach(GtkWidget *rootBox);
#ifdef __cplusplus
}
#endif
#endif
