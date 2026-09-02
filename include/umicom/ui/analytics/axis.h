/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/axis.h
 *
 * PURPOSE:
 *   Describe one semantic chart axis without renderer-specific coordinates.
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
#ifndef UMICOM_UI_ANALYTICS_AXIS_H
#define UMICOM_UI_ANALYTICS_AXIS_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics axis data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsAxis { char id[UMI_ANALYTICS_ID_CAPACITY]; char label[UMI_ANALYTICS_TEXT_CAPACITY]; UmiAnalyticsAxisScale scale; double minimum; double maximum; int include_zero; } UmiAnalyticsAxis;
/**
 * Initialise analytics axis from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_axis_init(UmiAnalyticsAxis *axis,const char *id,const char *label,UmiAnalyticsAxisScale scale,double minimum,double maximum);
/**
 * Check that analytics axis satisfies its contract before another service relies on it.
 */
int umi_analytics_axis_valid(const UmiAnalyticsAxis *axis);

#ifdef __cplusplus
}
#endif

#endif
