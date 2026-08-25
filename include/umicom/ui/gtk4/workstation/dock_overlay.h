/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/dock_overlay.h
 *
 * PURPOSE:
 *   Render visual docking targets and drop-preview geometry for the Framework docking/edit-layout workflow.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_DOCK_OVERLAY_H
#define UMICOM_UI_GTK4_WORKSTATION_DOCK_OVERLAY_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/dock_preview.h"

GtkWidget *umi_gtk4_ws_dock_overlay_create(GtkWidget *content);
UmiStatus umi_gtk4_ws_dock_overlay_set_preview(GtkWidget *overlay, const UmiWsDockPreview *preview, const char *label);

#endif
