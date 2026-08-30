/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/workspace_strip.h
 *
 * PURPOSE:
 *   Render the saved-layout/workspace strip used for rapid perspective/layout switching across workstation applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_WORKSPACE_STRIP_H
#define UMICOM_UI_GTK4_WORKSTATION_WORKSPACE_STRIP_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/workspace_strip.h"

GtkWidget *umi_gtk4_ws_workspace_strip_create(const UmiWsWorkspaceStrip *strip);

#endif
