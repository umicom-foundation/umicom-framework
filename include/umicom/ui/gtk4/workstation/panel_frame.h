/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/panel_frame.h
 *
 * PURPOSE:
 *   Render Framework panel chrome around arbitrary GTK4 child content without moving panel ownership into the adapter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_PANEL_FRAME_H
#define UMICOM_UI_GTK4_WORKSTATION_PANEL_FRAME_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/panel_chrome.h"

GtkWidget *umi_gtk4_ws_panel_frame_create(const UmiWsPanelChrome *chrome, GtkWidget *child);

#endif
