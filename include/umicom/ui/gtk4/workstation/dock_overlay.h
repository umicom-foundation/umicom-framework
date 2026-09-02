/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/dock_overlay.h
 *
 * PURPOSE:
 *   Render visual docking targets and drop-preview geometry for the Framework docking/edit-layout workflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_DOCK_OVERLAY_H
#define UMICOM_UI_GTK4_WORKSTATION_DOCK_OVERLAY_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/dock_preview.h"

/**
 * Initialise gtk4 ws dock overlay from caller-provided values so later operations receive
 * a known state.
 */
GtkWidget *umi_gtk4_ws_dock_overlay_create(GtkWidget *content);
/**
 * Provide the gtk4 ws dock overlay set preview operation used by this module and its
 * client applications.
 */
UmiStatus umi_gtk4_ws_dock_overlay_set_preview(GtkWidget *overlay, const UmiWsDockPreview *preview, const char *label);

#endif
