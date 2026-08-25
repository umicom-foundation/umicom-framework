/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/chart.h
 *
 * PURPOSE:
 *   Render a reusable chart viewport for trading, observability and analytics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_CHART_H
#define UMICOM_UI_QT6_WORKSTATION_CHART_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/chart_surface.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_ws_chart_descriptor(void);
UmiQt6WidgetHandle umi_qt6_ws_chart_create(const UmiWsChartSurface *surface);

#ifdef __cplusplus
}
#endif
#endif
