/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/tooltip.h
 *
 * PURPOSE:
 *   Format bounded analytical tooltip content without renderer markup.
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
#ifndef UMICOM_UI_ANALYTICS_TOOLTIP_H
#define UMICOM_UI_ANALYTICS_TOOLTIP_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics tooltip data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsTooltip { char title[UMI_ANALYTICS_TEXT_CAPACITY]; char text[UMI_ANALYTICS_VALUE_CAPACITY]; double x; double y; int visible; } UmiAnalyticsTooltip;
/**
 * Copy analytics tooltip into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_analytics_tooltip_set(UmiAnalyticsTooltip *tooltip,const char *title,double x,double y);

#ifdef __cplusplus
}
#endif

#endif
