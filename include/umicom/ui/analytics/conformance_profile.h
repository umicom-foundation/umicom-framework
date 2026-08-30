/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/conformance_profile.h
 *
 * PURPOSE:
 *   Declare renderer conformance requirements for shared analytics surfaces.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral analytics capability extends canonical Umicom::ui.
 *   Existing Design System chart/gauge/heatmap specs and workstation surfaces
 *   remain authoritative; GTK4, Qt6, Native Web and thin applications render
 *   the same Framework-owned analytics semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_CONFORMANCE_PROFILE_H
#define UMICOM_UI_ANALYTICS_CONFORMANCE_PROFILE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsConformanceProfile { int charts; int gauges; int heatmaps; int dashboards; int keyboard_access; int high_dpi; int export_text; } UmiAnalyticsConformanceProfile;
void umi_analytics_conformance_profile_required(UmiAnalyticsConformanceProfile *p);
int umi_analytics_conformance_profile_satisfied(const UmiAnalyticsConformanceProfile *required,const UmiAnalyticsConformanceProfile *actual);

#ifdef __cplusplus
}
#endif

#endif
