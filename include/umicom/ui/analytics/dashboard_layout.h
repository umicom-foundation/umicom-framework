/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/dashboard_layout.h
 *
 * PURPOSE:
 *   Describe named responsive dashboard layout metadata around a canonical grid.
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
#ifndef UMICOM_UI_ANALYTICS_DASHBOARD_LAYOUT_H
#define UMICOM_UI_ANALYTICS_DASHBOARD_LAYOUT_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/analytics/dashboard_grid.h"
/**
 * Represent the analytics dashboard layout data shared with callers of this public
 * contract.
 */
typedef struct UmiAnalyticsDashboardLayout { char id[UMI_ANALYTICS_ID_CAPACITY]; char title[UMI_ANALYTICS_TEXT_CAPACITY]; UmiAnalyticsDashboardGrid grid; int compact; } UmiAnalyticsDashboardLayout;
/**
 * Initialise analytics dashboard layout from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_dashboard_layout_init(UmiAnalyticsDashboardLayout *layout,const char *id,const char *title,uint16_t rows,uint16_t columns);

#ifdef __cplusplus
}
#endif

#endif
