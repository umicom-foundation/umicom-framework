/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/split_host.h
 *
 * PURPOSE:
 *   Render semantic split regions as GTK4 paned hosts with ratio-aware child placement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_SPLIT_HOST_H
#define UMICOM_UI_GTK4_WORKSTATION_SPLIT_HOST_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/split_region.h"

/**
 * Initialise gtk4 ws split host from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_split_host_create(const UmiWsSplitRegion *split, UmiUiOrientation orientation, GtkWidget *first, GtkWidget *second);

#endif
