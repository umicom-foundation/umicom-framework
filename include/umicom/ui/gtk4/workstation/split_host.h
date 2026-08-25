/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/split_host.h
 *
 * PURPOSE:
 *   Render semantic split regions as GTK4 paned hosts with ratio-aware child placement.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_SPLIT_HOST_H
#define UMICOM_UI_GTK4_WORKSTATION_SPLIT_HOST_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/split_region.h"

GtkWidget *umi_gtk4_ws_split_host_create(const UmiWsSplitRegion *split, UmiUiOrientation orientation, GtkWidget *first, GtkWidget *second);

#endif
