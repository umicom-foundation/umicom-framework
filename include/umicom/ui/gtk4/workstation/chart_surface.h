/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/chart_surface.h
 *
 * PURPOSE:
 *   Render a reusable GTK4 chart viewport scaffold for Trader, profiling, analytics and operational time-series panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_CHART_SURFACE_H
#define UMICOM_UI_GTK4_WORKSTATION_CHART_SURFACE_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/chart_surface.h"

GtkWidget *umi_gtk4_ws_chart_surface_create(const UmiWsChartSurface *surface);

#endif
